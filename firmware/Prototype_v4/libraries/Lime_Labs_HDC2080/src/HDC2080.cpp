/*
	HDC2080.cpp
	HDC2010.cpp originally created by: Brandon Fisher, August 1st 2017
	
	This code is release AS-IS into the public domain, no guarantee or warranty is given.
	
	Description: This library facilitates communication with, and configuration of,
	the HDC2080 Temperature and Humidity Sensor. It makes extensive use of the 
	Wire.H library, and should be useable with both Arduino and Energia. 
*/


#include <HDC2080.h>
#include <Wire.h>

//Define Register Map
	#define TEMP_LOW 0x00
	#define TEMP_HIGH 0x01
	#define HUMID_LOW 0x02
	#define HUMID_HIGH 0x03
	#define INTERRUPT_DRDY 0x04
	#define TEMP_MAX 0x05
	#define HUMID_MAX 0x06
	#define INTERRUPT_CONFIG 0x07
	#define TEMP_OFFSET_ADJUST 0x08
	#define HUM_OFFSET_ADJUST 0x09
	#define TEMP_THR_L 0x0A
	#define TEMP_THR_H 0x0B
	#define HUMID_THR_L 0x0C
	#define HUMID_THR_H 0x0D
	#define CONFIG 0x0E
	#define MEASUREMENT_CONFIG 0x0F
	#define MID_L 0xFC
	#define MID_H 0xFD
	#define DEVICE_ID_L 0xFE
	#define DEVICE_ID_H 0xFF
	
void HDC2080::begin(uint8_t addr)
{
	_addr = addr;
	Wire.begin();
}

float HDC2080::readTemp(void)
{
	uint8_t byte[2];
	uint16_t temp;
	byte[0] = readReg(TEMP_LOW);
	byte[1] = readReg(TEMP_HIGH);
	temp = byte[1];
	temp = (temp << 8) | byte[0];
	float f = temp;
	f = ((f*165.0f)/65536.0f)-40.0f;
	return f;
	
}

float HDC2080::readHumidity(void)
{
	uint8_t byte[2];
	uint16_t humidity;
	byte[0] = readReg(HUMID_LOW);
	byte[1] = readReg(HUMID_HIGH);
	humidity = byte[1];
	humidity = (humidity << 8) | byte[0];
	float f = humidity;
	f = (f/65536.0f)*100.0f;
	
	return f;
	
}

uint16_t HDC2080::readManufacturerId(void)
{
	uint8_t byte[2];
	byte[0] = readReg(MID_L);
	byte[1] = readReg(MID_H);
	return (byte[1] << 8) | byte[0];
}

uint16_t HDC2080::readDeviceId(void)
{
	uint8_t byte[2];
	byte[0] = readReg(DEVICE_ID_L);
	byte[1] = readReg(DEVICE_ID_H);
	return (byte[1] << 8) | byte[0];
}

void HDC2080::enableHeater(void)
{
	uint8_t configContents;	//Stores current contents of config register
	
	configContents = readReg(CONFIG);
	
	//set bit 3 to 1 to enable heater
	configContents = (configContents | 0x08);
	
	writeReg(CONFIG, configContents);
	
}

void HDC2080::disableHeater(void)
{
	uint8_t configContents;	//Stores current contents of config register
	
	configContents = readReg(CONFIG);
	
	//set bit 3 to 0 to disable heater (all other bits 1)
	configContents = (configContents & 0xF7);
	writeReg(CONFIG, configContents);
	
}

void HDC2080::openReg(uint8_t reg)
{
	Wire.beginTransmission(_addr); 			// Connect to HDC2080
	Wire.write(reg); 						// point to specified register
	Wire.endTransmission(); 				// Relinquish bus control	
}

