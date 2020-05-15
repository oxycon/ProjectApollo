//
//    Valve cycling code for Project Apollo v1 prototype
//

#include "apollo_board_pinout.h"
#include "oxygen_sensor.h"
#include "debug_console.h"
#include "valve_timing.h"


void setup() {
  Serial.begin(115200);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(valve_2way, OUTPUT);
  pinMode(valve_5way, OUTPUT);
  pinMode(relief_valve, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // turn off relief valve initially
  digitalWrite(relief_valve, LOW);    

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
  DBG_println_float("- Sleeping ", (float)milliseconds/1000, " seconds ...", 3);
  delay(milliseconds);
}

void ActuateReliefValveIfNeeded()
{
  for(int i = 0; i < 1000; i++)
  {
    bool button = GetReliefValveButtonStatus();
    if (!button)
      break;
      
    DBG_println("- Relief button pressed!");
  
    // turn on the relief valve
    digitalWrite(relief_valve, HIGH);    
    delay(50);
  }

  // turn off relief valve
  digitalWrite(relief_valve, LOW);
}

void halfCycle(int state_5way)
{
  // Read O2 value
  loop_o2sensor(100);

  digitalWrite(LED_BUILTIN, state_5way);   // turn the built-in LED with the same value as 5-WAY state

  // Turn on 5-way
  setValve("5-way", valve_5way, state_5way);    

  // Wait for 5-way to be opened for the given amount of time
  delay_with_dbg_info(Get5WayValveTimingMilliseconds());

  // Turn on the 2-way briefly
  setValve("2-way", valve_2way, HIGH);    

  delay_with_dbg_info(Get2WayValveTimingMilliseconds());
  setValve("2-way", valve_2way, LOW);    

  ActuateReliefValveIfNeeded();
}

void loop() {
  DBG_println_buffered("- Start HIGH cycle ...")
  halfCycle(HIGH);
  DBG_println_buffered("- Start LOW cycle ...")
  halfCycle(LOW);  
}
