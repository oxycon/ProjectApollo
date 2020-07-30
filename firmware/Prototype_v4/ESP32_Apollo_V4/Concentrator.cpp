#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Concentrator.h"
#include "Valve.h"

/*
 * The Concentrator is based on a state machine.
 */

bool concentrator_is_enabled = false;
unsigned long next_state_ms = 0;
unsigned int concentrator_state = 0;

void concentrator_start() {
  DEBUG_print(F("Start Concentrator\n"));
  concentrator_state = 0;
  next_state_ms = millis();
  concentrator_is_enabled = true;
}

void concentrator_stop() {
  DEBUG_print(F("Stop Concentrator\n"));
  concentrator_is_enabled = false;
}

// Cycle through the valve states
void concentrator_run() {
  if (!concentrator_is_enabled) { return; }
  if (millis() < next_state_ms) { return; }
  
  set_valves(config.concentrator.valve_state[concentrator_state], config.concentrator.cycle_valve_mask);
  next_state_ms += config.concentrator.state_ms[concentrator_state];
  // DEBUG_printf(FS("State:%u Valves:%x  Next:%u\n"), concentrator_state, config.concentrator.valve_state[concentrator_state], next_state_ms);
  concentrator_state++;
  if (concentrator_state >= config.concentrator.state_count) { concentrator_state = 0; }
}
