#include <EEPROM.h>
#include <avr/eeprom.h>

#define FAULT_EEPROM_END (E2END - sizeof(fault_t) - 1)

void fault_save(uint32_t x)
{
	static uint8_t seq = 0;
	uint16_t addr;
	fault_t tmp;
	if (x == 0) {
		return;
	}
	seq += 1;
	addr = fault_findNextSpot(&seq);
	if (seq >= 0xFF || seq == 0x00) {
		seq = 1;
	}
	tmp.seq = seq;
	tmp.odometer_hours = nvm.odometer_hours;
	tmp.millis = odo_millis();
	tmp.fault_code = fault_code;
	eeprom_update_block((void*)&tmp, (const void *)i, sizeof(fault_t));
	if (nvm.debug_mode) { Serial.print(F("fault: 0x")); Serial.println(x, HEX); }
}

uint16_t fault_findNextSpot(uint8_t* seq)
{
	uint16_t i;
	uint8_t  maxseq = 0;
	uint16_t maxadr = FAULTS_START_ADDR;
	for (i = FAULTS_START_ADDR; i < FAULT_EEPROM_END; i += sizeof(fault_t))
	{
		fault_t tmp;
		eeprom_read_block((void*)&tmp, (const void *)i, sizeof(fault_t));
		if (tmp.seq == 0x00 || tmp.seq == 0xFF) // blank spot found
		{
			if (maxseq != 0) {
				*seq = maxseq + 1;
			}
			return i;
		}
		if (tmp.seq > maxseq) {
			maxseq = tmp.seq;
			maxadr = i;
		}
	}
	if (maxseq != 0) {
		*seq = maxseq + 1;
	}
	maxadr += sizeof(fault_t);
	if (maxadr < FAULT_EEPROM_END)
	{
		return maxadr;
	}
	return FAULTS_START_ADDR; // nothing is found, or overflow occured, write to start location as default
}

uint16_t fault_findEarliest()
{
	uint16_t i;
	uint8_t  minseq = 0xFF;
	uint16_t minadr = FAULTS_START_ADDR; // if nothing is found, return starting address
	for (i = FAULTS_START_ADDR; i < FAULT_EEPROM_END; i += sizeof(fault_t))
	{
		fault_t tmp;
		eeprom_read_block((void*)&tmp, (const void *)i, sizeof(fault_t));
		if (tmp.seq == 0x00 || tmp.seq == 0xFF) // blank spot found
		{
			break;
		}
		if (tmp.seq < minseq) {
			minseq = tmp.seq;
			minadr = i;
		}
	}
	return minadr;
}

void fault_printAll(void)
{
	uint16_t i, addr;
	fault_t tmp;
	bool looped = false;
	bool has = false;
	for (i = addr = fault_findEarliest(); ; ) // loop will exit when either a blank spot is found or when a loop is detected
	{
		fault_t tmp;
		eeprom_read_block((void*)&tmp, (const void *)i, sizeof(fault_t));
		if (tmp.seq == 0x00 || tmp.seq == 0xFF) // no more to print
		{
			break;
		}
		else if (i == addr)
		{
			if (looped != false)
			{
				break;
			}
			looped = true;
		}

		if (has == false)
		{
			Serial.println(F("faults: "));
		}
		has = true; // at this point, we definitely do have a fault registered in memory

		Serial.print(F("\t ")); Serial.print(tmp.seq, DEC); Serial.print(F(": "));
		Serial.print(tmp.odometer_hours, DEC); Serial.print(F(", "));
		Serial.print(tmp.millis, DEC); Serial.print(F(", 0x"));
		Serial.println(tmp.fault_code, HEX);

		i += sizeof(fault_t);
		if (i >= FAULT_EEPROM_END) { // overflow, roll-over the address
			i = 0;
		}
	}
	if (has == false)
	{
		Serial.println(F("no faults"));
	}
	else
	{
		Serial.println(F("end of faults"));
	}
}

void fault_clearAll()
{
	EEPROM.update(FAULTS_START_ADDR, 0); // clearing the very first sequence number will fool above functions
}

