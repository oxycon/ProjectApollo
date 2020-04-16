
#define PRESSURE_SENSOR_MAX_CNT 8

uint32_t mpr_readings[PRESSURE_SENSOR_MAX_CNT];
uint8_t  mpr_status[PRESSURE_SENSOR_MAX_CNT];
uint8_t  mpr_mode[PRESSURE_SENSOR_MAX_CNT]

uint8_t  mpr_sensor_cnt = 0;

#define I2CADDR_TCA9548A (0x70 << 1)
#define I2CADDR_MCP23017 (0x20 << 1)
#define I2CADDR_MPR      (0x18 << 1)

void pressure_init()
{
	i2c_errorFlag = 0;
	i2c_start(I2CADDR_TCA9548A); i2c_stop();
	has_i2cmux = i2c_errorFlag == 0;
	i2c_errorFlag = 0;
	i2c_start(I2CADDR_MCP23017); i2c_stop();
	has_gpioexp = i2c_errorFlag == 0;
	if (has_i2cmux != false && has_gpioexp != false)
	{
		uint8_t c;
		for (c = 0; c < PRESSURE_SENSOR_MAX_CNT; c++)
		{
			i2cmux_switch(c);
			i2c_errorFlag = 0;
			i2c_start(I2CADDR_TCA9548A); i2c_stop();
			if (i2c_errorFlag == 0)
			{
				mpr_mode[c] = MPR_MODE_I2C;
			}
			else
			{
				mpr_mode[c] = MPR_MODE_SPI;
			}
		}
	}
	else
	{
		
	}
}

void i2c_setpins()
{
	pinMode(PIN_I2C_SCL, INPUT);
	pinMode(PIN_I2C_SDA, INPUT);
	digitalWrite(PIN_I2C_SCL, LOW);
	digitalWrite(PIN_I2C_SDA, LOW);
}

void spi_setpins()
{
	i2c_deinit();
	pinMode(PIN_I2C_SCL, OUTPUT);
	pinMode(PIN_I2C_SDA, OUTPUT);
	pinMode(PIN_MISO, INPUT);
}

uint8_t spi_shift(uint8_t x)
{
	uint8_t i;
	uint8_t m;
	uint8_t y = 0x00;
	for (i = 0, m = 0x80; i < 8; i++, m >> 1)
	{
		digitalWrite(PIN_I2C_SCL, LOW);
		digitalWrite(PIN_I2C_SDA, (x & m) == 0 ? HIGH : LOW);
		_delay_us(1);
		digitalWrite(PIN_I2C_SCL, HIGH);
		if (digitalRead(PIN_MISO) != LOW) {
			y |= m;
		}
		_delay_us(1);
	}
	return y;
}

bool mpr_startRead(uint8_t chan)
{
	uint8_t mode = mpr_mode[chan];
	if (mode == MPR_MODE_I2C)
	{
		if (has_i2cmux != false) {
			i2cmux_switch(chan);
			if (i2c_errorFlag != 0) {
				return false;
			}
		}
		i2c_init();
		i2c_start(I2CADDR_MPR);
		i2c_write(0xAA);
		i2c_write(0x00);
		i2c_write(0x00);
		i2c_stop();
		if (i2c_errorFlag != 0) {
			return false;
		}
		return true;
	}
	else if (mode == MPR_MODE_SPI)
	{
	}
	return false;
}

bool mpr_endRead(uint8_t chan)
{
	uint8_t mode = mpr_mode[chan];
	uint32_t x;
	if (mode == MPR_MODE_I2C)
	{
		if (has_i2cmux != false) {
			i2cmux_switch(chan);
			if (i2c_errorFlag != 0) {
				return false;
			}
		}
		i2c_init();
		i2c_start(I2CADDR_MPR | TW_READ);
		mpr_status[chan] = i2c_readAck();
		x  = i2c_readAck();
		x <<= 8;
		x |= i2c_readAck();
		x <<= 8;
		x |= i2c_readNak();
		mpr_readings[chan] = x;
		if (i2c_errorFlag != 0) {
			return false;
		}
		return true;
	}
	else if (mode == MPR_MODE_SPI)
	{
		if (has_i2cmux != false) {
			i2cmux_switch(chan);
			if (i2c_errorFlag != 0) {
				return false;
			}
		}
	}
	return false;
}

void i2cmux_switch(uint8_t chan)
{
	i2c_setpins();
	i2c_init();
	i2c_start(I2CADDR_TCA9548A);
	i2c_write(1 << i);
	i2c_stop();
}

bool gpioexp_readEoc(uint8_t chan)
{
	
}