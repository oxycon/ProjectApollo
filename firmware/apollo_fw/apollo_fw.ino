#include "defs.h"
#include "config.h"

nvm_t nvm;                          // non-volatile memory variables stored in this data structure
uint32_t now;                       // calling millis disables interrupts, which can cause corrupted SoftwareSerial data, so cache millis here only once per loop
uint8_t valve_phase = PHASE_IDLE;   // valve state-machine state
bool run;                           // whether or not to run the valve state-machine
uint32_t debug_timestamp = 0;       // last time the debug output has been written

void setup()
{
	Serial.begin(115200);

	pinMode(PIN_LED_RED, OUTPUT);
	digitalWrite(PIN_LED_RED, LOW);
	pinMode(PIN_LED_GREEN, OUTPUT);
	digitalWrite(PIN_LED_GREEN, LOW);

	pinMode(PIN_BUTTON, INPUT);
	digitalWrite(PIN_BUTTON, HIGH);

	pinMode(PIN_BUZZER, OUTPUT);
	digitalWrite(PIN_BUZZER, LOW);

	pinMode(PIN_MPR_RST, OUTPUT);
	pinMode(PIN_MPR_EOC, INPUT);
	pinMode(PIN_AUX2, INPUT);
	pinMode(PIN_AUX3, INPUT);

	nvm_read_or_default(&nvm);
	oxy_init();
	pressure_init();
	valves_init();

	run = true;
}

void loop()
{
	now = millis();
	odo_task();
	cmd_task();
	oxy_task();
	pressure_task();
	vmon_task();

	if (run)
	{
		valve_task();
		faults_blink();
		faults_beep();
	}
	else
	{
		// if we are not running, go back to the idle state
		// because we are able to change pin states while run mode is off
		// the idle state will restore to the correct state based on pin states
		valve_phase = PHASE_IDLE;
	}

	if (nvm.debug_mode != false && TIME_HAS_ELAPSED(now, debug_timestamp, 1000))
	{
		Serial.print(F("dbg[")); Serial.print(now, DEC); Serial.print(F("]:"));
		// TODO: report sensor readings here
		Serial.println();
	}
}

// timestamps for when the state-machine transitioned state
uint32_t valve_phase_time;
uint32_t valve_5way_time;
uint32_t valve_2way_time;

void valve_task()
{
	switch(valve_phase)
	{
		case PHASE_5WAY_OFF_2WAY_OFF:
		case PHASE_5WAY_ON_2WAY_OFF:
			if (TIME_HAS_ELAPSED(now, valve_phase_time, nvm.time_stage_5way))
			{
				valve_phase_time = now;
				valve_2way_time = now;
				digitalWrite(PIN_COIL_2WAY,
					#ifndef INVERT_2WAY_COIL
						HIGH
					#else
						LOW
					#endif
					);
				valve_phase += 1; // next phase, no chance of overflow
				if (nvm.debug_mode) { Serial.print(F("valve next phase ")); Serial.println(valve_phase, DEC); }
			}
			break;
		case PHASE_5WAY_OFF_2WAY_ON:
		case PHASE_5WAY_ON_2WAY_ON:
			if (TIME_HAS_ELAPSED(now, valve_phase_time, nvm.time_stage_2way))
			{
				valve_phase_time = now;
				valve_2way_time = now;
				digitalWrite(PIN_COIL_2WAY,
					#ifndef INVERT_2WAY_COIL
						LOW
					#else
						HIGH
					#endif
					);
				if (nvm.time_stage_pause <= 0)
				{
					// no pause period, skip the pause phase
					valve_phase += 2;
					valve_5way_time = now;
					if (valve_phase >= PHASE_WRAP)
					{
						valve_phase = PHASE_5WAY_OFF_2WAY_OFF;
						digitalWrite(PIN_COIL_5WAY, LOW);
					}
					else
					{
						// in PHASE_5WAY_ON_2WAY_OFF
						digitalWrite(PIN_COIL_5WAY, HIGH);
					}
				}
				else
				{
					valve_phase += 1; // goto next phase, no chance of overflow
				}
				if (nvm.debug_mode) { Serial.print(F("valve next phase ")); Serial.println(valve_phase, DEC); }
			}
			break;
		case PHASE_5WAY_OFF_2WAY_PAUSE:
		case PHASE_5WAY_ON_2WAY_PAUSE:
			if (TIME_HAS_ELAPSED(now, valve_phase_time, nvm.time_stage_2way))
			{
				valve_phase_time = now;
				valve_5way_time = now;
				valve_phase += 1; // goto next phase
				if (valve_phase >= PHASE_WRAP)
				{
					valve_phase = PHASE_5WAY_OFF_2WAY_OFF;
					digitalWrite(PIN_COIL_5WAY, LOW);
				}
				else
				{
					// in PHASE_5WAY_ON_2WAY_OFF
					digitalWrite(PIN_COIL_5WAY, HIGH);
				}
				if (nvm.debug_mode) { Serial.print(F("valve next phase ")); Serial.println(valve_phase, DEC); }
			}
			break;
		case PHASE_IDLE:
		case default:
			valve_phase_time = now;
			valve_5way_time = now;
			valve_2way_time = now;
			// go to next state depending on pin states
			if (digitalRead(PIN_COIL_2WAY)
				#ifndef INVERT_2WAY_COIL
					==
				#else
					!=
				#endif
					LOW)
			{
				valve_phase = digitalRead(PIN_COIL_5WAY) == LOW ? PHASE_5WAY_OFF_2WAY_OFF ? PHASE_5WAY_ON_2WAY_OFF;
			}
			else
			{
				valve_phase = digitalRead(PIN_COIL_5WAY) == LOW ? PHASE_5WAY_OFF_2WAY_ON ? PHASE_5WAY_ON_2WAY_ON;
			}
			if (nvm.debug_mode) { Serial.print(F("valve next phase ")); Serial.println(valve_phase, DEC); }
			break;
	}

	valve_check(PIN_COILCHECK_5WAY);
	valve_check(PIN_COILCHECK_2WAY);
}

