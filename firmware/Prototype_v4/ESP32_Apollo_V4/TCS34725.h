#ifndef TCS34725_H
#define TCS34725_H

#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include "Sensor.h"

class Tcs34725 : public Sensor {
public:
  Tcs34725() : tcs_(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X) {};
  bool begin(uint8_t i2cAddr = TCS34725_ADDRESS);
  void run();
  const char* getTypeName() { return FS("TCS34725"); };
  size_t getDataCsv(char* buffer, size_t bSize=1<<30);
  size_t getSensorJson(char* buffer, size_t bSize=1<<30);
  size_t getDataJson(char* buffer, size_t bSize=1<<30);
  size_t getDataDisplay(char* buffer, size_t bSize=1<<30) { return getDataString(buffer, FS(" R:%02X G:%02X B:%02X")); }
  size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30);
  float getHash() { return ((uint32_t) color_r << 16) + ((uint32_t) color_g << 8) + ((uint32_t) color_b); }

  // Red, Green, Blue, Clear
  uint16_t r, g, b, c;
  uint16_t ir;
  uint16_t r_comp, g_comp, b_comp, c_comp; // IR compensated
  uint8_t color_r, color_g, color_b;       // Normalized color to full brightness.
/*
  uint16_t saturation, saturation75;
  float cratio, cpl, ct, lux, maxlux;
*/


protected:
  Adafruit_TCS34725 tcs_;
};



#endif  // TCS34725_H
