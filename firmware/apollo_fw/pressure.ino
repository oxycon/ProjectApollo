#include "defs.h"
#include "config.h"
#include <i2clite.h>
#include <util/delay.h>

int32_t  mpr_readings[PRESSURE_SENSOR_MAX_CNT];
uint8_t  mpr_status[PRESSURE_SENSOR_MAX_CNT];
uint8_t  mpr_mode[PRESSURE_SENSOR_MAX_CNT];

uint8_t  mpr_sensor_cnt = 0;
bool has_i2cmux, has_gpioexp, has_mpr;

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
		if (nvm.debug_mode) { Serial.println(F("muxing board found")); }
		mpr_sensor_cnt = PRESSURE_SENSOR_MAX_CNT;
		for (c = 0; c < mpr_sensor_cnt; c++)
		{
			// scan for all 8 sensors
			// if a sensor replies over I2C then we know it's a I2C sensor
			// otherwise it is SPI
			i2cmux_switch(c);
			i2c_init();
			i2c_start(I2CADDR_TCA9548A); i2c_stop();
			if (nvm.debug_mode) { Serial.print(F("mux sensor ")); Serial.print(c); }
			if (i2c_errorFlag == 0)
			{
				mpr_mode[c] = MPR_MODE_I2C;
				if (nvm.debug_mode) { Serial.print(F(" I2C")); }
			}
			else
			{
				mpr_mode[c] = MPR_MODE_SPI;
				if (nvm.debug_mode) { Serial.print(F(" SPI")); }
			}
			mpr_readings[c] = -1;
			mpr_status[c]   = 0;
		}
		current_faults &= ~FAULTCODE_PRESSURE_HW_FAIL;
	}
	else if (has_i2cmux == false && has_gpioexp == false)
	{
		// I2C mux and GPIO expander are both missing
		if (has_mpr != false)
		{
			// meaning we have a singular I2C sensor

			mpr_mode[0] = MPR_MODE_I2C;
			mpr_readings[0] = -1;
			mpr_sensor_cnt = 1;
			current_faults &= ~FAULTCODE_PRESSURE_HW_FAIL;
			if (nvm.debug_mode) { Serial.println(F("single pressure sensor found")); }
		}
		else
		{
			mpr_sensor_cnt = 0;
			current_faults |= FAULTCODE_PRESSURE_HW_FAIL;
			fault_save(current_faults);
		}
	}
	else if (has_i2cmux != has_gpioexp)
	{
		// reaching here means we've attached a malfunctioning daughterboard!
		mpr_sensor_cnt = 0;
		current_faults |= FAULTCODE_PRESSURE_HW_FAIL;
		fault_save(current_faults);
	}
	if ((current_faults & FAULTCODE_PRESSURE_HW_FAIL) != 0 && nvm.debug_mode)
	{
		Serial.println(F("pressure sensor(s) init failed"));
	}
}

uint8_t mpr_phase = MPRPHASE_WAIT;
uint32_t mpr_timestamp = 0;
uint8_t mpr_service_idx = 0;

void pressure_task()
{
	if (mpr_sensor_cnt <= 0) {
		return;
	}

	if (mpr_phase == MPRPHASE_START)
	{
		if (mpr_service_idx >= mpr_sensor_cnt)
		{
			// has sent start command to all sensors
			// transition to next phase
			mpr_service_idx = 0;
			mpr_timestamp = now;
			mpr_phase = MPRPHASE_WAIT;
			return;
		}
		else
		{
			if (mpr_startRead(mpr_service_idx) == false)
			{
				if ((current_faults & FAULTCODE_PRESSURE_HW_FAIL) == 0) {
					current_faults |= FAULTCODE_PRESSURE_HW_FAIL;
					fault_save(current_faults);
				}
			}
			mpr_service_idx++; // next sensor
		}
	}
	else if (mpr_phase == MPRPHASE_WAIT)
	{
		if (TIME_HAS_ELAPSED(now, mpr_timestamp, MPR_SAMPLE_TIME))
		{
			mpr_timestamp = now;
			mpr_phase = MPRPHASE_READ;
		}
	}
	else if (mpr_phase == MPRPHASE_READ)
	{
		if (mpr_service_idx >= mpr_sensor_cnt)
		{
			// has read from all sensors
			// transition to first phase
			mpr_service_idx = 0;
			mpr_timestamp = now;
			mpr_phase = MPRPHASE_START;
			return;
		}
		else
		{
			if (mpr_endRead(mpr_service_idx) == false)
			{
				if ((current_faults & FAULTCODE_PRESSURE_HW_FAIL) == 0) {
					current_faults |= FAULTCODE_PRESSURE_HW_FAIL;
					fault_save(current_faults);
				}
			}
			mpr_service_idx++; // next sensor
		}
	}
}

