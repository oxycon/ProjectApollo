#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Concentrator.h"
#include "Valve.h"

/*
 * The Concentrator is based on a state machine.
 */

bool concentrator_is_enabled = false;
unsigned long next_cycle_ms = 0;
unsigned int concentrator_cycle = 0;

void concentrator_start() {
  DEBUG_print(F("Start Concentrator\n"));
  concentrator_cycle = 0;
  next_cycle_ms = millis();
  concentrator_is_enabled = true;
}

void concentrator_stop() {
  DEBUG_print(F("Stop Concentrator\n"));
  concentrator_is_enabled = false;
}

// Loop through the valve cycles
void concentrator_run() {
  if (!concentrator_is_enabled) { return; }
  if (millis() < next_cycle_ms) { return; }
  
  set_valves(config.concentrator.valve_state[concentrator_cycle], config.concentrator.cycle_valve_mask);
  next_cycle_ms += config.concentrator.duration_ms[concentrator_cycle];
  // DEBUG_printf(FS("State:%u Valves:%x  Next:%u\n"), concentrator_state, config.concentrator.valve_state[concentrator_cycle], next_cycle_ms);
  concentrator_cycle++;
  if (concentrator_cycle >= config.concentrator.cycle_count) { concentrator_cycle = 0; }
}
