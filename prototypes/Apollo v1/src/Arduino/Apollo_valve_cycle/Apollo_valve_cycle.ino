//
//    Valve cycling code for Project Apollo v1 prototype
//


// Connectivity:
// - Connect GND from Arduino to GND(-) on ULN2003 board 
// - Connect Vin from Arudino to 12V(+) on ULN2003 board and to (+) of 2-way valve motor and to (+) of 5-way valve motor
// - Connect PIN 2 pin from Arduino to IN PIN 1 on ULN2003 board
// - Connect PIN 3 pin from Arduino to IN PIN 2 on ULN2003 board
// - Connect PIN 4 pin from Arduino to IN PIN 3 on ULN2003 board
// - Connect OUT PIN 1 pin from ULN2003 board to (-) of 2-way valve
// - Connect OUT PIN 2 pin from ULN2003 board to (-) of 5-way valve
// - Connect OUT PIN 3 pin from ULN2003 board to (-) of fan
// - Program Arduino board
// - Verify that the LEDs blink in the right order
// - Connect 12V supply to Arduino (at least 1.5A current)
// - Verify that valves couple in the right order 


// PIN identifiers

// 
// PIN 2 on Arduino Uno
// - To be connnected to INPUT PIN 1 on ULN2003 board 
// - Which drives OUT PIN 1 on ULN2003 board 
// - Which drives (-) on 2-way valve
// 
int valve_2way = 2;

// 
// PIN 3 on Arduino Uno
// - To be connnected to INPUT PIN 2 on ULN2003 board 
// - Which drives OUT PIN 2 on ULN2003 board 
// - Which drives (-) on 5-way valve
// 
int valve_5way = 3;

// 
// PIN 4 on Arduino Uno
// - To be connnected to INPUT PIN 3 on ULN2003 board 
// - Which drives OUT PIN 3 on ULN2003 board 
// - Which drives (-) on the FAN
// 
int fan_control = 4;

// Scale in which timing values are expressed (as multiplies of the number below)
int timing_unit_scale_milliseconds = 1;

// 1000/2/7 = 32.5%
// 1000/1/14 = 35.6%, 45 psi max
// 1/0.5/14 = 30.5%, 45 psi max
// 1/2.5/0.5/14 = 38.5%, 45 psi max
// 1/3.5/0.5/12 = 39.1%, 45 psi max
// 1/2.5/0.5/12 = 39.2%, 45 psi max
// 1/2.5/0.5/12 = 39.2%, 45 psi max
// 1/1.5/0.1/14 = 38.0%, 45 psi
// 1/2.5/0.5/12 = 38.0%, 45 psi

// Time for opening the 2-way valve
// 1 time units 
int timing_valve_2way = 2500; 
int timing_valve_2way_after_switch = 500;

// Time for alternating the 5-way valve
// 8 time units 
int timing_valve_5way_alternating = 12000; 

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(valve_2way, OUTPUT);
  pinMode(valve_5way, OUTPUT);
  pinMode(fan_control, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // make the fan work all the time for now
  // TBD - add temperature-control driven fan (based on the temperature of the copper loop)
  digitalWrite(fan_control, HIGH);    
}

void halfCycle(int state_5way)
{
  digitalWrite(LED_BUILTIN, state_5way);   // turn the built-in LED with the same value as 5-WAY state

  delay(timing_valve_2way_after_switch * timing_unit_scale_milliseconds);
  digitalWrite(valve_2way, LOW);    

  // Turn on 5-way
  digitalWrite(valve_5way, state_5way);    
  // Wait for 5-way to be opened for the given amount of time
  delay(timing_valve_5way_alternating * timing_unit_scale_milliseconds);

  // Turn on the 2-way briefly
  digitalWrite(valve_2way, HIGH);    
  delay(timing_valve_2way * timing_unit_scale_milliseconds);
}

void loop() {
  halfCycle(HIGH);
  halfCycle(LOW);  
}
