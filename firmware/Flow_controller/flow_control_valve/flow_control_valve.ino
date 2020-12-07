#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


/*
 * Components:
 * - Arduino Nano 33 IoT
 * - PCA9685 board
 * - ULN2003 board
 * - Breadboard + jumper wires
 * - Stepper motor (BYJ48)
 * 
 * Connections:
 * - from Arduino to PCA9685
 *        5V    ->    Vcc 
 *        GND   ->    GND
 *        SCL   ->    SCL
 *        SDA   ->    SDA
 *        Vin   ->    Power V+
 *        GND   ->    Power GND
 * 
 *  - from Arduino to ULN2003
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
 *  - from Arduino to Rotary encoder
 *        pin 3 = CLK_A
 *        pin 4 = DT_B
 *        pin 5 = Switch
 *        pin 6 = VCC
 *        pin 7 = GND
 *
 *  Notes: 
 *  - the stepper motor rotates using default power from USB assuming a high-power USB port (1A minimum)
 */

//
//  Constants
//

// Rotary encoder pins
const int rotaryEncoder_GND = 7;
const int rotaryEncoder_VCC = 6;
const int rotaryEncoder_Switch = 5;
const int rotaryEncoder_DT_B = 4;
const int rotaryEncoder_CLK_A = 3;

// Variables to debounce Rotary Encoder
const int DelayofDebounce = 0.01;

const int I2C_clock_speed = 100000;

const int I2C_Address_PCA9685 = 0x40;

// Stepper constants
const int max_steps = 2000/8;


//
//  State
//

// PWM driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(I2C_Address_PCA9685);

// Rotary encoder state: previous Pins
long timeOfLastDebounce = 0;
int previousCLK = 0;   
int previousDATA = 0;


// Stepper state
static int current_test_steps = 0;
static bool forwardRotation = true;

double current_valve_position = 10;
double desired_valve_position = 10;
double target_flow = 10;




int stepper_one_cycle(bool forwardRotation = true)
{
  int rotational_sense = forwardRotation? 1: 3;
  int currentPin = 0;
  int previousPin = 0;

  for(int i = 0; i < 8; i++)
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
    
    delay(1);
  }
}

void test_rotate_valve()
{
  if (current_test_steps >= max_steps)
  {
    current_test_steps = 0;
    forwardRotation = !forwardRotation;
  }
  else
  {
    current_test_steps++;
  }

  stepper_one_cycle(forwardRotation);
}



void init_valve()
{
  // Position stepper to the min value of the valve
  // At some point it will get stuck but that's okay
  for(int i = 0; i < max_steps + 1; i++)
    stepper_one_cycle(true);

  current_valve_position = 0;
}

void reposition_valve_one_step()
{
  if ((current_valve_position <= desired_valve_position) && ((current_valve_position + 1) > desired_valve_position))
  {
    // Serial.println("No steps needed");
    // Compensate for delay in missing stepper_one_cycle()
    delay(8);
    return;
  }

  if (current_valve_position < desired_valve_position)
  {
    // Serial.println("move forward");
    stepper_one_cycle(false);

    if (current_valve_position < max_steps)
      current_valve_position += 1;

    return;
  }

  if (current_valve_position > desired_valve_position)
  {
    // Serial.println("move back");
    stepper_one_cycle(true);

    if (current_valve_position > 0)
      current_valve_position -= 1;
  }
}


// Adjust target pressure in 1 psi increments
// Check if Rotary Encoder was moved
void check_rotary_encoder() {

 if ((previousCLK == 0) && (previousDATA == 1)) {
    if ((digitalRead(rotaryEncoder_CLK_A) == 1) && (digitalRead(rotaryEncoder_DT_B) == 0)) {
      target_flow++;
    }
    if ((digitalRead(rotaryEncoder_CLK_A) == 1) && (digitalRead(rotaryEncoder_DT_B) == 1)) {
      target_flow--;
    }
  }

if ((previousCLK == 1) && (previousDATA == 0)) {
    if ((digitalRead(rotaryEncoder_CLK_A) == 0) && (digitalRead(rotaryEncoder_DT_B) == 1)) {
      target_flow++;
    }
    if ((digitalRead(rotaryEncoder_CLK_A) == 0) && (digitalRead(rotaryEncoder_DT_B) == 0)) {
      target_flow--;
    }
  }

if ((previousCLK == 1) && (previousDATA == 1)) {
    if ((digitalRead(rotaryEncoder_CLK_A) == 0) && (digitalRead(rotaryEncoder_DT_B) == 1)) {
      target_flow++;
    }
    if ((digitalRead(rotaryEncoder_CLK_A) == 0) && (digitalRead(rotaryEncoder_DT_B) == 0)) {
      target_flow--;
    }
  }  

if ((previousCLK == 0) && (previousDATA == 0)) {
    if ((digitalRead(rotaryEncoder_CLK_A) == 1) && (digitalRead(rotaryEncoder_DT_B) == 0)) {
      target_flow++;
    }
    if ((digitalRead(rotaryEncoder_CLK_A) == 1) && (digitalRead(rotaryEncoder_DT_B) == 1)) {
      target_flow--;
    }
  }            
}


void setup() 
{
  Serial.begin(115200);
    
  pwm.begin();
  pwm.setPWMFreq(1000); 

  Serial.println(F("starting ..."));

  Wire.setClock(I2C_clock_speed);

  // Setup rotary encoder
  pinMode(rotaryEncoder_GND,OUTPUT);
  pinMode(rotaryEncoder_VCC,OUTPUT);
  pinMode(rotaryEncoder_Switch,INPUT_PULLUP);
  pinMode(rotaryEncoder_DT_B,INPUT);
  pinMode(rotaryEncoder_CLK_A,INPUT);

  digitalWrite(rotaryEncoder_GND, LOW);
  digitalWrite(rotaryEncoder_VCC, HIGH);

  previousDATA=digitalRead(rotaryEncoder_DT_B);
  previousCLK=digitalRead(rotaryEncoder_CLK_A);

  Serial.println(F("started."));

  init_valve();
}


void loop() 
{

  // test_rotate_valve();

  // If enough time has passed check the rotary encoder
  if ((millis() - timeOfLastDebounce) > DelayofDebounce) 
  {
    check_rotary_encoder();
    
    previousCLK=digitalRead(rotaryEncoder_CLK_A);
    previousDATA=digitalRead(rotaryEncoder_DT_B);
   
    timeOfLastDebounce=millis();  // Set variable to current millis() timer
  }  


  Serial.println(target_flow);

  desired_valve_position = target_flow;

  reposition_valve_one_step();
}
