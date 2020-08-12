#include <Arduino.h>

#include "Hardware.h"
#include "Config.h"
#include "OxygenSensor.h"

#include <gasboard7500E.h>

// HardwareSerial Serial1(1);


#ifdef O2SENSE_NEED_METADATA
bool has_sernum = false;
bool has_vernum = false;
#endif

const bool o2s_log_enabled = false;

float o2s_concentration = 0.0;
float o2s_flow = 0.0;
float o2s_temperature = 0.0;

void o2_sensor_setup() {
  #ifdef O2SENSE_NEED_METADATA
  const uint8_t cmd_vernum[] = {O2SENSE_CMD_VERSIONNUMBER};
  const uint8_t cmd_sernum[] = {O2SENSE_CMD_SERIALNUMBER};
  #endif

  o2sens_init();
  Serial1.begin(O2SENSE_BAUD_RATE, SERIAL_8N1, O2_RXD_PIN, O2_TXD_PIN);
  DEBUG_println(F("Oxygen Sensor initialized")); 
}

// This function must not block or delay
void o2_sensor_run() {
  while (!o2sens_hasNewData()) { // Loop until a complete packet has been processed
    if (!Serial1.available()) { return; }   // Yield if there is no more data available
    o2sens_feedUartByte(Serial1.read()); // give byte to the parser     
  }
  o2sens_clearNewData(); // clear the new packet flag

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

    if (has_sernum == false)
    {
      uint8_t* data = o2sens_getSerialNumber();
      if (data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0 || data[4] != 0)
      {
        has_sernum = true;
        DEBUG_printf(FS("Serial Number: %02X %02X %02X %02X %02X")
          , data[0]
          , data[1]
          , data[2]
          , data[3]
          , data[4]
          );
      }
    }
    if (has_vernum == false)
    {
      char* data = o2sens_getVersionNumber();
      if (data[0] != 0) // first byte 
      {
        has_vernum = true;
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
