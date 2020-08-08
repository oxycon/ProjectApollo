// Bosch Temperature, Humidity, Pressure (BME280)
// Note: BMP280 has no humidity, BME680 has VOC

#include <Arduino.h>

#include "Config.h"
#include "Hardware.h"
#include <Wire.h>
#include "BME280.h"

bool Bme::begin(uint8_t i2cAddr) {
  address_ = i2cAddr;
  is_found_ = bme_.begin(i2cAddr);

  if (!is_found_) {
      DEBUG_printf(FS("Could not find BME280 humidity / pressure sensor 0x%02X.\n"), i2cAddr);
      return false;
  } 
  temperature_ = bme_.readTemperature();
  pressure_ = bme_.readPressure() * 0.01;
  humidity_ = bme_.readHumidity();
  DEBUG_printf(FS("Found BME280 humidity / pressure sensor 0x%02X.\n"), i2cAddr);
  next_read_ms_ = millis();
  return true;
}

void Bme::run() {
  if (!is_found_  || millis() < next_read_ms_) { return; }
  next_read_ms_ += delay_ms;
  temperature_ = bme_.readTemperature();
  pressure_ = bme_.readPressure() * 0.01;
  humidity_ = bme_.readHumidity();
}

size_t Bme::getSensorJson(char* buffer, size_t bSize) {
  strncpy(buffer, FS("{\"$\":\"BME280\",\"temperature\":\"true\",\"pressure\":\"true\",\"humidity\":\"true\"},"), bSize-1);
  return strlen(buffer);
}

size_t Bme::getDataJson(char* buffer, size_t bSize) {
  return getDataString(buffer, FS("{\"$\":\"BME280\",\"temp\":%s,\"pressure\":%s,\"humidity\":%s},"), bSize);
}

size_t Bme::getDataString(char* buffer, const char* fmt, size_t bSize) {
  return snprintf_P(buffer, bSize, fmt, pressure_, temperature_, humidity_);
}
