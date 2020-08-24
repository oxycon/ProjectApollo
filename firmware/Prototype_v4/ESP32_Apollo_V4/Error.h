// Honeywell MPRLS SPI pressure Sensor

#ifndef ERROR_H
#define ERROR_H

#include "Config.h"

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
  ERR(MAX_ERROR)\

extern uint32_t error_flags;
extern const char *ERROR_STRING[];

typedef enum ErrorType {
    FOREACH_ERROR(GENERATE_ENUM)
} ErrorType;

void setError(ErrorType error, const char* text=nullptr);
void resetError(ErrorType error, const char* text=nullptr);
inline uint32_t getErrorFlags() { return error_flags; }

#endif // ERROR_H