uint8_t HDC2080::readReg(uint8_t reg)
{
	openReg(reg);
	uint8_t reading; 					// holds byte of read data
	Wire.requestFrom(_addr, 1); 		// Request 1 byte from open register
	if (Wire.available() == 0){
		reading = 0;
	}
	else {
		reading = Wire.read();
	}
	
	return reading;
}

void HDC2080::writeReg(uint8_t reg, uint8_t data)
{
	
	Wire.beginTransmission(_addr);		// Open Device
	Wire.write(reg);						// Point to register
	Wire.write(data);						// Write data to register 
	Wire.endTransmission();				// Relinquish bus control	
		
}

void HDC2080::setLowTemp(float temp)
{
	uint8_t temp_thresh_low;
	
	// Verify user is not trying to set value outside bounds
	if (temp < -40.0f)
	{
		temp = -40.0f;
	}
	else if (temp > 125.0f)
	{
		temp = 125.0f;
	}
	
	// Calculate value to load into register
	temp_thresh_low = (uint8_t)(256.0f * (temp + 40.0f)/165.0f);
	
	writeReg(TEMP_THR_L, temp_thresh_low);
	
}

void HDC2080::setHighTemp(float temp)
{ 
	uint8_t temp_thresh_high;
	
	// Verify user is not trying to set value outside bounds
	if (temp < -40.0f)
	{
		temp = -40.0f;
	}
	else if (temp > 125.0f)
	{
		temp = 125.0f;
	}
	
	// Calculate value to load into register
	temp_thresh_high = (uint8_t)(256.0f * (temp + 40.0f)/165.0f);
	
	writeReg(TEMP_THR_H, temp_thresh_high);
	
}

void HDC2080::setHighHumidity(float humid)
{
	uint8_t humid_thresh;
	
	// Verify user is not trying to set value outside bounds
	if (humid < 0.0f)
	{
		humid = 0.0f;
	}
	else if (humid > 100.0f)
	{
		humid = 100.0f;
	}
	
	// Calculate value to load into register
	humid_thresh = (uint8_t)(256.0f * humid/100.0f);
	
	writeReg(HUMID_THR_H, humid_thresh);
	
}

void HDC2080::setLowHumidity(float humid)
{
	uint8_t humid_thresh;
	
	// Verify user is not trying to set value outside bounds
	if (humid < 0.0f)
	{
		humid = 0.0f;
	}
	else if (humid > 100.0f)
	{
		humid = 100.0f;
	}
	
	// Calculate value to load into register
	humid_thresh = (uint8_t)(256.0f * humid / 100.0f);
	
	writeReg(HUMID_THR_L, humid_thresh);
	
}

//  Return humidity from the low threshold register
float HDC2080::readLowHumidityThreshold(void)
{
	uint8_t regContents;
	
	regContents = readReg(HUMID_THR_L);
	float f = regContents;
	f = f * 100.0f / 256.0f;
	return f;
	
}

//  Return humidity from the high threshold register
float HDC2080::readHighHumidityThreshold(void)
{
	uint8_t regContents;
	
	regContents = readReg(HUMID_THR_H);
	float f = regContents;
	f = f * 100.0f / 256.0f;
	return f;
	
}

//  Return temperature from the low threshold register
float HDC2080::readLowTempThreshold(void)
{
	uint8_t regContents;
	
	regContents = readReg(TEMP_THR_L);
	float f = regContents;
	f = (f * 165.0f / 256.0f)-40.0f;
	return f;
	
}

//  Return temperature from the high threshold register
float HDC2080::readHighTempThreshold(void)
{
	uint8_t regContents;
	
	regContents = readReg(TEMP_THR_H);
	float f = regContents;
	f = (f * 165.0f / 256.0f)-40.0f;
	
	return f;
	
}


/* Upper two bits of the MEASUREMENT_CONFIG register controls
   the temperature resolution*/
