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

// HTU21D I2C Temperature, Humidity Sensor

#ifndef HTU21D_H
#define HTU21D_H

#include <Adafruit_HTU21DF.h>
#include "Sensor.h"


class Htu21d : public Sensor {
public:
  bool begin(uint8_t i2cAddr = HTU21DF_I2CADDR);
  void run();
  const char* getTypeName() { return FS("HTU21D"); };
  size_t getSensorJson(char* buffer, size_t bSize=1<<30);
  size_t getDataJson(char* buffer, size_t bSize=1<<30);
  size_t getDataString(char* buffer, const char* fmt, size_t bSize=1<<30);
  virtual bool hasTemperature() { return true; }
  virtual bool hasHumidity() { return true; }

protected:
  Adafruit_HTU21DF htu21d_;
};

#endif // HTU21D_H
