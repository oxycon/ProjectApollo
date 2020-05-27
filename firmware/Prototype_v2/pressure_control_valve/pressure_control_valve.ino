#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <SPI.h>

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

const int SPI_clock_speed = 5000;
const int SPI_chipSelectPin = 10;

void setup() {
  // pwm.begin();
  // pwm.setPWMFreq(1000); 

  // Wire.setClock(shared_clock_speed);

  pressure_init();

  Serial.begin(115200);
  Serial.println("################## end setup() ##################");
}

void pressure_init()
{
  SPI.begin(); 
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  SPI.setDataMode(SPI_MODE0);
  
  pinMode(SPI_chipSelectPin, OUTPUT);
  digitalWrite(SPI_chipSelectPin, HIGH);
}

void println_MPR_status(byte status)
{
  Serial.print("- Status = ");
  Serial.print(status);
  
  if ((status & (1 << 7)) || (status & (1 << 4)) || (status & (1 << 3)) || (status & (1 << 1)))
    Serial.print(", comm ERROR! (some zero bits are 1)");

  if (status & 1)
    Serial.print(", math saturation ERROR");

  if (status & (1 << 2))
    Serial.print(", integrity test FAILED!");
  else
    Serial.print(", integrity test passed");

  if (status & (1 << 6))
    Serial.print(", powered on");
  else 
    Serial.print(", powered OFF");
    
  if (status & (1 << 5))
    Serial.print(", BUSY!");
  else 
    Serial.print(", not busy");

  Serial.println();
}

void pressure_read()
{
  // Serial.println("- start SPI");
  // SPI.beginTransaction(SPISettings(SPI_clock_speed, MSBFIRST, SPI_MODE0));

  bool success = false;
  while(!success)
  {
    success = false;
    
    // Send the init command 
    byte bufferInit[4] = {0xAA, 0, 0};
    Serial.print("- INIT: IN = ");
    Serial.print(bufferInit[0]);
    Serial.print(", ");
    Serial.print(bufferInit[1]);
    Serial.print(", ");
    Serial.print(bufferInit[2]);
 
    digitalWrite(SPI_chipSelectPin, LOW);
    SPI.transfer(&bufferInit, 3);
    digitalWrite(SPI_chipSelectPin, HIGH);
  
    Serial.print(", OUT = ");
    Serial.print(bufferInit[0]);
    Serial.print(", ");
    Serial.print(bufferInit[1]);
    Serial.print(", ");
    Serial.print(bufferInit[2]);
    Serial.println();
  
    println_MPR_status(bufferInit[0]);
  
    delay(5);
   
    Serial.println("- Looping until busy flag cleared ... ");
    for(int i = 0; i < 300; i++)
    {
      // Wait for busy flag to clear
      byte nopCmd = 0xF0;
      
      digitalWrite(SPI_chipSelectPin, LOW);
      byte status = SPI.transfer(nopCmd);
      digitalWrite(SPI_chipSelectPin, HIGH);
  
      if ((status & (1 << 5)) == 0)
      {
        println_MPR_status(status);
        Serial.println(i);
        Serial.println("- MPR not busy. Exiting loop!");
        success = true;
        break;
      }
  
      Serial.print(".");
      if (i % 100 == 0)
        Serial.println();
      
      delay(3);
    }
    Serial.println();

    delay(1000);
  }
  
  Serial.println("- Loop exited");

  byte bufferPressureValue[4];

  for(int i = 0; i < 100; i++)
  {
    bufferPressureValue[0] = 0xF0;
    bufferPressureValue[1] = 0;
    bufferPressureValue[2] = 0;
    bufferPressureValue[3] = 0;
      
    digitalWrite(SPI_chipSelectPin, LOW);
    SPI.transfer(&bufferPressureValue, 4);
    digitalWrite(SPI_chipSelectPin, HIGH);
    
    Serial.print("- Pressure value[");
    Serial.print(i);
    Serial.print("] = ");    
    Serial.print(bufferPressureValue[0]);
    Serial.print(", ");
    Serial.print(bufferPressureValue[1]);
    Serial.print(", ");
    Serial.print(bufferPressureValue[2]);
    Serial.print(", ");
    Serial.println(bufferPressureValue[3]);
    delay(20);

    if (bufferPressureValue[0] != 64)
    {
      println_MPR_status(bufferPressureValue[0]);
    }

    if (bufferPressureValue[1] == 0)
    {
      Serial.println(".................");
      break;
    }
  }




  // SPI.endTransaction();
  Serial.println("- ended SPI");

}

void loop() {
  int rotational_sense = 1;
  int max_steps = 2000;

  int currentPin = 0;
  int previousPin = 0;
  int current_steps = 0;

  int counter_MPR_step = 0;
  int counter_MPR_step_reading = 500;
  
  while(true)
  {
    /*
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
    */
    delay(1);
    
    if (0 == (counter_MPR_step % counter_MPR_step_reading))
    {
      pressure_read();
      delay(500);
    }
    counter_MPR_step++;
  }
}
