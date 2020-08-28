// HDC2080 I2C Temperature, Humidity Sensor

#include <Arduino.h>

#include "Config.h"
#include "Hardware.h"
#include <Wire.h>
#include "Hdc2080.h"

bool Hdc2080::begin(uint8_t i2cAddr) {
  address_ = i2cAddr;
  hdc2080_.begin(i2cAddr);
  uint16_t mid = hdc2080_.readManufacturerId();
  uint16_t did = hdc2080_.readDeviceId();
  is_found_ = mid == 0x5449 && did == 0x7D0;
  if (!is_found_) {
      DEBUG_printf(FS("Could not find HDC2080 humidity sensor 0x%02X.\n"), i2cAddr);
      return false;
  } 
  
  // Reset Sensor
  hdc2080_.reset();
  delay(200);

  // Configure Measurements
  hdc2080_.setMeasurementMode(TEMP_AND_HUMID);  // Set measurements to temperature and humidity
  hdc2080_.setRate(ONE_HZ);                     // Set measurement frequency to 1 Hz
  hdc2080_.setTempRes(FOURTEEN_BIT);
  hdc2080_.setHumidRes(FOURTEEN_BIT);

  //begin measuring
  hdc2080_.triggerMeasurement();
  delay(200);

  temperature_ = hdc2080_.readTemp();
  humidity_ = hdc2080_.readHumidity();
  
  DEBUG_printf(FS("Found HDC2080 humidity sensor 0x%02X. %0.1f C  |  %0.1f %%\n"), i2cAddr, temperature_, humidity_ );
  next_read_ms_ = millis();
  return true;
}

void Hdc2080::run() {
  if (!is_found_  || millis() < next_read_ms_) { return; }
  next_read_ms_ += delay_ms;
  temperature_ = hdc2080_.readTemp();
  humidity_ = hdc2080_.readHumidity();
}

size_t Hdc2080::getSensorJson(char* buffer, size_t bSize) {
  return snprintf(buffer, bSize, FS("{\"$\":\"HDC2080\",\"address\":%d,\"capabilities\":[\"temperature\",\"humidity\"]}"), address_);
}

size_t Hdc2080::getDataJson(char* buffer, size_t bSize) {
  return getDataString(buffer, FS("{\"$\":\"HDC2080\",\"temp\":%.1f,\"humidity\":%.1f}"), bSize);
}

size_t Hdc2080::getDataString(char* buffer, const char* fmt, size_t bSize) {
  return snprintf_P(buffer, bSize, fmt, temperature_, humidity_);
}
