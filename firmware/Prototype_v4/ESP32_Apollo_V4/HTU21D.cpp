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

#include <Arduino.h>

#include "Config.h"
#include "Hardware.h"
#include <Wire.h>
#include "HTU21D.h"

bool Htu21d::begin(uint8_t i2cAddr) {
  address_ = i2cAddr;
  is_found_ = htu21d_.begin();

  if (!is_found_) {
      DEBUG_printf(FS("Could not find HTU21D humidity sensor 0x%02X.\n"), i2cAddr);
      return false;
  } 
  temperature_ = htu21d_.readTemperature();
  humidity_ = htu21d_.readHumidity();
  DEBUG_printf(FS("Found HTU21D humidity sensor 0x%02X. %0.1f C  |  %0.1f %%\n"), i2cAddr, temperature_, humidity_ );
  next_read_ms_ = millis();
  return true;
}

void Htu21d::run() {
  if (!is_found_  || millis() < next_read_ms_) { return; }
  next_read_ms_ += delay_ms;
  temperature_ = htu21d_.readTemperature();
  humidity_ = htu21d_.readHumidity();
}


size_t Htu21d::getSensorJson(char* buffer, size_t bSize) {
  return snprintf(buffer, bSize, FS("{\"$\":\"HTU21D\",\"address\":%d,\"capabilities\":[\"temperature\",\"humidity\"]}"), address_);
  return strlen(buffer);
}

size_t Htu21d::getDataJson(char* buffer, size_t bSize) {
  return getDataString(buffer, FS("{\"$\":\"HTU21D\",\"temp\":%.1f,\"humidity\":%.1f}"), bSize);
}


size_t Htu21d::getDataString(char* buffer, const char* fmt, size_t bSize) {
  return snprintf_P(buffer, bSize, fmt, temperature_, humidity_);
}
