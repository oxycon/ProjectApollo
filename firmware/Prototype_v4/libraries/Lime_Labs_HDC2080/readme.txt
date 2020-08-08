Texas Instruments HDC2080 Arduino Library
==========================================
*** NOTE: This library was originally from https://training.ti.com/how-interface-hdc2010-humidity-and-temperature-sensor-arduino-using-i2c
*** This library was originally for the HDC2010 and was converted to support the HDC2080.
*** forked from https://github.com/tinkeringtech/HDC2080_breakout to https://github.com/lime-labs/HDC2080-Arduino for use in the "Lime Labs Hot Slice" temperature and humidity logger project
*** contains fixes and cleanups generously provided by https://github.com/stickbreaker to fix behavior on newer ESP32 Arduino core releases
*** generalized the error handling to restore compatibilty with broader range of MCUs and Arduino cores.

Description: 
	The HDC2080 Library for Arduino allows for configuration and use of 
	the HDC2080 without the need to use I2C or know the register mapping of 
	the device. 
	
	Feel free to modify and reuse this library as you see fit. This code is
	released AS-IS into the public domain. No guarantee or warranty is given.
	This code will not be supported by Texas Instruments after release. 
	
---------------------------------------------------------------------------------------
Installation:
	Extract or copy the HDC2080 Arduino Library folder into the 'libraries'
	folder found in your Arduino Sketchbook. The example sketch will then
	be available under 'custom libraries' when you select File-> Examples.
	
---------------------------------------------------------------------------------------
Contents:
	HDC2080.cpp 		- C++ File defining  behavior of HDC2080 methods
	HDC2080.h   		- Header file for HDC2080.cpp
	keywords.txt		- Keywords from HDC2080.h
	library.properties 	- Properties file for HDC2080 library
	Examples 		- Holds HDC2080 example sketch

----------------------------------------------------------------------------------------

Library Functions:

		HDC2080(uint8_t addr)
			- Initializes an HDC2080 object
			
		void begin(void)  						
			- Equivalent to wire.Begin(), Joins I2C Bus
			
		float readTemp(void)					
			- Returns the temperature in degrees C
		
		float readHumidity(void)				
			- Returns the relative humidity
		
		void enableHeater(void)				
			- Enables the heating element
		
		void disableHeater(void)
			- Disables the heating element
			
		void setLowTemp(float temp)			
			- Sets the lower temperature threshold (in celcius). Temperature may not
			  be exact due to 8 bit quantization.
			  
		void setHighTemp(float temp)	
			- Sets the upper temperature threshold (in celcius). Temperature may not
			  be exact due to 8 bit quantization.	
			  
		void setHighHumidity(float humid)		
			- Sets the upper humidity threshold (as a %). Humidity may not
			  be exact due to 8 bit quantization.
		
		void setLowHumidity(float humid)	
			- Sets the lower humidity threshold (as a %). Humidity may not
			  be exact due to 8 bit quantization.
	
		float readLowHumidityThreshold(void)	
			- Returns contents of low humidity threshold register
			
		float readHighHumidityThreshold(void)	
			- Returns contents of high humidity threshold register
			
		float readLowTempThreshold(void)		
			- Returns contents of low temperature threshold register (in C)
			
		float readHighTempThreshold(void)
			- Returns contents of high temperature threshold register (in C)
			
		void triggerMeasurement(void)			
			- Triggers a manual temperature/humidity reading. Use to start
			  the taking of regular measurements.
			  
		void reset(void)						
			- Triggers a software reset, all registers are returned to default values
			
		void enableInterrupt(void)				
			- Enables the interrupt/DRDY pin
			
		void disableInterrupt(void)			
			- Disables the interrupt/DRDY pin (High Z)

		uint8_t readInterruptStatus(void)		
			- Reads the status of the interrupt register. The results can be interpreted
			  to know which interrupts have been generated. The HDC2010 datasheet explains
			  the contents of this register.
			  
		void clearMaxTemp(void)			
			- Clears the Maximum temperature register.
			
		void clearMaxHumidity(void)			
			- Clears the Maximum humidity register
			
		float readMaxTemp(void) 				
			- Reads the maximum temperature register
			
		float readMaxHumidity(void)			
			- Reads the maximum humidity register
			
		void enableThresholdInterrupt(void)	
			- Enables high and low temperature/humidity interrupts
			
		void disableThresholdInterrupt(void)	
			- Disables high and low temperature/humidity interrupts
			
		void enableDRDYInterrupt(void)		
			- Enables data ready interrupt
			
		void disableDRDYInterrupt(void)		
			- Disables data ready interrupt
		
		
		

		void setTempRes(int resolution)		
		void setHumidRes(int resolution)	
		   Sets Temperature & Humidity Resolution, 3 options
		   0 - 14 bit
		   1 - 11 bit
		   2 - 9 bit
		   default - 14 bit	


		void setMeasurementMode(int mode)
		   Sets measurement mode, 3 options
		   0 - Temperature and Humidity
		   1 - Temperature only
		   2 - Humidity only
		   default - Temperature & Humidity	
		   
	
		void setRate(int rate)
		   Sets reading rate, 8 options
		   0 - Manual
		   1 - reading every 2 minutes
		   2 - reading every minute
		   3 - reading every ten seconds
		   4 - reading every 5 seconds
		   5 - reading every second
		   6 - reading at 2Hz
		   7 - reading at 5Hz
		   default - Manual			
		

		void setInterruptPolarity(int polarity)
		   Sets Interrupt polarity, 2 options
		   0 - Active Low
		   1 - Active High
		   default - Active Low		
		
	
		void setInterruptMode(int polarity)
		   Sets Interrupt mode, 2 options
		   0 - Level sensitive
		   1 - Comparator mode
		   default - Level sensitive	
