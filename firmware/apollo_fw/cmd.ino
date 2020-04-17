#include <avr/pgmspace.h>
#include <string.h>

char cmd_buffer[CMD_BUFFER_SIZE];
uint8_t cmd_buffer_idx = 0;
char cmd_buffer_last = 0;

void cmd_task()
{
	while (Serial.available() > 0)
	{
		cmd_feed(Serial.read());
	}
}

void cmd_feed(uint8_t x)
{
	if (x == '\r' || x == '\n' || x == '\0')
	{
		if (x == '\n' && cmd_buffer_last == '\r')
		{
			// this prevents "\r\n" from triggering an error message
			cmd_buffer_last = x;
			return;
		}
		cmd_buffer_last = x;
		if (cmd_buffer_idx <= 0)
		{
			Serial.println(F("ERR no command"));
			return;
		}
		cmd_buffer[cmd_buffer_idx] = 0; // null terminate
		cmd_execute(cmd_buffer);
		cmd_buffer_idx = 0;
		return;
	}

	if (x == 0x08 || x == 0x18 || x == 0x1B || x == 0x7F)
	{
		cmd_buffer_idx = 0; // reset
		cmd_buffer[cmd_buffer_idx] = 0; // null terminate
		Serial.println(F("ERR command cancelled"));
		return;
	}

	if (cmd_buffer_idx >= (CMD_BUFFER_SIZE - 3))
	{
		cmd_buffer_idx = 0; // reset
		cmd_buffer[cmd_buffer_idx] = 0; // null terminate
		Serial.println(F("ERR command cancelled (too long)"));
		return;
	}

	if (cmd_buffer_idx == 0 && x == ' ')
	{
		// ignore leading spaces
		return;
	}

	cmd_buffer[cmd_buffer_idx] = x; // store
	cmd_buffer_idx += 1             // next
	cmd_buffer[cmd_buffer_idx] = 0; // null terminate
}

