#include "defs.h"
#include "config.h"

void setup()
{
	pinMode(PIN_LED_RED     , OUTPUT);
	pinMode(PIN_LED_GREEN   , OUTPUT);
	pinMode(PIN_BUTTON      , INPUT);
	pinMode(PIN_BUZZER      , OUTPUT);
	pinMode(PIN_COIL_1      , OUTPUT);
	pinMode(PIN_COIL_2      , OUTPUT);
	pinMode(PIN_COILCHECK_1 , INPUT);
	pinMode(PIN_COILCHECK_2 , INPUT);
	pinMode(PIN_O2SENS_RX   , INPUT);
	pinMode(PIN_O2SENS_TX   , OUTPUT);
	pinMode(PIN_12V_MONITOR , INPUT);
	pinMode(PIN_MPR_RST     , OUTPUT);
	pinMode(PIN_MPR_EOC     , INPUT);
	pinMode(PIN_AUX2        , INPUT);
	pinMode(PIN_AUX3        , INPUT);

	pinMode(PIN_I2C_SCL     , INPUT);
	pinMode(PIN_I2C_SDA     , INPUT);
}

void loop()
{
}