int32_t pressure_readLast(uint8_t c)
{
	if (c >= mpr_sensor_cnt) {
		c = 0;
	}
	return mpr_readings[c];
}

void pressure_printOne(uint8_t c)
{
	uint8_t sts = mpr_status[c];
	Serial.print(F("{"));
	if (mpr_sensor_cnt > 1)
	{
		Serial.print(c, DEC);
		Serial.print(F(", "));
	}
	Serial.print(F("0x"));
	if (sts <= 0x0F) 
	{
		Serial.print(F("0"));
	}
	Serial.print(sts, HEX);
	Serial.print(F(", "));
	Serial.print(mpr_readings[c], DEC);
	Serial.print(F("}"));
}

void pressure_printAll(bool tab, bool comma, bool newline)
{
	uint8_t i;
	for (i = 0; i < mpr_sensor_cnt || i == 0; i++)
	{
		if (tab) {
			Serial.print(F("\t"));
		}
		pressure_printOne(i);
		if (comma && (i + 1) < mpr_sensor_cnt) {
			Serial.print(F(", "));
		}
		if (newline) {
			Serial.println();
		}
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
	pinMode(PIN_DAUGHTER_MOSI, OUTPUT);
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
		_delay_us(SPI_BITBANG_DLY); // use _delay_us instead of delayMicroseconds, we need interrupts to be enabled during this
		digitalWrite(PIN_I2C_SCL, HIGH);
		if (digitalRead(PIN_DAUGHTER_MOSI) != LOW) {
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
		if (mpr_spiSelect(chan) == false) {
			mpr_spiDeselect();
			return false;
		}
		spi_shift(0xAA);
		spi_shift(0x00);
		spi_shift(0x00);
		if (mpr_spiDeselect() == false) {
			return false;
		}
	}
	return false;
}

bool mpr_endRead(uint8_t chan)
{
	uint8_t mode = mpr_mode[chan];
	uint8_t sts;
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
		mpr_status[chan] = sts = i2c_readAck();
		x  = i2c_readAck();
		x <<= 8;
		x |= i2c_readAck();
		x <<= 8;
		x |= i2c_readNak();
		if (i2c_errorFlag != 0) {
			return false;
		}
	}
	else if (mode == MPR_MODE_SPI)
	{
		if (mpr_spiSelect(chan) == false) {
			mpr_spiDeselect();
			return false;
		}
		mpr_status[chan] = sts = spi_shift(0xF0);
		x  = spi_shift(0x00);
		x <<= 8;
		x |= spi_shift(0x00);
		x <<= 8;
		x |= spi_shift(0x00);
		if (mpr_spiDeselect() == false) {
			return false;
		}
	}
	if ((sts & MPR_STATUS_MASK) == MPR_STATUS_POWERED) {
		mpr_readings[chan] = x;
	}
	if ((sts & MPR_STATUS_ERROR) != 0) {
		return false;
	}
	return true;
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

	i2c_writeReg(I2CADDR_MCP23017, 0x01, 0x01 << chan); // IODIRB, direction to output
	if (i2c_errorFlag != 0) {
		return false;
	}
	i2c_writeReg(I2CADDR_MCP23017, 0x13, ~(0x01 << chan)); // GPIOB, state to LOW
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
	i2c_write(1 << chan);
	i2c_stop();
}