void cmd_execute(char* s)
{
	uint8_t pstart;
	if      (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("echo"), pstart = 5) == 0) {
		Serial.println(F("echo"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("version"), pstart = 8) == 0) {
		Serial.print(F("FW compiled on ")); Serial.print(__DATE__); Serial.print(F("    ")); Serial.print(__TIME__);
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("millis"), pstart = 7) == 0) {
		Serial.println(millis(), DEC);
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("debug_on"), pstart = 9) == 0) {
		Serial.println(F("debug ON"));
		nvm.debug_mode = true;
		nvm_write(&nvm);
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("debug_off"), pstart = 10) == 0) {
		Serial.println(F("debug OFF"));
		nvm.debug_mode = false;
		nvm_write(&nvm);
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("nvmreset"), pstart = 9) == 0) {
		nvm_reset(&nvm);
		Serial.println(F("NVM reset"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("nvmdump"), pstart = 8) == 0) {
		Serial.println(F("NVM dump:"));
		nvm_dump();
		Serial.println();
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("nvmsave"), pstart = 8) == 0) {
		nvm_write(&nvm);
		Serial.println(F("NVM saved"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("odo"), pstart = 4) == 0) {
		Serial.print(F("odo = "));
		Serial.println(nvm.odometer_hours, DEC);
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("run"), pstart = 4) == 0) {
		run = true;
		Serial.println(F("running"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("stop"), pstart = 5) == 0) {
		run = false;
		Serial.println(F("stopping"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("coil_5way_on"), pstart = 13) == 0) {
		run = false;
		digitalWrite(PIN_COIL_5WAY, HIGH);
		Serial.println(F("coil 5way on"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("coil_5way_off"), pstart = 14) == 0) {
		run = false;
		digitalWrite(PIN_COIL_5WAY, LOW);
		Serial.println(F("coil 5way off"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("coil_2way_on"), pstart = 13) == 0) {
		run = false;
		digitalWrite(PIN_COIL_2WAY, HIGH);
		Serial.println(F("coil 2way on"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("coil_2way_off"), pstart = 14) == 0) {
		run = false;
		digitalWrite(PIN_COIL_2WAY, LOW);
		Serial.println(F("coil 2way off"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("led_green_on"), pstart = 13) == 0) {
		run = false;
		digitalWrite(PIN_LED_GREEN, HIGH);
		Serial.println(F("LED green on"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("led_green_off"), pstart = 14) == 0) {
		run = false;
		digitalWrite(PIN_LED_GREEN, LOW);
		Serial.println(F("LED green off"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("led_red_on"), pstart = 11) == 0) {
		run = false;
		digitalWrite(PIN_LED_RED, HIGH);
		Serial.println(F("LED red on"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("led_red_off"), pstart = 12) == 0) {
		run = false;
		digitalWrite(PIN_LED_RED, LOW);
		Serial.println(F("LED red off"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("buzz_on"), pstart = 8) == 0) {
		run = false;
		digitalWrite(PIN_BUZZER, HIGH);
		Serial.println(F("buzzer on"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("buzz_off"), pstart = 9) == 0) {
		run = false;
		digitalWrite(PIN_BUZZER, LOW);
		Serial.println(F("buzzer off"));
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("readpins"), pstart = 9) == 0) {
		Serial.println(F("pin states:"));
		Serial.print(F("\t PIN_LED_RED    : ")); Serial.println(digitalRead(PIN_LED_RED    ));
		Serial.print(F("\t PIN_LED_GREEN  : ")); Serial.println(digitalRead(PIN_LED_GREEN  ));
		Serial.print(F("\t PIN_BUTTON     : ")); Serial.println(digitalRead(PIN_BUTTON     ));
		Serial.print(F("\t PIN_BUZZER     : ")); Serial.println(digitalRead(PIN_BUZZER     ));
		Serial.print(F("\t PIN_COIL_1     : ")); Serial.println(digitalRead(PIN_COIL_1     ));
		Serial.print(F("\t PIN_COIL_2     : ")); Serial.println(digitalRead(PIN_COIL_2     ));
		Serial.print(F("\t PIN_COILCHECK_1: ")); Serial.println(digitalRead(PIN_COILCHECK_1));
		Serial.print(F("\t PIN_COILCHECK_2: ")); Serial.println(digitalRead(PIN_COILCHECK_2));
		Serial.print(F("\t PIN_12V_MONITOR: ")); Serial.println(analogRead(PIN_12V_MONITOR));
		#ifdef PIN_MPR_RST
		Serial.print(F("\t PIN_MPR_RST: ")); Serial.println(digitalRead(PIN_MPR_RST));
		#endif
		#ifdef PIN_MPR_EOC
		Serial.print(F("\t PIN_MPR_EOC: ")); Serial.println(digitalRead(PIN_MPR_EOC));
		#endif
		#ifdef PIN_AUX2
		Serial.print(F("\t PIN_AUX2: ")); Serial.println(digitalRead(PIN_AUX2));
		#endif
		#ifdef PIN_AUX3
		Serial.print(F("\t PIN_AUX3: ")); Serial.println(digitalRead(PIN_AUX3));
		#endif
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("gas"), pstart = 4) == 0) {
		Serial.println(F("gas data:"));
		Serial.print(F("\t gas pressure      : ")); Serial.println(pressure);
		Serial.print(F("\t oxy concentration : ")); Serial.println(o2sens_getConcentration16());
		Serial.print(F("\t oxy flow rate     : ")); Serial.println(o2sens_getFlowRate16());
		Serial.print(F("\t oxy temperature   : ")); Serial.println(o2sens_getTemperature16());
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("volt"), pstart = 5) == 0) {
		Serial.print(F("Voltage (mv): "));
		Serial.println(voltage_read(), DEC);
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("testint"), pstart = 7) == 0) {
		int16_t x;
		pstart++; // skip space
		if (cmd_readInt16(&x, s, pstart) != false)
		{
			Serial.print(F("test int "));
			Serial.println(x, DEC);
		}
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("time_5way"), pstart = 9) == 0) {
		int16_t x;
		if (s[pstart] == 0)
		{
			Serial.print(F("time for 5way valve = "));
			Serial.println(nvm.time_stage_5way, DEC);
		}
		else
		{
			pstart++; // skip space
			if (cmd_readInt16(&x, s, pstart) != false)
			{
				nvm.time_stage_5way = x;
				//nvm_write(&nvm);
				Serial.print(F("time for 5way valve set "));
				Serial.println(x, DEC);
			}
		}
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("time_2way"), pstart = 9) == 0) {
		int16_t x;
		if (s[pstart] == 0)
		{
			Serial.print(F("time for 2way valve = "));
			Serial.println(nvm.time_stage_2way, DEC);
		}
		else
		{
			pstart++; // skip space
			if (cmd_readInt16(&x, s, pstart) != false)
			{
				nvm.time_stage_2way = x;
				//nvm_write(&nvm);
				Serial.print(F("time for 2way valve set "));
				Serial.println(x, DEC);
			}
		}
	}
	else if (memcmp_PF((const void*)s, (uint_farptr_t)PSTR("time_pause"), pstart = 10) == 0) {
		int16_t x;
		if (s[pstart] == 0)
		{
			Serial.print(F("time for pause = "));
			Serial.println(nvm.time_stage_pause, DEC);
		}
		else
		{
			pstart++; // skip space
			if (cmd_readInt16(&x, s, pstart) != false)
			{
				nvm.time_stage_pause = x;
				//nvm_write(&nvm);
				Serial.print(F("time for pause set "));
				Serial.println(x, DEC);
			}
		}
	}
	else
	{
		Serial.print(F("ERR command unknown: "));
		Seria.println(s);
	}
}

bool cmd_readInt16(int16_t* x, char* s, uint8_t idx, int base)
{
	char* ep;
	*x = strtol(&s[idx], &ep, base);
	if (ep == &s[idx])
	{
		Serial.println(F("ERR invalid number"));
		return false;
	}
}
