// Bosch Temperature, Humidity, Pressure (BME280)
// Note: BMP280 has no humidity, BME680 has VOC

#include <Arduino.h>

#include "config.h"
#include "Hardware.h"
#include <Wire.h>
#include "BME280.h"


Bme bme280_1;
Bme bme280_2;

void Bme::setup() {
  bme280_1.begin(BME280_ADDRESS);
  bme280_2.begin(BME280_ADDRESS_ALTERNATE); 
}

bool Bme::begin(uint8_t i2cAddr) {
   bmeStatus = bme.begin(i2cAddr);

  if (!bmeStatus) {
      DEBUG_printf(FS("Could not find BME280 humidity / pressure sensor %02X.\n"), i2cAddr);
      return false;
  } 
  DEBUG_printf(FS("Found BME280 humidity / pressure sensor %02X.\n"), i2cAddr);
  next_read_ms = millis();
  return true;
}

void Bme::run() {
  if (!bmeStatus || millis() < next_read_ms) { return; }
  next_read_ms += delay_ms;
  temperature = bme.readTemperature();
  pressure = bme.readPressure();
  humidity = bme.readHumidity();
}

size_t Bme::getDataJson(char* buffer, size_t bSize) {
  return getDataString(buffer, FS("{\"$\":\"BME280\",\"t\":%s,\"bp\":%s,\"rh\":%s},"), bSize);
}

size_t Bme::getDataString(char* buffer, const char* fmt, size_t bSize) {
  return snprintf_P(buffer, bSize, fmt, temperature, pressure*0.01, humidity);
}
