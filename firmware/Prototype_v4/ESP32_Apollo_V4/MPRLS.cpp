// Honeywell MPRLS SPI pressure Sensor

#include <Arduino.h>

#include "Config.h"
#include "Hardware.h"
#include "MPRLS.h"

#define DEVICE_POWERED 0x40
#define DEVICE_BUSY 0x20
#define INTEGRITY_FAIL 0x04
#define MATH_SATURATION 0x01

#define CMD_READ 0xAA
#define CMD_NOP 0xF0

#define PSI_TO_HPA 68.947572932

extern SPIClass spi; // SPI used for the LCD
static const uint8_t dummy_data[] PROGMEM = {0,0,0,0};

bool Mprls::begin(uint8_t i2cAddr) {
  address_ = i2cAddr;
  pinMode(MPRLS_CS_PIN, OUTPUT);
  digitalWrite(MPRLS_CS_PIN, HIGH); // Chip select high (inactive)
  delay(20);
  read();
  return is_found_;
}

void Mprls::run() {
  if (!is_found_  || millis() < next_read_ms_) { return; }
  next_read_ms_ += 100;
  read();
}

void Mprls::read() {
  spi.beginTransaction(spiSettings_);
  digitalWrite(MPRLS_CS_PIN, LOW);
  delay(1);
  uint32_t tmp = spi.transfer32(CMD_READ<<24);
  status_ = tmp >> 24;
  raw_ = tmp & 0xFFFFFF;
  if (tmp == 0 || tmp == 0xFFFFFFFF || status_ & DEVICE_POWERED == 0) {
    DEBUG_println(F("MPRLS pressure sensor not found\n"));
    is_found_ = false;
    spi.endTransaction();  
    return;
  }
  if (!is_found_) {
    DEBUG_println(F("Found MPRLS pressure sensor\n"));
    is_found_ = true;
  }
  digitalWrite(MPRLS_CS_PIN, HIGH);
  delay(1);
  spi.endTransaction();  

  /*
  if (status_ & (INTEGRITY_FAIL | MATH_SATURATION)) {
    DEBUG_printf(FS("MPRLS error: %02X\n"), status_);
    // return;
  }
  */

  // Calculate the PSI and convert to hPA
  // use the 10% - 90% calibration curve
  psi_ = (raw_ - 0x19999A) * (config.concentrator.mprls_max_pressure - config.concentrator.mprls_min_pressure);
  psi_ /= (float)(0xE66666 - 0x19999A);
  psi_ += config.concentrator.mprls_min_pressure;
  // convert PSI to hPA
  pressure_ = psi_ * PSI_TO_HPA;
  // DEBUG_printf(FS("MPRLS : %02X %0.1f   %0.1f  %d\n"), status_, psi_, pressure_, raw_);

}

size_t Mprls::getSensorJson(char* buffer, size_t bSize) {
  return snprintf(buffer, bSize, FS("{\"$\":\"MPRLS\",\"address\":%d,\"capabilities\":[\"pressure\"],\"status\":%d}"), address_, status_);
}

size_t Mprls::getDataJson(char* buffer, size_t bSize) {
  return snprintf_P(buffer, bSize, FS("{\"$\":\"MPRLS\",\"pressure\":%.1f,\"status\":%d}"), pressure_, status_);
}

size_t Mprls::getDataDisplay(char* buffer, size_t bSize) { 
  return snprintf_P(buffer, bSize, "   %0.1fpsi | %0.1fhPa", psi_, pressure_);
}

size_t Mprls::getDataString(char* buffer, const char* fmt, size_t bSize) {
  return snprintf_P(buffer, bSize, fmt, pressure_);
}