void HDC2080::setTempRes(int resolution)
{ 
	uint8_t configContents;
	configContents = readReg(MEASUREMENT_CONFIG);
	
	switch(resolution)
	{
		case FOURTEEN_BIT:
			configContents = (configContents & 0x3F);
			break;
			
		case ELEVEN_BIT:
			configContents = (configContents & 0x7F);
			configContents = (configContents | 0x40);  
			break;
			
		case NINE_BIT:
			configContents = (configContents & 0xBF);
			configContents = (configContents | 0x80); 
			break;
			
		default:
			configContents = (configContents & 0x3F);
	}
	
	writeReg(MEASUREMENT_CONFIG, configContents);
	
}
/*  Bits 5 and 6 of the MEASUREMENT_CONFIG register controls
    the humidity resolution*/
void HDC2080::setHumidRes(int resolution)
{ 
	uint8_t configContents;
	configContents = readReg(MEASUREMENT_CONFIG);
	
	switch(resolution)
	{
		case FOURTEEN_BIT:
			configContents = (configContents & 0xCF);
			break;
			
		case ELEVEN_BIT:
			configContents = (configContents & 0xDF);
			configContents = (configContents | 0x10);  
			break;
			
		case NINE_BIT:
			configContents = (configContents & 0xEF);
			configContents = (configContents | 0x20); 
			break;
			
		default:
			configContents = (configContents & 0xCF);
	}
	
	writeReg(MEASUREMENT_CONFIG, configContents);	
}

/*  Bits 2 and 1 of the MEASUREMENT_CONFIG register controls
    the measurement mode  */
void HDC2080::setMeasurementMode(int mode)
{ 
	uint8_t configContents;
	configContents = readReg(MEASUREMENT_CONFIG);
	
	switch(mode)
	{
		case TEMP_AND_HUMID:
			configContents = (configContents & 0xF9);
			break;
			
		case TEMP_ONLY:
			configContents = (configContents & 0xFC);
			configContents = (configContents | 0x02);  
			break;
			
		case HUMID_ONLY:
			configContents = (configContents & 0xFD);
			configContents = (configContents | 0x04); 
			break;
			
		default:
			configContents = (configContents & 0xF9);
	}
	
	writeReg(MEASUREMENT_CONFIG, configContents);
}

/*  Bit 0 of the MEASUREMENT_CONFIG register can be used
    to trigger measurements  */
void HDC2080::triggerMeasurement(void)
{ 
	uint8_t configContents;
	configContents = readReg(MEASUREMENT_CONFIG);

	configContents = (configContents | 0x01);
	writeReg(MEASUREMENT_CONFIG, configContents);
}

/*  Bit 7 of the CONFIG register can be used to trigger a 
    soft reset  */
void HDC2080::reset(void)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);

	configContents = (configContents | 0x80);
	writeReg(CONFIG, configContents);
	delay(50);
}

/*  Bit 2 of the CONFIG register can be used to enable/disable 
    the interrupt pin  */
void HDC2080::enableInterrupt(void)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);

	configContents = (configContents | 0x04);
	writeReg(CONFIG, configContents);
}

/*  Bit 2 of the CONFIG register can be used to enable/disable 
    the interrupt pin  */
void HDC2080::disableInterrupt(void)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);

	configContents = (configContents & 0xFB);
	writeReg(CONFIG, configContents);
}


/*  Bits 6-4  of the CONFIG register controls the measurement 
    rate  */
void HDC2080::setRate(int rate)
{ 
	uint8_t configContents;
	configContents = readReg(CONFIG);
	
	switch(rate)
	{
		case MANUAL:
			configContents = (configContents & 0x8F);
			break;
			
		case TWO_MINS:
			configContents = (configContents & 0x9F);
			configContents = (configContents | 0x10);  
			break;
			
		case ONE_MINS:
			configContents = (configContents & 0xAF);
			configContents = (configContents | 0x20); 
			break;
		
		case TEN_SECONDS:
			configContents = (configContents & 0xBF);
			configContents = (configContents | 0x30); 
			break;
		
		case FIVE_SECONDS:
			configContents = (configContents & 0xCF);
			configContents = (configContents | 0x40); 
			break;
		
		case ONE_HZ:
			configContents = (configContents & 0xDF);
			configContents = (configContents | 0x50); 
			break;
		
		case TWO_HZ:
			configContents = (configContents & 0xEF);
			configContents = (configContents | 0x60); 
			break;
		
		case FIVE_HZ:
			configContents = (configContents | 0x70); 
			break;
			
		default:
			configContents = (configContents & 0x8F);
	}
	
	writeReg(CONFIG, configContents);
}