void valve_check(uint8_t pin)
{
	uint32_t supposed_fault;
	uint8_t expected_pin_state, actual_pin_state;
	if (TIME_HAS_ELAPSED(now, (pin == PIN_COILCHECK_5WAY) ? valve_5way_time : valve_2way_time, COIL_SETTLE_TIME))
	{
		// the solenoid takes time to move, and then takes time for the back-EMF to dissipate
		// we only check after the settle time

		actual_pin_state = digitalRead(PIN_COILCHECK_5WAY) == LOW ? LOW : HIGH;
		// what we are checking depends on if the coil is ON or OFF
		if (digitalRead((pin == PIN_COILCHECK_5WAY) ? PIN_COIL_5WAY : PIN_COIL_2WAY) != LOW)
		{
			// when a coil is ON, the optocoupler input is LOW normally, the output should be HIGH
			// when the coil is short-circuit, the fuse will be blown, and thus, the optocoupler input will be HIGH, and the output will be LOW
			supposed_fault = (pin == PIN_COILCHECK_5WAY) ? FAULTCODE_COIL_5WAY_SHORT : FAULTCODE_COIL_2WAY_SHORT;
			expected_pin_state = HIGH;
		}
		else
		{
			// when a coil is OFF, the optocoupler input is HIGH normally, the output should be LOW
			// when the coil is open-circuit, the optocoupler's input will never be HIGH, so the output will never be LOW
			supposed_fault = (pin == PIN_COILCHECK_5WAY) ? FAULTCODE_COIL_5WAY_OPEN : FAULTCODE_COIL_2WAY_OPEN;
			expected_pin_state = LOW;
		}

		// check if the actual pin state is expected
		if (actual_pin_state != expected_pin_state)
		{
			// is it a new fault?
			if ((current_faults & supposed_fault) == 0)
			{
				// register new fault
				current_faults |= supposed_fault;
				fault_save(current_faults);
			}
		}
		else
		{
			current_faults &= ~supposed_fault;
		}
	}
}

void valves_init()
{
	pinMode(PIN_COIL_1, OUTPUT);
	digitalWrite(PIN_COIL_1, LOW);
	pinMode(PIN_COIL_2, OUTPUT);
	digitalWrite(PIN_COIL_1, LOW);

	pinMode(PIN_COILCHECK_1, INPUT);
	digitalWrite(PIN_COILCHECK_1, HIGH);
	pinMode(PIN_COILCHECK_2, INPUT);
	digitalWrite(PIN_COILCHECK_2, HIGH);
}

uint32_t odo_timestamp = 0;

void odo_task()
{
	// if an hour has passed, increase the odometer and save it
	if (TIME_HAS_ELAPSED(now, odo_timestamp, 60 * 60 * 1000))
	{
		odo_timestamp = now;
		nvm.odometer_hours++;
		nvm_write(&nvm);
	}
}

uint32_t odo_millis()
{
	// instead of using millis itself, use the millis since the last time the hour counter incremented
	return now - odo_timestamp;
}
