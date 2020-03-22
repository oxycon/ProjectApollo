/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

// Constants
int valve_2way = 2;
int valve_5way = 3;

// Scale in which timing values are expressed (as multiplies of the number below)
int timing_unit_scale_milliseconds = 1000;

// Time for opening the 2-way valve
// 1 time units 
int timing_valve_2way = 1; 

// Time for alternating the 5-way valve
// 8 time units 
int timing_valve_5way_alternating = 8; 

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(valve_2way, OUTPUT);
  pinMode(valve_5way, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void halfCycle(int state_5way)
{
  digitalWrite(LED_BUILTIN, state_5way);   // turn the built-in LED with the same value as 5-WAY state

  // Turn on 5-way
  digitalWrite(valve_5way, state_5way);    
  // Wait for 5-way to be opened for the given amount of time
  delay(timing_valve_5way_alternating * timing_unit_scale_milliseconds);

  // Turn on the 2-way briefly
  digitalWrite(valve_2way, HIGH);    
  delay(timing_valve_2way * timing_unit_scale_milliseconds);
  digitalWrite(valve_2way, LOW);    
}

void loop() {
  halfCycle(HIGH);
  halfCycle(LOW);  
}
