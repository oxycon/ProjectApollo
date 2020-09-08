#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Concentrator.h"
#include "Valve.h"

/*
 * The Concentrator is based on a state machine.
 * 
 * The current implementation is configurable, non-blocking and driven by a hardware timer iterrupt. 
 * This means that if valves are driven by SPI, the SPI can not be shared with other peripherals.
 * Otherwise there is a risk of race conditions when the main program is in the middle 
 * of talking to peripherals on that SPI. 
 * 
 * An alternative to interrupts could be an RTOS task. https://www.youtube.com/watch?v=k_D_Qu0cgu8
 */

bool concentrator_is_enabled = false;
volatile unsigned int concentrator_stage = 0;

/* create a hardware timer */
hw_timer_t* concentrator_timer = NULL;

void IRAM_ATTR concentratorISR() {
  // digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  set_valves(config.concentrator.valve_state[concentrator_stage], config.concentrator.stage_valve_mask);
  timerAlarmWrite(concentrator_timer, config.concentrator.duration_ms[concentrator_stage] * 10, true);
  // DEBUG_printf(FS("State:%u Valves:%x  Next:%u\n"), concentrator_state, config.concentrator.valve_state[concentrator_stage], next_stage_ms);
  concentrator_stage++;
  if (concentrator_stage >= config.concentrator.stage_count) { concentrator_stage = 0; }
}


void concentrator_start() {
  DEBUG_print(F("Start Concentrator\n"));
  concentrator_stage = 0;

  /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/8000) = 100us so we set divider 8000 and count up */
  concentrator_timer = timerBegin(0, 8000, true);

  /* Attach onTimer function to our timer */
  timerAttachInterrupt(concentrator_timer, &concentratorISR, true);

  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 second is 1000000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(concentrator_timer, config.concentrator.duration_ms[concentrator_stage] * 10, true);

  /* Start an alarm */
  timerAlarmEnable(concentrator_timer);

  concentrator_is_enabled = true;
}

void concentrator_stop() {
  DEBUG_print(F("Stop Concentrator\n"));
  concentrator_is_enabled = false;
  timerAlarmDisable(concentrator_timer);
}

/*
// Loop through the valve stages
// This function must not block or delay
void concentrator_run() {
  if (!concentrator_is_enabled) { return; }
  if (millis() < next_stage_ms) { return; }
  
  set_valves(config.concentrator.valve_state[concentrator_stage], config.concentrator.stage_valve_mask);
  next_stage_ms += config.concentrator.duration_ms[concentrator_stage];
  // DEBUG_printf(FS("State:%u Valves:%x  Next:%u\n"), concentrator_state, config.concentrator.valve_state[concentrator_stage], next_stage_ms);
  concentrator_stage++;
  if (concentrator_stage >= config.concentrator.stage_count) { concentrator_stage = 0; }
}
*/
