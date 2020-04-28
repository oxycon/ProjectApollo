//
//    Valve cycling code for Project Apollo v1 prototype
//

#include "oxygen_sensor.h"
#include "debug_console.h"


// Connectivity:
// - Connect 5V from Arduino to VCC (Power connector) on Gasboard 7500E 
// - Connect GND from Arduino to GND (Power connector) on Gasboard 7500E 
// - Connect PIN 2 (Arduino RX) with TX (IO connector) on Gasboard 7500E 
// - Connect PIN 3 (Arduino TX) with RX (IO connector) on Gasboard 7500E 
// - Connect PIN digital GND Arduino with GND (IO connector) on Gasboard 7500E 
// 
// - Connect Vin from Arudino to 12V(+) on ULN2003 board and to (+) of 2-way valve motor and to (+) of 5-way valve motor
// - Connect GND from Arduino to GND(-) on ULN2003 board 
// - Connect PIN 6 pin from Arduino to IN PIN 1 on ULN2003 board
// - Connect PIN 7 pin from Arduino to IN PIN 2 on ULN2003 board
// - Connect PIN 8 pin from Arduino to IN PIN 3 on ULN2003 board
// - Connect OUT PIN 1 pin from ULN2003 board to (-) of 2-way valve
// - Connect OUT PIN 2 pin from ULN2003 board to (-) of 5-way valve
// - Connect OUT PIN 3 pin from ULN2003 board to (-) of fan
// - Connect OUT PIN 5 pin to common VCC(+) for all the consumers above. 
// - Program Arduino board
// - Verify that the LEDs blink in the right order
// - Connect 12V supply to Arduino (at least 1.5A current)
// - Verify that valves couple in the right order 


// PIN identifiers

// 
// PIN 6 on Arduino Uno
// - To be connnected to INPUT PIN 1 on ULN2003 board 
// - Which drives OUT PIN 1 on ULN2003 board 
// - Which drives (-) on 2-way valve
// 
int valve_2way = 6;

// 
// PIN 7 on Arduino Uno
// - To be connnected to INPUT PIN 2 on ULN2003 board 
// - Which drives OUT PIN 2 on ULN2003 board 
// - Which drives (-) on 5-way valve
// 
int valve_5way = 7;

// 
// PIN 8 on Arduino Uno
// - To be connnected to INPUT PIN 3 on ULN2003 board 
// - Which drives OUT PIN 3 on ULN2003 board 
// - Which drives (-) on the FAN
// 
int fan_control = 8;

// Scale in which timing values are expressed (as multiplies of the number below)
int timing_unit_scale_milliseconds = 1000;

// Time for opening the 2-way valve
// 1 time units 
int timing_valve_2way = 2; 

// Time for alternating the 5-way valve
// 8 time units 
int timing_valve_5way_alternating = 7; 

void setup() {
  Serial.begin(115200);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(valve_2way, OUTPUT);
  pinMode(valve_5way, OUTPUT);
  pinMode(fan_control, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // make the fan work all the time for now
  // TBD - add temperature-control driven fan (based on the temperature of the copper loop)
  digitalWrite(fan_control, HIGH);    

  // Setup O2 sensor
  setup_o2sensor();

}

void setValve(char* valveName, int valvePortNum, int newValue)
{
  digitalWrite(valvePortNum, newValue);    
  // print out the human readable data
  DBG_println_buffered("- Setting valve: %s (%d) to %s", valveName, valvePortNum, (newValue == HIGH)? "HIGH": "LOW");
}

void delay_with_dbg_info(int milliseconds)
{
  DBG_println_float("- Sleeping ", milliseconds/1000, " seconds ...", 3);
  delay(milliseconds);
}


void halfCycle(int state_5way)
{
  // Read O2 value
  loop_o2sensor();

  digitalWrite(LED_BUILTIN, state_5way);   // turn the built-in LED with the same value as 5-WAY state

  // Turn on 5-way
  setValve("5-way", valve_5way, state_5way);    

  // Wait for 5-way to be opened for the given amount of time
  delay_with_dbg_info(timing_valve_5way_alternating * timing_unit_scale_milliseconds);

  // Turn on the 2-way briefly
  setValve("2-way", valve_2way, HIGH);    

  delay_with_dbg_info(timing_valve_2way * timing_unit_scale_milliseconds);
  setValve("2-way", valve_2way, LOW);    
}

void loop() {
  DBG_println_buffered("- Start HIGH cycle ...")
  halfCycle(HIGH);
  DBG_println_buffered("- Start LOW cycle ...")
  halfCycle(LOW);  
}
