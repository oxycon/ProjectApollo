#ifndef CONFIG_H
#define CONFIG_H

#include "WiFiClient.h"

// Lolin32 Pinout https://arduino-projekte.info/wp-content/uploads/2017/07/lolin32_pinout.png

/* ============================================== *\
 * Debug
\* ============================================== */

#define SERIAL_DEBUG 1
#define SERIAL_SPEED 115200

#define COMMAND_BUFFER_SIZE 64

#if SERIAL_DEBUG < 1
#define DEBUG_println(...) 
#define DEBUG_print(...) 
#define DEBUG_printf(...) 
#else
#define DEBUG_println(...) if (debugStream) {debugStream->println(__VA_ARGS__);}
#define DEBUG_print(...) if (debugStream) {debugStream->print(__VA_ARGS__);}
#define DEBUG_printf(...) if (debugStream) {debugStream->printf(__VA_ARGS__);}
#endif

#define FS (const char *)F
#define PM (const __FlashStringHelper *)

/* ============================================== *\
 * Defaults
\* ============================================== */

#define TIME_ZONE "PST8PDT,M3.2.0/2:00:00,M11.1.0/2:00:00"

// "time.nist.gov"
#define NTP_SERVER_1 "132.163.96.1"
// "time.windows.com"
#define NTP_SERVER_2 "13.65.245.138"
#define MAX_CONCENTRATOR_CYCLES 16


/* ============================================== *\
 * Data
\* ============================================== */

extern Stream* debugStream;

/* ============================================== *\
 * Functions
\* ============================================== */
bool loadConfig();
void saveConfig();
void setConfigData(const char* field, const char* data);
void buildConfigForm(WiFiClient &client);

struct ConcentratorConfig {
  uint16_t drv8806_count;                        // Number of valve driver chips used
  uint16_t cycle_count;                          // How many different states there are per full cycle
  uint32_t duration_ms[MAX_CONCENTRATOR_CYCLES]; // Timing in milliseconds for each cycle
  uint8_t valve_state[MAX_CONCENTRATOR_CYCLES];  // State of all the valves as bit mask
  uint8_t cycle_valve_mask;                      // Which valves should be set by cycle changes
  uint16_t ambient_sensor_address;               // I2C address of the ambient temperture / humidity / pressure sensor
  uint16_t intake_sensor_address;                // I2C address of the intake temperture / humidity sensor
  uint16_t desiccant_sensor_address;             // I2C address of the desiccant temperture / humidity sensor
  uint16_t output_sensor_address;                // I2C address of the output temperture / humidity sensor
  uint16_t in_pressure_address;                  // I2C address of the intake pressure sensor
  uint16_t out_pressure_address;                 // I2C address of the output pressure sensor
  float mprls_min_pressure;                      // MPRLS pressure sensor low end of pressure range
  float mprls_max_pressure;                      // MPRLS pressure sensor high end of pressure range 
};

struct WifiInfo {
  char ssid[36];
  char password[64];
  uint8_t ip[4];
  uint8_t dns[4];
  uint8_t gateway[4];
  uint8_t subnet[4];
  bool is_disabled;
};

#define CONFIG_MAGIC 0x19710914
struct ConfigData {
  uint32_t magic;
  uint32_t config_size;
  uint16_t display_brightness;
  char time_zone[48];
  float adc_calibration;
  WifiInfo wifi;
  ConcentratorConfig concentrator;
  uint32_t crc;  // Must be at end of struct
};

extern ConfigData config;

#endif // CONFIG_H
