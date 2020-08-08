#ifndef SENSOR_H
#define SENSOR_H

class Sensor {
public:
  Sensor() {};
  bool begin(uint16_t address) { address_ = address; }
  void run() { return; }
  size_t getSensorJson(char* buffer, size_t bSize=1<<30) { return 0; }
  size_t getDataJson(char* buffer, size_t bSize=1<<30) { return 0; }
  size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30)  { return 0; }
  inline bool isFound() { return is_found_; }
  inline float getTemperature() { return temperature_; }
  inline float getPressure() { return pressure_; }
  inline float getHumidity() { return humidity_; }

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
