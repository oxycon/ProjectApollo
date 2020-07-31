#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Concentrator.h"
#include "Valve.h"

/*
 * The Concentrator is based on a state machine.
 * 
 * The current implementation is configurable and non-blocking. It needs to be called regularely from the main loop.
 * There is a possibility for jitter if the main loop is busy with other things e.g. sensors, display or (debug) serial.
 * 
 * If more percise timing is required, an implementation that is based on interrupts from a hardware timer can 
 * be considered. However this risks the possibility of race conditions when the main program is in the middle 
 * of talking to peripherals. In this case it may be best to directly control the valves through GPIO, or at 
 * least use a dedicated SPI.
 * 
 * An alternative to interrupts could be an RTOS task. https://www.youtube.com/watch?v=k_D_Qu0cgu8
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
