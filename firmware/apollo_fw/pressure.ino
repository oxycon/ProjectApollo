#define PRESSURE_SENSOR_MAX_CNT 8

uint32_t mpr_readings[PRESSURE_SENSOR_MAX_CNT];
uint8_t  mpr_status[PRESSURE_SENSOR_MAX_CNT];
uint8_t  mpr_mode[PRESSURE_SENSOR_MAX_CNT]

uint8_t  mpr_sensor_cnt = 0;
bool has_i2cmux, has_gpioexp, has_mpr;

#define I2CADDR_TCA9548A (0x70 << 1)
#define I2CADDR_MCP23017 (0x20 << 1)
#define I2CADDR_MPR      (0x18 << 1)

void pressure_init()
{
	// check the existance of the I2C mux and GPIO expander
	i2c_init();
	i2c_start(I2CADDR_TCA9548A); i2c_stop();
	has_i2cmux = i2c_errorFlag == 0;
	i2c_init();
	i2c_start(I2CADDR_MCP23017); i2c_stop();
	has_gpioexp = i2c_errorFlag == 0;
	i2c_init();
	i2c_start(I2CADDR_MPR); i2c_stop();
	has_mpr = i2c_errorFlag == 0;

	if (has_i2cmux != false && has_gpioexp != false)
	{
		// both I2C mux and GPIO expander exist
		uint8_t c;
		mpr_sensor_cnt = PRESSURE_SENSOR_MAX_CNT;
		for (c = 0; c < mpr_sensor_cnt; c++)
		{
			// scan for all 8 sensors
			// if a sensor replies over I2C then we know it's a I2C sensor
			// otherwise it is SPI
			i2cmux_switch(c);
			i2c_init();
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
	else if (has_i2cmux == false && has_gpioexp == false && has_mpr != false)
	{
		// I2C mux and GPIO expander are both missing
		// meaning we have a singular I2C sensor

		mpr_mode[0] = MPR_MODE_I2C;
		mpr_sensor_cnt = 1;

		// TODO:
	}
	else if (has_i2cmux != has_gpioexp)
	{
		// reaching here means we've attached a malfunctioning daughterboard!
		// TODO:
	}
}

void i2c_setpins()
{
	// we are about to start using I2C
	// meaning the pins must be readied to be open-drain without internal pull-up resistors
	pinMode(PIN_I2C_SCL, INPUT);
	pinMode(PIN_I2C_SDA, INPUT);
	digitalWrite(PIN_I2C_SCL, LOW);
	digitalWrite(PIN_I2C_SDA, LOW);
}

void spi_setpins()
{
	// we are about to start using SPI
	// meaning the TWI module must be off and pins readied to be bit-banged
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
	// bit-bang SPI according to the correct SPI mode and bit ordering of the MPR sensor
	for (i = 0, m = 0x80; i < 8; i++, m >> 1)
	{
		digitalWrite(PIN_I2C_SCL, LOW);
		digitalWrite(PIN_I2C_SDA, (x & m) != 0 ? HIGH : LOW);
		_delay_us(SPI_BITBANG_DLY);
		digitalWrite(PIN_I2C_SCL, HIGH);
		if (digitalRead(PIN_MISO) != LOW) {
			y |= m;
		}
		_delay_us(SPI_BITBANG_DLY);
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
		mpr_spiSelect(chan);
		spi_shift(0xAA);
		spi_shift(0x00);
		spi_shift(0x00);
		mpr_spiDeselect();
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
		
		mpr_spiSelect(chan);
		mpr_status[chan] = spi_shift(0xF0);
		x  = spi_shift(0x00);
		x <<= 8;
		x |= spi_shift(0x00);
		x <<= 8;
		x |= spi_shift(0x00);
		mpr_spiDeselect();
		mpr_readings[chan] = x;
		return true;
	}
	return false;
}

bool mpr_spiSelect(uint8_t chan)
{
	digitalWrite(PIN_DAUGHTER_CS, LOW); // LOW to the MOSFET gates means CS is actually HIGH

	if (has_i2cmux != false) {
		i2cmux_switch(chan);
		if (i2c_errorFlag != 0) {
			return false;
		}
	}

	i2c_writeReg(I2CADDR_MCP23017, 0x01, 0x01 << chan); // IODIRB
	if (i2c_errorFlag != 0) {
		return false;
	}
	i2c_writeReg(I2CADDR_MCP23017, 0x13, ~(0x01 << chan)); // GPIOB
	if (i2c_errorFlag != 0) {
		return false;
	}

	spi_setpins();
	digitalWrite(PIN_DAUGHTER_CS, HIGH); // HIGH to the MOSFET gates means CS is actually LOW
	// we need to do this after all I2C traffic or else the sensor will be confused

	return true;
}

bool mpr_spiDeselect()
{
	digitalWrite(PIN_DAUGHTER_CS, LOW); // LOW to the MOSFET gates means CS is actually HIGH
	// we need to do this before any more I2C traffic or else the sensor will be confused

	i2c_setpins();
	i2c_init();

	#if 0 // we don't really need to touch the GPIO expander if the MOSFET is off
	i2c_writeReg(I2CADDR_MCP23017, 0x01, 0); // IODIRB
	if (i2c_errorFlag != 0) {
		return false;
	}
	i2c_writeReg(I2CADDR_MCP23017, 0x13, 0xFF)); // GPIOB
	if (i2c_errorFlag != 0) {
		return false;
	}
	#endif

	return true;
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