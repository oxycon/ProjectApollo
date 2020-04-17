#include "defs.h"
#include "config.h"

nvm_t nvm;                          // non-volatile memory variables stored in this data structure
uint32_t now;                       // calling millis disables interrupts, which can cause corrupted SoftwareSerial data, so cache millis here only once per loop
uint8_t valve_phase = PHASE_IDLE;   // valve state-machine state
bool run;                           // whether or not to run the valve state-machine
bool run_heartbeat;                 // whether or not to run the heartbeat LEDs
uint32_t debug_timestamp = 0;       // last time the debug output has been written

void setup()
{
	Serial.begin(115200);

	// bringup();

	pinMode(PIN_LED_RED, OUTPUT);
	digitalWrite(PIN_LED_RED, LOW);
	pinMode(PIN_LED_GREEN, OUTPUT);
	digitalWrite(PIN_LED_GREEN, LOW);

	pinMode(PIN_BUTTON, INPUT);
	digitalWrite(PIN_BUTTON, HIGH);

	pinMode(PIN_BUZZER, OUTPUT);
	digitalWrite(PIN_BUZZER, LOW);

	#ifdef USE_PRESSURE_SENSOR
	pinMode(PIN_MPR_RST, OUTPUT);
	pinMode(PIN_MPR_EOC, INPUT);
	pinMode(PIN_AUX2, INPUT);
	pinMode(PIN_AUX3, INPUT);
	#endif

	nvm_read_or_default(&nvm);
	#ifdef USE_OXY_SENSOR
	oxy_init();
	#endif
	#ifdef USE_PRESSURE_SENSOR
	pressure_init();
	#endif
	valves_init();

	run = true;
	run_heartbeat = true;
}

void loop()
{
	now = millis();
	odo_task();
	cmd_task();
	#ifdef USE_OXY_SENSOR
	oxy_task();
	#endif
	#ifdef USE_PRESSURE_SENSOR
	pressure_task();
	#endif
	#ifdef USE_VOLTAGE_MONITOR
	vmon_task();
	#endif

	if (run)
	{
		valve_task();
	}
	else
	{
		// if we are not running, go back to the idle state
		// because we are able to change pin states while run mode is off
		// the idle state will restore to the correct state based on pin states
		valve_phase = PHASE_IDLE;
	}

	if (run_heartbeat)
	{
		heartbeat();
		faults_blink();
		faults_beep();
	}

	if (nvm.debug_mode != false && TIME_HAS_ELAPSED(now, debug_timestamp, 1000))
	{
		debug_timestamp = now;
		Serial.print(F("dbg[")); Serial.print(now, DEC); Serial.print(F("]: "));
		Serial.print(F("phase: "));
		Serial.print(valve_phase);
		Serial.print(F("; "));
		#ifdef USE_PRESSURE_SENSOR
		Serial.print(F("pressure: "));
		pressure_printAll(false, true, false);
		Serial.print(F("; "));
		#endif
		#ifdef USE_OXY_SENSOR
		Serial.print(F("oxy/flow/temp: "));
		Serial.print(o2sens_getConcentration16(), DEC); Serial.print(F(", "));
		Serial.print(o2sens_getFlowRate16(), DEC); Serial.print(F(", "));
		Serial.print(o2sens_getTemperature16(), DEC);
		Serial.print(F("; "));
		#endif
		#ifdef USE_VOLTAGE_MONITOR
		Serial.print(F("volt: "));
		Serial.print(voltage_read(), DEC);
		Serial.print(F("; "));
		#endif
		Serial.print(F("faults: "));
		Serial.print(current_faults, HEX);
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
				if (nvm.time_stage_2way > 0) // check if 2 way valve is enabled
				{
					valve_2way_time = now;
					digitalWrite(PIN_COIL_2WAY,
						#ifndef INVERT_2WAY_COIL
							HIGH
						#else
							LOW
						#endif
						);
					valve_phase += 1; // next phase, no chance of overflow
				}
				else
				{
					// 2 way valve is disabled, simply goto the next 5 way valve phase
					valve_5way_time = now;
					if (valve_phase == PHASE_5WAY_OFF_2WAY_OFF)
					{
						valve_phase = PHASE_5WAY_ON_2WAY_OFF;
						digitalWrite(PIN_COIL_5WAY, HIGH);
					}
					else
					{
						valve_phase = PHASE_5WAY_OFF_2WAY_OFF;
						digitalWrite(PIN_COIL_5WAY, LOW);
					}
				}
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
		case PHASE_FAULT:
			// DO NOTHING
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

	#ifdef USE_COIL_CHECK
	valve_check(PIN_COILCHECK_5WAY);
	if (nvm.time_stage_2way > 0)
	{
		valve_check(PIN_COILCHECK_2WAY);
	}
	#endif
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

			if (supposed_fault == FAULTCODE_COIL_5WAY_SHORT || supposed_fault == FAULTCODE_COIL_2WAY_SHORT) {
				// short circuit is not healthy, shut it down
				digitalWrite((pin == PIN_COILCHECK_5WAY) ? PIN_COIL_5WAY : PIN_COIL_2WAY, LOW);
				valve_phase = PHASE_FAULT;
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

	#ifdef USE_COIL_CHECK
	pinMode(PIN_COILCHECK_1, INPUT);
	digitalWrite(PIN_COILCHECK_1, HIGH);
	pinMode(PIN_COILCHECK_2, INPUT);
	digitalWrite(PIN_COILCHECK_2, HIGH);
	#endif
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

// use the green LED to blink a pattern indicating which valve phase we are in
void heartbeat()
{
	uint32_t now_mod;
	uint32_t now_lim;
	if (run == false)
	{
		// short but infrequent blink
		now_mod = 1000;
		now_lim = 200;
	}
	else
	{
		if (digitalRead(PIN_COIL_2WAY)
			#ifndef INVERT_2WAY_COIL
				!=
			#else
				==
			#endif
				LOW)
		{
			// we are in the 2way-on states
			// do a fast rapid blink
			now_mod = 400;
			now_lim = 200;
		}
		else
		{
			// do a slow long blink
			now_mod = 800;
			now_lim = 600;
		}
	}

	digitalWrite(PIN_LED_GREEN, (now & now_mod) < now_lim ? HIGH : LOW);
}
