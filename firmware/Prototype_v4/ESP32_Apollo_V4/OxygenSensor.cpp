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

#include <Arduino.h>

#include "Hardware.h"
#include "Config.h"
#include "Error.h"
#include "OxygenSensor.h"

#include <gasboard7500E.h>

// HardwareSerial Serial1(1);

static const uint8_t GET_DATA_CMD[] PROGMEM = { O2SENSE_CMD_READ_DATA };
#ifdef O2SENSE_NEED_METADATA
static const uint8_t GET_VERSION_CMD[] PROGMEM = { O2SENSE_CMD_VERSIONNUMBER };
static const uint8_t GET_SERIAL_CMD[] PROGMEM = { O2SENSE_CMD_SERIALNUMBER };
#endif

static const uint32_t O2S_TIMEOUT = 1100;
static const uint32_t O2S_REFRESH_MS = 250;
static const bool o2s_log_enabled = false;

float o2s_concentration = 0.0;
float o2s_flow = 0.0;
float o2s_temperature = 0.0;
bool o2s_auto_data = false;    // Some O2 sensors send data automatically every 500ms
bool o2s_is_found = false;
static uint32_t o2s_last_data_ms = 0;
static uint32_t o2s_next_cmd_ms = 0;


#ifdef O2SENSE_NEED_METADATA
static bool o2s_has_serial = false;
static bool o2s_has_version = false;
#endif

bool o2_sensor_setup() {
  o2sens_init();
  Serial1.begin(O2SENSE_BAUD_RATE, SERIAL_8N1, O2_RXD_PIN, O2_TXD_PIN);
  o2s_last_data_ms = millis();
  while (millis() < o2s_last_data_ms + 1000) {
    if (Serial1.available()) {
      DEBUG_println(F("Oxygen Sensor found (automatic)")); 
      o2s_auto_data = true;
      o2s_is_found = true;
      return o2s_is_found;
    }
  }
  if (!o2s_auto_data) {
    Serial1.write(GET_DATA_CMD, sizeof(GET_DATA_CMD));
    o2s_last_data_ms = millis();
    while (millis() < o2s_last_data_ms + 500) {
      if (Serial1.available()) {
        DEBUG_println(F("Oxygen Sensor found (non-automatic)")); 
        o2s_next_cmd_ms = millis() + O2S_REFRESH_MS;
        o2s_is_found = true;
        return o2s_is_found;
      }
    }
  }
  
  DEBUG_println(F("Oxygen Sensor not found")); 
  setError(OXYGEN_SENSOR_NOT_FOUND);
  return false;
}

// This function must not block or delay
void o2_sensor_run() {
  if (!o2s_auto_data && o2s_next_cmd_ms < millis()){
    o2s_next_cmd_ms += O2S_REFRESH_MS;    
    Serial1.write(GET_DATA_CMD, sizeof(GET_DATA_CMD));
  }
  if (o2s_is_found && !(error_flags & (1<<OXYGEN_SENSOR_LOST)) && millis() > o2s_last_data_ms + O2S_TIMEOUT) {
    setError(OXYGEN_SENSOR_LOST);
  }
  while (!o2sens_hasNewData()) { // Loop until a complete packet has been processed
    if (!Serial1.available()) { return; }   // Yield if there is no more data available
    o2sens_feedUartByte(Serial1.read()); // give byte to the parser     
  }
  o2sens_clearNewData(); // clear the new packet flag
  o2s_last_data_ms = millis();
  if (error_flags & (1<<OXYGEN_SENSOR_LOST)) { resetError(OXYGEN_SENSOR_LOST); } 

  uint8_t* all_data = o2sens_getRawBuffer();

  if (o2s_log_enabled) {
    // display time in milliseconds for logging
    DEBUG_print(millis(), DEC); 
    DEBUG_print(F(": "));

    // print out the whole buffer for debugging
    for (int i = 0; i < all_data[1] + 3; i++) {
      DEBUG_printf(FS("%02X "), all_data[i]);
    }

    // print out the human readable data
    DEBUG_printf(FS(", %u , %u , %u\n")
      , o2sens_getConcentration16()
      , o2sens_getFlowRate16()
      , o2sens_getTemperature16()
      );

    if (o2s_has_serial == false)
    {
      uint8_t* data = o2sens_getSerialNumber();
      if (data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0 || data[4] != 0)
      {
        o2s_has_serial = true;
        DEBUG_printf(FS("Serial Number: %02X %02X %02X %02X %02X")
          , data[0]
          , data[1]
          , data[2]
          , data[3]
          , data[4]
          );
      }
    }
    if (o2s_has_version == false)
    {
      char* data = o2sens_getVersionNumber();
      if (data[0] != 0) // first byte 
      {
        o2s_has_version = true;
        DEBUG_printf(FS("Version Number: %s"), data);
      }
    }
  }

  o2s_concentration = (float)o2sens_getConcentration16()/10;
  o2s_flow = (float)o2sens_getFlowRate16()/10;
  o2s_temperature = o2sens_getTemperature16()/10;

  if (o2s_log_enabled) {
    DEBUG_println(F("- Oxygen Sensor Data:"));
    DEBUG_printf(FS("     O2 Concentration: %.1f%%\n"), o2s_concentration);
    DEBUG_printf(FS("     Flow: %.1f liter/min\n"), o2s_flow);
    DEBUG_printf(FS("     Temperature:  %.1f Celsius"), o2s_temperature);
  }
}

size_t o2_sensor_data2csv(char* buffer, size_t bSize/*=1<<30*/) {
  return snprintf_P(buffer, bSize, FS(\
    "%.1f%%, %.1flpm, %.1fC"), o2s_concentration, o2s_flow, o2s_temperature);
}
