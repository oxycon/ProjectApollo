#include <PID_v1.h>

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
 * - from Arduino Uno to PCA9685
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

const int I2C_clock_speed = 100000;
const int SPI_clock_speed = 5000;
const int SPI_chipSelectPin = 10;
const int pressureSensorEnablePin = 9;

double pressureSetpoint, pressureInput, pressureOutput;

//Specify the links and initial tuning parameters
PID myPID(&pressureInput, &pressureOutput, &pressureSetpoint, 2, 5, 1, P_ON_M, DIRECT);

void setup() 
{
  pwm.begin();
  pwm.setPWMFreq(1000); 

  Wire.setClock(I2C_clock_speed);

  pressure_init();

  Serial.begin(115200);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  // Set target pressure at 2 psi
  pressureSetpoint = 2;
}

void pressure_init()
{
  SPI.begin(); 
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  SPI.setDataMode(SPI_MODE0);
  
  pinMode(SPI_chipSelectPin, OUTPUT);
  digitalWrite(SPI_chipSelectPin, HIGH);

  pinMode(pressureSensorEnablePin, OUTPUT);
  digitalWrite(SPI_chipSelectPin, LOW);
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

void printByteArray(char* text, int iteration, byte bufferPressureValue[], int bufferSize, bool appendNewLine = true)
{
    Serial.print(text);
    Serial.print("[");
    Serial.print(iteration);
    Serial.print("] = ");    
    for(int i = 0; i < bufferSize; i++)
    {
      if (i > 0)
        Serial.print(", ");
      Serial.print(bufferPressureValue[i]);
    }
    if (appendNewLine)
      Serial.println();
}

void pressure_read()
{
  // TODO - fix SPI settings
  // Serial.println("- start SPI");
  // SPI.beginTransaction(SPISettings(SPI_clock_speed, MSBFIRST, SPI_MODE0));

  // Reset the sensor by making all its pins High-Z. Wait some time 
  digitalWrite(pressureSensorEnablePin, LOW);
  delay(1);

  // Enable the sensor 
  digitalWrite(pressureSensorEnablePin, HIGH);

  // Initialize sensor and initiate a reading/conversion
  bool success = false;
  byte status = 0;
  
  for(int i = 0; i < 100; i++)
  {
    success = false;
    
    // Send the init command 
    byte bufferInit[4] = {0xAA, 0, 0};
    // printByteArray("- [INIT] IN", i, bufferInit, 3, false);
 
    digitalWrite(SPI_chipSelectPin, LOW);
    SPI.transfer(&bufferInit, 3);
    digitalWrite(SPI_chipSelectPin, HIGH);

    status = bufferInit[0];
  
    // printByteArray(", OUT", i, bufferInit, 3, true);
    // println_MPR_status(bufferInit[0]);

    // Delay 5 ms to allow convergence. 
    // TODO - also probe EOC pin
    delay(5);
   
    // Serial.println("- Looping until busy flag cleared ... ");
    for(int i = 0; i < 300; i++)
    {
      // Wait for busy flag to clear
      byte nopCmd = 0xF0;
      
      digitalWrite(SPI_chipSelectPin, LOW);
      status = SPI.transfer(nopCmd);
      digitalWrite(SPI_chipSelectPin, HIGH);
  
      if ((status & (1 << 5)) == 0)
      {
        // println_MPR_status(status);
        success = true;
        break;
      }

      // Serial.print(".");
      // if (i % 100 == 0) Serial.println();

      // Wait some more time to allow for more conversion
      delay(3);
    }

    if (success)
      break;
      
    // Serial.println();
  }

  if (!success)
  {
    // TODO - error case! 
    Serial.println("ERROR initializing/getting convergence");
    println_MPR_status(status);
  }
  
  byte bufferPressureValue[4];

  // Dump output reading a few times
  for(int i = 0; i < 100; i++)
  {
    bufferPressureValue[0] = 0xF0;
    bufferPressureValue[1] = 0;
    bufferPressureValue[2] = 0;
    bufferPressureValue[3] = 0;
      
    digitalWrite(SPI_chipSelectPin, LOW);
    SPI.transfer(&bufferPressureValue, 4);
    digitalWrite(SPI_chipSelectPin, HIGH);
    
    // Values for sensor Honeywell MPRLS0030PG0000SA 
    // Gage sensor, Transfer function A
    // Max pressure = 30 psi, min pressure = 0 psi

    long outputPressure = ((long)bufferPressureValue[1]) << 16;
    outputPressure += ((long)bufferPressureValue[2]) << 8;
    outputPressure += ((long)bufferPressureValue[3]);

    // Output min. = 1677722 counts (10% of 2^24 counts or 0x19999A) 
    const long outputPressureMin = 0x19999AL;

    // Output max. = 15099494 counts (90% of 2^24 counts or 0xE66666) 
    const long outputPressureMax = 0xE66666L;

    // Output pressure delta max
    const long deltaOutputPressureMax = outputPressureMax - outputPressureMin;

    // Min/max pressures for this sensor
    const float pressureMinPsi = 0;
    const float pressureMaxPsi = 30;

    if (bufferPressureValue[0] != 64)
    {
      Serial.print("- ERROR reading data: ");
      println_MPR_status(bufferPressureValue[0]);
      printByteArray("DATA", i, bufferPressureValue, 4);
    }
    else
    {
      // float convertedPressure = map(outputPressure, outputPressureMin, outputPressureMax, pressureMinPsi, pressureMaxPsi);
      float convertedPressurePsi = (outputPressure - outputPressureMin) * (pressureMaxPsi - pressureMinPsi) / (outputPressureMax - outputPressureMin) + pressureMinPsi;
      
      Serial.print("Pressure:");
      Serial.print(convertedPressurePsi);
      Serial.println();

      pressureInput = convertedPressurePsi;
      break;
    }

    delay(1);
  }

  // SPI.endTransaction();
  // Serial.println("- ended SPI");

  // Disable all pins to the sensor
  digitalWrite(pressureSensorEnablePin, LOW);
}


int rotational_sense = 1;
int max_steps = 2000;

int currentPin = 0;
int previousPin = 0;
int current_steps = 0;

int counter_MPR_step = 0;
int counter_MPR_step_reading = 100;

void test_rotate_valve()
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
}

void loop() 
{
  if (0 == (counter_MPR_step % counter_MPR_step_reading))
  {
    pressure_read();
  }
  counter_MPR_step++;

  myPID.Compute();

  test_rotate_valve();

  delay(1);
}
