
#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Valve.h"


// Maps valve numbers to pins.
const int8_t valve_pin_map[] PROGMEM {
  VALVE_0_PIN,
  VALVE_1_PIN,
  VALVE_2_PIN,
  VALVE_3_PIN,
  VALVE_4_PIN
};

uint8_t current_valve_states = 0;

void valve_setup() {
  for (size_t n=0; n<sizeof(valve_pin_map); n++) {
    pinMode(valve_pin_map[n], OUTPUT);
    digitalWrite(valve_pin_map[n], LOW);
  }
}

void set_valves(const uint8_t states, const uint8_t mask/*=0b11111111*/) {
  current_valve_states = (current_valve_states & ~mask) | states;
  for (size_t n=0; n<sizeof(valve_pin_map); n++) {
    digitalWrite(valve_pin_map[n], (current_valve_states >>n) & 1 );    
  }
}

bool set_valve(size_t n, bool state) {
  DEBUG_printf(FS("Set Valve %d to %d\n"), n, state);
  current_valve_states = (current_valve_states & ~(1<<n)) | (((uint8_t) state) << n);
  digitalWrite(valve_pin_map[n], (current_valve_states >>n) & 1 );  
}
