#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/*
 * Components:
 * - Arduino Uno
 * - ULN2003 board
 * - PCA9685 board
 * - Breadboard + jumper wires
 * - Stepper motor (BYJ48)
 * 
 * Connections:
 * - from Arduino Uno to PCA9695
 *        5V    ->    Vcc 
 *        GND   ->    GND
 *        SCL   ->    SCL
 *        SDA   ->    SDA
 *        Vin   ->    Power V+
 *        GND   ->    Power GND
 * 
 *  - from Arduino Uno to ULN2003
 *        Vin   ->    Power + (5..12V)
 *        GND   ->    Power - 
 *        
 *  - from PCA9685 to ULN2003 board
 *        PWM pin 1   ->    IN 1
 *        PWM pin 2   ->    IN 2
 *        PWM pin 3   ->    IN 3
 *        PWM pin 4   ->    IN 4
 * 
 *  - from ULN2003 to BYJ48 stepper motor
 *        Use the standard 5-pin connector with the default wiring
 * 
 *  Notes: 
 *  - the stepper motor rotates using default power from USB assuming a high-power USB port (1A minim)
 */

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

void setup() {
  pwm.begin();
  pwm.setPWMFreq(1000); 

  Wire.setClock(400000);
}

void loop() {
  int rotational_sense = 1;
  int max_steps = 2000;

  int currentPin = 0;
  int previousPin = 0;
  int current_steps = 0;
  while(true)
  {
    if (currentPin == previousPin)
    {
      currentPin = (currentPin + rotational_sense) % 4;
      pwm.setPWM(currentPin, 4096, 0);
    }
    else
    {
      pwm.setPWM(previousPin, 0, 4096);
      previousPin = currentPin;
    }
    current_steps += 1;
    if (current_steps == max_steps)
    {
      current_steps = 0;
      rotational_sense = (rotational_sense + 2) % 4;
    }
    delay(1);
  }
}
