// Bosch Temperature, Humidity, Pressure (BMP280), VOC (BME680) Sensors

#ifndef BME280_H
#define BME280_H

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Sensor.h"


class Bme : public Sensor {
public:
  bool begin(uint8_t i2cAddr = BME280_ADDRESS);
  void run();
  size_t getSensorJson(char* buffer, size_t bSize=1<<30);
  size_t getDataJson(char* buffer, size_t bSize=1<<30);
  size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30);

protected:
  Adafruit_BME280 bme_;
};

#endif // BME280_H
