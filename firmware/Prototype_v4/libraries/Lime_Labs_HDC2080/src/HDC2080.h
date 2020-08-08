/*
	HDC2080.h
	HDC2010.h originally created by: Brandon Fisher, August 1st 2017
	
	This code is release AS-IS into the public domain, no guarantee or warranty is given.
	
	Description: This header file accompanies HDC2080.cpp, and declares all methods, fields,
	and constants used in the source code. 
*/

#ifndef HDC2080_H
#define HDC2080_h

#include <Arduino.h>
#include <Wire.h>

//  Constants for setting measurement resolution
#define FOURTEEN_BIT 0
#define ELEVEN_BIT 1
#define NINE_BIT  2

//  Constants for setting sensor mode
#define TEMP_AND_HUMID 0
#define TEMP_ONLY	   1
#define HUMID_ONLY	   2
#define ACTIVE_LOW	   0
#define ACTIVE_HIGH	   1
#define LEVEL_MODE		0
#define COMPARATOR_MODE 1

//  Constants for setting sample rate
#define MANUAL			0
#define TWO_MINS		1
#define ONE_MINS		2
#define TEN_SECONDS		3 
#define	FIVE_SECONDS	4
#define ONE_HZ			5
#define TWO_HZ			6
#define FIVE_HZ			7	

class HDC2080
{
	public:
		HDC2080() {};	        				// Initialize the HDC2080
		void begin(uint8_t addr);    			// Join I2C bus
		float readTemp(void);					// Returns the temperature in degrees C
		float readHumidity(void);				// Returns the relative humidity
		void enableHeater(void);				// Enables the heating element
		void disableHeater(void);				// Disables the heating element
		void setLowTemp(float temp);			// Sets low threshold temperature (in c)
		void setHighTemp(float temp);			// Sets high threshold temperature (in c)
		void setHighHumidity(float humid);		// Sets high Humiditiy threshold
		void setLowHumidity(float humid);		// Sets low Humidity threshold 
		float readLowHumidityThreshold(void);	// Returns contents of low humidity threshold register
		float readHighHumidityThreshold(void);	// Returns contents of high humidity threshold register
		float readLowTempThreshold(void);		// Returns contents of low temperature threshold register (in C)
		float readHighTempThreshold(void);		// Returns contents of high temperature threshold register (in C)
		void triggerMeasurement(void);			// Triggers a manual temperature/humidity reading
		void reset(void); 						// Triggers a software reset
		void enableInterrupt(void);				// Enables the interrupt/DRDY pin
		void disableInterrupt(void); 			// Disables the interrupt/DRDY pin (High Z)
		uint8_t readInterruptStatus(void); 		// Reads the status of the interrupt register
		void clearMaxTemp(void);				// Clears the Maximum temperature register
		void clearMaxHumidity(void);			// Clears the Maximum humidity register
		float readMaxTemp(void); 				// Reads the maximum temperature register
		float readMaxHumidity(void);			// Reads the maximum humidity register
		uint16_t readManufacturerId(void);
		uint16_t readDeviceId(void);
		void enableThresholdInterrupt(void);	// Enables high and low temperature/humidity interrupts
		void disableThresholdInterrupt(void);	// Disables high and low temperature/humidity interrupts
		void enableDRDYInterrupt(void);			// Enables data ready interrupt
		void disableDRDYInterrupt(void);		// Disables data ready interrupt
		
		
		
		/* Sets Temperature & Humidity Resolution, 3 options
		   0 - 14 bit
		   1 - 11 bit
		   2 - 9 bit
		   default - 14 bit							*/
		void setTempRes(int resolution);		
		void setHumidRes(int resolution);	

		/* Sets measurement mode, 3 options
		   0 - Temperature and Humidity
		   1 - Temperature only
		   2 - Humidity only
		   default - Temperature & Humidity			*/
		void setMeasurementMode(int mode);
		
		/* Sets reading rate, 8 options
		   0 - Manual
		   1 - reading every 2 minutes
		   2 - reading every minute
		   3 - reading every ten seconds
		   4 - reading every 5 seconds
		   5 - reading every second
		   6 - reading at 2Hz
		   7 - reading at 5Hz
		   default - Manual		*/		
		void setRate(int rate);
		
		/* Sets Interrupt polarity, 2 options
		   0 - Active Low
		   1 - Active High
		   default - Active Low			*/		
		void setInterruptPolarity(int polarity);
		
		/* Sets Interrupt mode, 2 options
		   0 - Level sensitive
		   1 - Comparator mode
		   default - Level sensitive	*/		
		void setInterruptMode(int polarity);
		
	private:
		int _addr; 									// Address of sensor 
		void openReg(uint8_t reg); 	    			// Points to a given register
		uint8_t readReg(uint8_t reg);				// Reads a given register, returns 1 byte
		void writeReg(uint8_t reg, uint8_t data); 	// Writes a byte of data to one register
		

};


#endif