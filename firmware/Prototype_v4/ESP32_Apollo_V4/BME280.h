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
  const char* getTypeName() { return FS("BME280"); };
  size_t getSensorJson(char* buffer, size_t bSize=1<<30);
  size_t getDataJson(char* buffer, size_t bSize=1<<30);
  size_t getDataDisplay(char* buffer, size_t bSize=1<<30) { return getDataString(buffer, FS(" %.0fhPa | %0.1fC | %0.1f%%")); }
  size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30);
  virtual float getHash() { return humidity_ * 0.0001 + temperature_ * 0.01 + pressure_; }
  virtual bool hasTemperature() { return true; }
  virtual bool hasHumidity() { return true; }
  virtual bool hasPressure() { return true; }

protected:
  Adafruit_BME280 bme_;
};

#endif // BME280_H