/*
fault codes are blinked out to the red LED in a pattern
the pattern will loop until there are no faults
when the pattern loop starts, there will be an extra long red blink
followed by a series of blinks that may be long or short
a long blink indicates the fault flag is 1
a short blink indicates the fault flag is 0

for example, if the fault is "FAULTCODE_COIL_2WAY_OPEN", which has the flag 0x01 << 6
the blinks sequence will look like

0             1     2     3     4     5     6        7     8     9     10    11    12
EXTRALOOOOONG SHORT SHORT SHORT SHORT SHORT LOOOOONG SHORT SHORT SHORT SHORT SHORT SHORT

multiple faults can be reported in a single loop, which will have two or more long blinks in the sequence
*/

uint8_t faultblink_mask = 0;
uint32_t faultblink_timestamp;
uint32_t faultblink_duration;

void faults_blink()
{
	if (faultblink_mask == FAULTCODE_NONE || faultblink_mask == FAULTCODE_END)
	{
		if (current_faults == FAULTCODE_NONE)
		{
			// nothing to report
			digitalWrite(PIN_LED_RED, LOW);
			faultblink_mask = FAULTCODE_NONE;
			return;
		}
		// there's a fault and the blink loop is just starting
		faultblink_mask = FAULTCODE_START_PLACEHOLDER;
		digitalWrite(PIN_LED_RED, HIGH);
		faultblink_timestamp = now;
		faultblink_duration = 1500; // extra long blink at the very start
		return;
	}

	if (TIME_HAS_ELAPSED(now, faultblink_timestamp, faultblink_duration))
	{
		faultblink_timestamp = now;
		if (digitalRead(PIN_LED_RED) != LOW)
		{
			// turn off the LED and have a short blank time before turning back on
			digitalWrite(PIN_LED_RED, LOW);
			faultblink_duration = 300;
		}
		else
		{
			faultblink_mask <<= 1; // check the next fault
			if (faultblink_mask == FAULTCODE_END) // end of fault loop
			{
				// handle it on the next loop with the logic in the code above
				return;
			}

			digitalWrite(PIN_LED_RED, HIGH);
			if ((current_faults & faultblink_mask) != 0)
			{
				// longer blink to represent a particular fault
				faultblink_duration = 1000;
			}
			else
			{
				faultblink_duration = 300;
			}
		}
	}
}

char btn_prev = HIGH;
uint32_t btn_timestamp;
bool buzzer_silent = false;
char silence_latch = false;

void faults_buzz()
{
	uint32_t care_about = 0
						#ifdef USE_VOLTAGE_MONITOR
						| FAULTCODE_UNDERVOLTAGE | FAULTCODE_OVERVOLTAGE
						#endif
						#ifdef USE_COIL_CHECK
						| FAULTCODE_COIL_5WAY_SHORT | FAULTCODE_COIL_5WAY_OPEN 
						| FAULTCODE_COIL_2WAY_SHORT | FAULTCODE_COIL_2WAY_OPEN
						#endif
						#ifdef USE_OXY_SENSOR
						| FAULTCODE_OXY_HW_FAIL
						#endif
						#ifdef USE_PRESSURE_SENSOR
						| FAULTCODE_PRESSURE_HW_FAIL
						#endif
						;

	char btn = digitalRead(PIN_BUTTON);
	bool need_buzz;

	if (need_buzz = ((current_faults & care_about) != 0))
	{
		if (buzzer_silent == false)
		{
			// buzz if not forced silence
			digitalWrite(PIN_BUZZER, HIGH);
		}
	}
	else
	{
		// only allowed to buzz again if faults have cleared
		buzzer_silent = false;
	}

	if (btn_prev != LOW && btn == LOW)
	{
		// down edge, remember when
		btn_timestamp = now;
	}
	else if (btn_prev == LOW && btn == LOW)
	{
		if (TIME_HAS_ELAPSED(now, btn_timestamp, BUTTON_HOLD_TIME))
		{
			// button held long enough
			if (need_buzz)
			{
				// force silence
				if (silence_latch == false)
				{
					buzzer_silent = true;
					silence_latch = true;
					#ifdef USE_PRESSURE_SENSOR
					if ((current_faults & FAULTCODE_PRESSURE_HW_FAIL) != 0) {
						pressure_init(); // this attempts to reinitialize the pressure sensor(s)
					}
					#endif
				}
				digitalWrite(PIN_BUZZER, LOW);
			}
			else
			{
				// use the button as a buzzer testing button
				digitalWrite(PIN_BUZZER, HIGH);
			}
		}
	}
	else if (btn != LOW)
	{
		silence_latch = false;
		if (need_buzz == false)
		{
			digitalWrite(PIN_BUZZER, LOW);
		}
	}

	prev_btn = btn; // remember for edge detect
}