/*  Bit 1 of the CONFIG register can be used to control the  
    the interrupt pins polarity */
void HDC2080::setInterruptPolarity(int polarity)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);
	
	switch(polarity)
	{
		case ACTIVE_LOW:
			configContents = (configContents & 0xFD);
			break;
			
		case ACTIVE_HIGH:
			configContents = (configContents | 0x02);  
			break;
			
		default:
			configContents = (configContents & 0xFD);
	}
	
	writeReg(CONFIG, configContents);	
}

/*  Bit 0 of the CONFIG register can be used to control the  
    the interrupt pin's mode */
void HDC2080::setInterruptMode(int mode)
{
	uint8_t configContents;
	configContents = readReg(CONFIG);
	
	switch(mode)
	{
		case LEVEL_MODE:
			configContents = (configContents & 0xFE);
			break;
			
		case COMPARATOR_MODE:
			configContents = (configContents | 0x01);  
			break;
			
		default:
			configContents = (configContents & 0xFE);
	}
	
	writeReg(CONFIG, configContents);	
}


uint8_t HDC2080::readInterruptStatus(void)
{
	uint8_t regContents;
	regContents = readReg(INTERRUPT_DRDY);
	return regContents;
	
}

//  Clears the maximum temperature register
void HDC2080::clearMaxTemp(void)
{ 
	writeReg(TEMP_MAX, 0x00);
}

//  Clears the maximum humidity register
void HDC2080::clearMaxHumidity(void)
{ 
	writeReg(HUMID_MAX, 0x00);
}

//  Reads the maximum temperature register
float HDC2080::readMaxTemp(void)
{
	uint8_t regContents;
	
	regContents = readReg(TEMP_MAX);
	float f = regContents;
	f = (f * 165.0f / 256.0f)-40.0f;

	return f;
	
}

//  Reads the maximum humidity register
float HDC2080::readMaxHumidity(void)
{
	uint8_t regContents;
	
	regContents = readReg(HUMID_MAX);
	float f = regContents;
	f = (f / 256.0f)*100.0f;

	return f;
	
}


// Enables the interrupt pin for comfort zone operation
void HDC2080::enableThresholdInterrupt(void)
{
	
	uint8_t regContents;
	regContents = readReg(INTERRUPT_CONFIG);

	regContents = (regContents | 0x78);

	writeReg(INTERRUPT_CONFIG, regContents);	
}

// Disables the interrupt pin for comfort zone operation
void HDC2080::disableThresholdInterrupt(void)
{
	uint8_t regContents;
	regContents = readReg(INTERRUPT_CONFIG);

	regContents = (regContents & 0x87);

	writeReg(INTERRUPT_CONFIG, regContents);	
}

// enables the interrupt pin for DRDY operation
void HDC2080::enableDRDYInterrupt(void)
{
	uint8_t regContents;
	regContents = readReg(INTERRUPT_CONFIG);

	regContents = (regContents | 0x80);

	writeReg(INTERRUPT_CONFIG, regContents);	
}

// disables the interrupt pin for DRDY operation
void HDC2080::disableDRDYInterrupt(void)
{
	uint8_t regContents;
	regContents = readReg(INTERRUPT_CONFIG);

	regContents = (regContents & 0x7F);

	writeReg(INTERRUPT_CONFIG, regContents);	
}
