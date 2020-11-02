/*
 *   ESP32 Oxygen Concentrator
 *  ===========================
 * 
 * This software is provided "as is" for educational purposes only. 
 * No claims are made regarding its fitness for medical or any other purposes. 
 * The authors are not liable for any injuries, damages or financial losses.
 * 
 * Use at your own risk!
 * 
 * License: MIT https://github.com/oxycon/ProjectApollo/blob/master/LICENSE.txt
 * For more information see: https://github.com/oxycon/ProjectApollo
 */

#ifndef CONCENTRATOR_H
#define CONCENTRATOR_H

#include <Stream.h>

extern bool concentrator_is_enabled;
extern volatile uint8_t concentrator_stage;
extern volatile uint32_t concentrator_cycle;
extern Stream* stats_stream;

void concentrator_start();
void concentrator_stop();
//void concentrator_run();

typedef struct ConcentratorData {
  uint32_t timestamp;
  uint32_t cycle;
  uint8_t stage;
  uint8_t valves;
  float oxygen;
  float flow;
  float pressure;
  float desiccant_humidity;  
  float desiccant_temperature;  
} ConcentratorData;


typedef struct ConcentratorStats {
  uint32_t sample_count;
  float oxygen_min;
  float oxygen_max;
  float oxygen_acc;
  float flow_min;
  float flow_max;
  float flow_acc;
} ConcentratorStats;

extern ConcentratorStats cycle_stats;
extern ConcentratorStats concentrator_stats;
extern uint32_t stats_period_ms;
extern Stream* concentrator_data_stream;
extern Stream* cycle_stats_stream;
extern Stream* concentrator_stats_stream;

void run_stats();
void reset_stats(ConcentratorStats& stats);
void update_stats(ConcentratorStats& stats);
size_t csv_stats_header(char* buffer, size_t bSize=1<<30);
size_t csv_stats(char* buffer, ConcentratorStats& stats, size_t bSize=1<<30);
size_t csv_concentrator_data_header(char* buffer, size_t bSize=1<<30);
size_t csv_concentrator_data(char* buffer, size_t bSize=1<<30);

void start_calibration(Stream* stream);
void stop_calibration();
void run_calibration();
  
#endif // CONCENTRATOR_H
