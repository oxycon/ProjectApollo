// Honeywell MPRLS SPI pressure Sensor

#ifndef MPRLS_H
#define MPRLS_H

#include "Sensor.h"
#include <SPI.h>

#define MPRLS_DEFAULT_ADDR 0x18 

class Mprls : public Sensor {
public:
  Mprls() : spiSettings_(MPRLS_SPI_FREQUENCY, MSBFIRST, SPI_MODE0) { status_=0; };
  bool begin(uint8_t i2cAddr = 0xFF);
  void run();
  void read();
  const char* getTypeName() { return FS("MPRLS"); };
  size_t getSensorJson(char* buffer, size_t bSize=1<<30);
  size_t getDataJson(char* buffer, size_t bSize=1<<30);
  size_t getDataDisplay(char* buffer, size_t bSize=1<<30);
  size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30);
  float getHash() { return pressure_; }
  virtual bool hasPressure() { return true; }

protected:
  SPISettings spiSettings_;
  uint8_t status_ = 0;
  int32_t raw_ = 0;
  float psi_ = -100.0;
};

#endif // MPRLS_H
