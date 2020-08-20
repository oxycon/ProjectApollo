// SHTC3 I2C Temperature, Humidity Sensor

#include <Arduino.h>

#include "Config.h"
#include "Hardware.h"
#include <Wire.h>
#include "SHTC3.h"

bool Shtc3::begin(uint8_t i2cAddr) {
  address_ = i2cAddr;
  is_found_ = shtc3_.begin();

  if (!is_found_) {
      DEBUG_printf(FS("Could not find SHTC3 humidity sensor 0x%02X.\n"), i2cAddr);
      return false;
  } 
  shtc3_.getEvent(&humidity_event_, &temperature_event_);
  temperature_ = temperature_event_.temperature;
  humidity_ = humidity_event_.relative_humidity;
  DEBUG_printf(FS("Found SHTC3 humidity sensor 0x%02X. %0.1f C  |  %0.1f %%\n"), i2cAddr, temperature_, humidity_ );
  next_read_ms_ = millis();
  return true;
}

void Shtc3::run() {
  if (!is_found_  || millis() < next_read_ms_) { return; }
  next_read_ms_ += delay_ms;
  shtc3_.getEvent(&humidity_event_, &temperature_event_);
  temperature_ = temperature_event_.temperature;
  humidity_ = humidity_event_.relative_humidity;
}


size_t Shtc3::getSensorJson(char* buffer, size_t bSize) {
  return snprintf(buffer, bSize, FS("{\"$\":\"SHTC3\",\"address\":%d,\"capabilities\":[\"temperature\",\"humidity\"]}"), address_);
  return strlen(buffer);
}

size_t Shtc3::getDataJson(char* buffer, size_t bSize) {
  return getDataString(buffer, FS("{\"$\":\"SHTC3\",\"temp\":%.1f,\"humidity\":%.1f}"), bSize);
}


size_t Shtc3::getDataString(char* buffer, const char* fmt, size_t bSize) {
  return snprintf_P(buffer, bSize, fmt, temperature_, humidity_);
}
