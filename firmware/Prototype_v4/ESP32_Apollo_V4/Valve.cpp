
#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Valve.h"


// Maps valve numbers to pins.
const int8_t valve_pin_map[] PROGMEM {
  VALVE_2_WAY_PIN,
  VALVE_5_WAY_PIN,
  VALVE_RELIEF_PIN,
};

uint8_t current_valve_states = 0;

void set_valves(const uint8_t states, const uint8_t mask/*=0b11111111*/) {
  current_valve_states = (current_valve_states & ~mask) | states;
  for (size_t n=0; n<sizeof(valve_pin_map); n++) {
    digitalWrite(valve_pin_map[n], (current_valve_states >>n) & 1 );    
  }
}

bool set_valve(size_t n, bool state) {
  current_valve_states = (current_valve_states & ~(1<<n)) | (((uint8_t) state) <<1);
  digitalWrite(valve_pin_map[n], (current_valve_states >>n) & 1 );  
}
