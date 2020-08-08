// SHTC3 I2C Temperature, Humidity Sensor

#ifndef SHTC3_H
#define SHTC3_H

#include <Adafruit_SHTC3.h>
#include "Sensor.h"


class Shtc3 : public Sensor {
public:
  bool begin(uint8_t i2cAddr = SHTC3_DEFAULT_ADDR);
  void run();
  size_t getSensorJson(char* buffer, size_t bSize=1<<30);
  size_t getDataJson(char* buffer, size_t bSize=1<<30);
  size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30);

protected:
  Adafruit_SHTC3 shtc3_;
  sensors_event_t humidity_event_;
  sensors_event_t temperature_event_;
};

#endif // SHTC3_H
