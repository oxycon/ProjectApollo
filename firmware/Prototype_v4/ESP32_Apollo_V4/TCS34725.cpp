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

#include <Arduino.h>

#include "Config.h"
#include "Hardware.h"
#include "TCS34725.h"


bool Tcs34725::begin(uint8_t i2cAddr) {
  address_ = i2cAddr;
  is_found_ = tcs_.begin();
  if (is_found_) {
    DEBUG_printf(FS("Found TCS34725 color sensor at %02X.\n"), i2cAddr);
  } else {
    DEBUG_println(F("TCS34725 color sensor not found"));
  }
  next_read_ms_ = millis();
  return is_found_;
}

void Tcs34725::run() {
  if (!is_found_ || millis() < next_read_ms_) { return; }
  next_read_ms_ += 1000;
  tcs_.getRawData(&r, &g, &b, &c);

  // DN40 calculations
  ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
  r_comp = r - ir;
  g_comp = g - ir;
  b_comp = b - ir;
  c_comp = c - ir;

  
  uint16_t rgb_max = max(max(r_comp, g_comp), b_comp);
  if (rgb_max < 8 || c_comp < 16) {
    color_r = color_g = color_b = 0;
  } else {
    float norm = 255.0 / (rgb_max > 0 ? rgb_max : 1);
    color_r = norm * r_comp;
    color_g = norm * g_comp;
    color_b = norm * b_comp;
  }

  /*
  cratio = float(ir) / float(c);
  saturation = ((256 - atime) > 63) ? 65535 : 1024 * (256 - atime);
  saturation75 = (atime_ms < 150) ? (saturation - saturation / 4) : saturation;
  isSaturated = (atime_ms < 150 && c > saturation75) ? 1 : 0;
  cpl = (atime_ms * againx) / (TCS34725_GA * TCS34725_DF);
  maxlux = 65535 / (cpl * 3);

  lux = (TCS34725_R_Coef * float(r_comp) + TCS34725_G_Coef * float(g_comp) + TCS34725_B_Coef * float(b_comp)) / cpl;
  ct = TCS34725_CT_Coef * float(b_comp) / float(r_comp) + TCS34725_CT_Offset;
  */
}



size_t Tcs34725::getDataCsv(char* buffer, size_t bSize) {
  return snprintf_P(buffer, bSize, FS("%d R, %d G, %d B, %d r, %d g, %d b, %d c, %d ir"), 
    color_r, color_g, color_b, r_comp, g_comp, b_comp, c_comp, ir);
}

size_t Tcs34725::getSensorJson(char* buffer, size_t bSize) {
  return snprintf(buffer, bSize, FS("{\"$\":\"TCS34725\",\"address\":%d,\"capabilities\":[\"color\",\"brightness\"]}"), address_);
}

size_t Tcs34725::getDataJson(char* buffer, size_t bSize) {
  return snprintf_P(buffer, bSize, FS("{\"$\":\"TCS34725\",\"R\":%d,\"G\":%d,\"B\":%d,\"r\":%d,\"g\":%d,\"b\":%d,\"c\":%d,\"ir\":%d}"), 
    color_r, color_g, color_b, r_comp, g_comp, b_comp, c_comp, ir);
}

size_t Tcs34725::getDataString(char* buffer, const char* fmt, size_t bSize) {
  return snprintf_P(buffer, bSize, fmt, color_r, color_g, color_b);
}
