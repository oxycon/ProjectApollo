#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

class Sensor {
public:
  Sensor() {};
  virtual bool begin(uint16_t address) { address_ = address; }
  virtual void run() { return; }
  virtual const char* getTypeName() { return FS("unknown"); };
  virtual size_t getSensorJson(char* buffer, size_t bSize=1<<30) { return 0; }
  virtual size_t getDataJson(char* buffer, size_t bSize=1<<30) { return 0; }
  virtual size_t getDataDisplay(char* buffer, size_t bSize=1<<30) { return getDataString(buffer, FS(" %0.1fC | %0.1f%%")); }
  virtual size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30)  { return 0; }
  inline bool isFound() { return is_found_; }
  inline uint16_t getAddress() { return address_; }
  inline float getTemperature() { return temperature_; }
  inline float getHumidity() { return humidity_; }
  inline float getPressure() { return pressure_; }
  virtual float getHash() { return humidity_  * 0.01 + temperature_; }

  size_t delay_ms = 1000;

protected:
  uint8_t address_;
  bool is_found_ = false;
  float temperature_ = -100.0;
  float pressure_ = -100.0;
  float humidity_ = -100.0;
  size_t next_read_ms_ = 0;
};

#endif SENSOR_H
