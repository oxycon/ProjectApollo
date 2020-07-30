# Overview

This simple Arduino example demonstrates driving a stepper motor using a combination of PCA9685 I2C multiplexer board and an ULN2003 Darlington driver. 

One single PCA9685 board can drive up to 4 stepper motors. The example can be easily extended to drive a very large number of stepper motors using one Arduino board, simply by chaining multiple PCA9685 boards.

We demonstrate this using a BYJ48 stepper motor which can be driven straight from the USB bus. 

All components involved are extremely inexpensive. 

# Components
- Arduino Uno or equivalent
- ULN2003 board
- PCA9685 board
- Stepper motor (BYJ48)
- Breadboard + jumper wires

# Connections
- from Arduino Uno to PCA9695
  - 5V    ->    Vcc 
  - GND   ->    GND
  - SCL   ->    SCL
  - SDA   ->    SDA
  - Vin   ->    Power V+
  - GND   ->    Power GND

- from Arduino Uno to ULN2003
  - Vin   ->    Power + (5..12V)	
  - GND   ->    Power - 
       
- from PCA9685 to ULN2003 board
  - PWM pin 1   ->    IN 1
  - PWM pin 2   ->    IN 2
  - PWM pin 3   ->    IN 3
  - PWM pin 4   ->    IN 4

- from ULN2003 to BYJ48 stepper motor
  - Use the standard 5-pin connector with the default wiring

# Notes
- the stepper motor rotates using default power from USB assuming a high-power USB port (1A minim)