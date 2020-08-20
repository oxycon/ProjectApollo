// HDC2080 I2C Temperature, Humidity Sensor
// The Limne Labs library was modified from the original to move the I2C address from constructor to begin().
// New functions were added to read MID and DID.

#ifndef HDC_2080_H
#define HDC_2080_H

#include <HDC2080.h>
#include "Sensor.h"

#define HDC2080_ADDRESS_1 0x40
#define HDC2080_ADDRESS_2 0x41

class Hdc2080 : public Sensor {
public:
  bool begin(uint8_t i2cAddr = HDC2080_ADDRESS_1);
  void run();
  const char* getTypeName() { return FS("HDC2080"); };
  size_t getSensorJson(char* buffer, size_t bSize=1<<30);
  size_t getDataJson(char* buffer, size_t bSize=1<<30);
  size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30);
  virtual bool hasTemperature() { return true; }
  virtual bool hasHumidity() { return true; }
  

protected:
  HDC2080 hdc2080_;
};

#endif // HDC_2080_H
