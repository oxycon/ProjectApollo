#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Concentrator.h"
#include "OxygenSensor.h"
#include "SensorManager.h"
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
volatile uint8_t concentrator_stage = 0;
volatile uint32_t concentrator_cycle = 0;

volatile bool new_cycle = false;
ConcentratorStats cycle_stats;
ConcentratorStats concentrator_stats;
uint32_t next_stats_ms_ = 0; 
uint32_t stats_period_ms = 500;
Stream* concentrator_data_stream = nullptr;
Stream* cycle_stats_stream = nullptr;
Stream* concentrator_stats_stream = nullptr;

/* create a hardware timer */
hw_timer_t* concentrator_timer = NULL;

void IRAM_ATTR concentratorISR() {
  // digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  set_valves(config.concentrator.valve_state[concentrator_stage], config.concentrator.stage_valve_mask);
  timerAlarmWrite(concentrator_timer, config.concentrator.duration_ms[concentrator_stage] * 10, true);
  // DEBUG_printf(FS("State:%u Valves:%x  Next:%u\n"), concentrator_state, config.concentrator.valve_state[concentrator_stage], next_stage_ms);
  concentrator_stage++;
  if (concentrator_stage >= config.concentrator.stage_count) { 
    concentrator_stage = 0; 
    concentrator_cycle++;
    new_cycle = true;
  }
}


void concentrator_start() {
  DEBUG_print(F("Start Concentrator\n"));
  concentrator_stage = 0;
  concentrator_cycle = 0;
  reset_stats(cycle_stats);
  reset_stats(concentrator_stats);
  new_cycle = false;
  next_stats_ms_ = millis() + stats_period_ms;

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

void run_stats() {
  if (!concentrator_is_enabled || millis() < next_stats_ms_) { return; }
  next_stats_ms_ += stats_period_ms; 
  if (new_cycle) {
    if (cycle_stats_stream) {
      char buffer[128];
      size_t n = csv_stats(buffer, cycle_stats, sizeof(buffer)-1);
      cycle_stats_stream->println(buffer);
    }
    if (concentrator_stats_stream) {
      char buffer[128];
      size_t n = csv_stats(buffer, concentrator_stats, sizeof(buffer)-1);
      concentrator_stats_stream->println(buffer);
    }
    reset_stats(cycle_stats);
    new_cycle = false;
  }
  if (concentrator_data_stream) {
    char buffer[128];
    size_t n = csv_concentrator_data(buffer, sizeof(buffer)-1);
    concentrator_data_stream->println(buffer);
  }  
  update_stats(cycle_stats);
  update_stats(concentrator_stats);
}

void reset_stats(ConcentratorStats& stats) {
  stats.sample_count = 0;
  stats.oxygen_min = 999.9;
  stats.oxygen_max = 0.0;
  stats.oxygen_acc = 0.0;
  stats.flow_min = 999.9;
  stats.flow_max = 0.0;
  stats.flow_acc = 0.0; 
}

void update_stats(ConcentratorStats& stats) {
  if (o2s_concentration < stats.oxygen_min) { stats.oxygen_min = o2s_concentration; }
  if (o2s_concentration > stats.oxygen_max) { stats.oxygen_max = o2s_concentration; }
  stats.oxygen_acc += o2s_concentration;
  if (o2s_flow < stats.flow_min) { stats.flow_min = o2s_flow; }
  if (o2s_flow > stats.flow_max) { stats.flow_max = o2s_flow; }
  stats.flow_acc += o2s_flow;
  stats.sample_count++;
}

size_t csv_concentrator_data_header(char* buffer, size_t bSize) {
  return snprintf_P(buffer, bSize, FS("Cycle, Stage, Valves, Oxygen, Flow, Pressure, DesRH, DesTemp, Millis"));
}

size_t csv_concentrator_data(char* buffer, size_t bSize) {
  return snprintf_P(buffer, bSize, FS("%u, %u, 0x%02X, %0.1f, %0.1f, %0.1f, %0.1f, %0.1f, %u"),
    concentrator_cycle, concentrator_stage, current_valve_states,
    o2s_concentration, o2s_flow, 
    out_pressure_sensor ? out_pressure_sensor->getPressure() : 0.0,  
    ambient_sensor ? ambient_sensor->getHumidity() : 0.0,
    ambient_sensor ? ambient_sensor->getTemperature() : 0.0,
    millis());
}

size_t csv_stats_header(char* buffer, size_t bSize) {
  return snprintf_P(buffer, bSize, FS("Cycle, OxyMin, OxyMax, OxyAvg, FlowMin, FlowMax, FlowMin, FlowAvg, Samples, Millis"));
}

size_t csv_stats(char* buffer, ConcentratorStats& stats, size_t bSize) {
  return snprintf_P(buffer, bSize, FS("%u, %0.1f, %0.1f, %0.1f, %0.1f, %0.1f, %0.1f, %u, %u"),
    concentrator_cycle,
    stats.oxygen_min, stats.oxygen_max, stats.oxygen_acc / stats.sample_count,
    stats.flow_min, stats.flow_max, stats.flow_acc / stats.sample_count,
    stats.sample_count, millis());
}
