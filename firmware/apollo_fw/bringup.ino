#include <SoftwareSerial.h>
#include <util/delay.h>

void bringup_valve(uint8_t p)
{
	uint32_t ts;
	char state = LOW;
	pinMode(p, OUTPUT);
	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);
	while (true)
	{
		Serial.print(F("bringup_valve "));
		Serial.println(millis(), DEC);
		digitalWrite(p, state);
		digitalWrite(PIN_LED_GREEN, state);
		ts = millis();
		while (TIME_HAS_ELAPSED(millis(), ts, 2000) == false)
		{
			digitalWrite(PIN_LED_RED, digitalRead(p == PIN_COIL_1 ? PIN_COILCHECK_1 : PIN_COILCHECK_2));
		}
		state = (state == LOW) ? HIGH : LOW;
	}
}

void bringup_button()
{
	pinMode(PIN_BUTTON, INPUT);
	digitalWrite(PIN_BUTTON, HIGH);
	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);
	pinMode(PIN_BUZZER, OUTPUT);
	while (true)
	{
		Serial.print(F("bringup_button "));
		Serial.print(millis(), DEC);
		if (digitalRead(PIN_BUTTON) == LOW)
		{
			Serial.println(F(" down"));
			digitalWrite(PIN_LED_GREEN, LOW);
			digitalWrite(PIN_LED_RED, HIGH);
			digitalWrite(PIN_BUZZER, HIGH);
		}
		else
		{
			Serial.println(F(" up"));
			digitalWrite(PIN_LED_GREEN, HIGH);
			digitalWrite(PIN_LED_RED, LOW);
			digitalWrite(PIN_BUZZER, LOW);
		}
		_delay_ms(250);
	}
}

void bringup_vmon()
{
	while (true)
	{
		Serial.print(F("v: "));
		Serial.println(voltage_read(), DEC);
		_delay_ms(500);
	}
}

extern SoftwareSerial* softSer;

void bringup_oxy()
{
	#define OUTPUT_BUFFER_SIZE 64
	char output_buffer[OUTPUT_BUFFER_SIZE];
	oxy_init();
	while (true)
	{
		oxy_task();
		if (softSer->available() > 0) // has at least one byte from UART
		{
			o2sens_feedUartByte(softSer->read()); // give byte to parser
			if (o2sens_hasNewData()) // has complete packet
			{
				o2sens_clearNewData();
				// display time in milliseconds for logging
				Serial.print(millis(), DEC); Serial.print(": ");

				// print out the whole buffer for debugging
				uint8_t* all_data = o2sens_getRawBuffer();
				for (int i = 0; i < all_data[1] + 3; i++)
				{
					sprintf(output_buffer, "%02X ", all_data[i]);
					Serial.print(output_buffer);
				}

				// print out the human readable data
				sprintf(output_buffer, ", %u , %u , %u"
									, o2sens_getConcentration16()
									, o2sens_getFlowRate16()
									, o2sens_getTemperature16()
				);
				Serial.println(output_buffer);
			}
		}
	}
}

void bringup_pressure()
{
	uint32_t ts = millis();
	nvm.debug_mode = true;
	pressure_init();
	Serial.println(F("pressure_init done"));
	while (true)
	{
		pressure_task();
		if (TIME_HAS_ELAPSED(millis(), ts, 500))
		{
			ts = millis();
			pressure_printAll(false, true, false);
			Serial.println();
		}
	}
}

void bringup_cmd()
{
	while (true)
	{
		cmd_task();
	}
}

void bringup()
{
	// bringup_cmd();
	// bringup_valve();
	// bringup_button();
	// bringup_vmon();
	// bringup_oxy();
	// bringup_pressure();
}