// Honeywell MPRLS SPI pressure Sensor

#ifndef ERROR_H
#define ERROR_H

#include "Config.h"
#include <time.h>

/* ============================================== *\
 * Errors
\* ============================================== */

#define FOREACH_ERROR(ERR) \
  ERR(OXYGEN_SENSOR_NOT_FOUND)\
  ERR(AMBIENT_HUMIDITY_SENSOR_NOT_FOUND)\
  ERR(INTAKE_HUMIDITY_SENSOR_NOT_FOUND)\
  ERR(DESICCANT_HUMIDITY_SENSOR_NOT_FOUND)\
  ERR(OUTPUT_HUMIDITY_SENSOR_NOT_FOUND)\
  ERR(IN_PRESSURE_SENSOR_NOT_FOUND)\
  ERR(OUT_PRESSURE_SENSOR_NOT_FOUND)\
  ERR(COLOR_SENSOR_NOT_FOUND)\
  ERR(EEPROM_NOT_FOUND)\
  ERR(WIFI_NOT_FOUND)\
  ERR(TOUCH_CONTROLLER_NOT_FOUND)\
  ERR(VALVE_FAULT)\
  ERR(PRESSURE_SENSOR_FAULT)\
  ERR(OXYGEN_SENSOR_LOST)\
  ERR(MAX_ERROR)\

extern uint32_t error_flags;
extern uint32_t error_count;

extern const char *ERROR_STRING[];

typedef enum ErrorType {
    FOREACH_ERROR(GENERATE_ENUM)
} ErrorType;

struct ErrorLog {
  time_t timestamp;
  ErrorType error_id;
  char text[64];
};

#define ERROR_LOG_SIZE 10

void setError(ErrorType error, const char* text=nullptr);
void resetError(ErrorType error, const char* text=nullptr);
size_t get_latest_display_error(size_t log_nr, char* buffer, size_t bSize=1<<30);
size_t get_error_json(size_t log_nr, char* buffer, size_t bSize=1<<30);
size_t get_error_log(size_t log_nr, char* buffer, size_t bSize=1<<30);
inline uint32_t getErrorFlags() { return error_flags; }

#endif // ERROR_H
