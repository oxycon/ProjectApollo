#include "SPI.h"

#include "TFT_eSPI.h"
#include <TJpg_Decoder.h>

#include "FreeFonts.h"
#include "ApolloLogo.h"

#include "Hardware.h"
#include "Config.h" // Warning: Must be included after TFT_eSPI.h to avoid FS() macro conflicts
#include "wifi.h"
#include "Secrets.h"

#include "Valve.h"
#include "Concentrator.h"
#include "OxygenSensor.h"

#include "BME280.h"
#include "Shtc3.h"

const static int TFT_L1 = 64;
const static int TFT_LH = 32;
const static int TFT_GRID_COLOR = TFT_LIGHTGREY;

#define TFT_DARKBLUE        0x000F 

TFT_eSPI tft = TFT_eSPI();

char buffer[64];

uint16_t* tft_buffer;
bool      buffer_loaded = false;
uint16_t  spr_width = 0;

uint32_t next_display_update_ms = 0;

uint8_t old_valve;
float old_bme280;

extern Bme bme280_2;
extern Shtc3 shtc3;

// =======================================================================================
// This function will be called during decoding of the jpeg file
// =======================================================================================
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}

void display_setup() {
  if (LCD_LED_PIN > -1) {
    pinMode(LCD_LED_PIN, OUTPUT);
    digitalWrite(LCD_LED_PIN, HIGH);
    ledcSetup(LCD_LED_PWM_CHANNEL, 2000, 10);
    ledcAttachPin(LCD_LED_PIN, LCD_LED_PWM_CHANNEL);
    // At first set brightness to 100% to show display is working
    ledcWrite(LCD_LED_PWM_CHANNEL, 1023); 
  }
}

void set_display_brightness(uint16_t value) {
  if (value > 100) return;
  config.display_brightness = value;
  if (LCD_LED_PIN > -1) {
    ledcWrite(LCD_LED_PWM_CHANNEL, pow(1023.0, (float)value / 100.0));
  }
}

void display_boot_screen() {
  tft.begin();
  tft.setRotation(0);

  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLUE);
  tft.drawString(F("ESP-32 Apollo"), 120, 2, 4); // Font 4 for fast drawing with background

  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The jpeg decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  // Draw the logo
  TJpgDec.drawJpg(0, 0, apollo_logo, sizeof(apollo_logo));

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString(F("Oxygen Concentrator"), 120, 290, 4); // Font 4 for fast drawing with background  
}

void display_config_screen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLUE);
  tft.fillRect(0, 0, 240, 30, TFT_BLUE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(F("Configure"), 120, 2, 4); // Font 4 for fast drawing with background
  tft.setFreeFont(FM12);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 80);    // Set cursor near top left corner of screen
  tft.println(F("Access Point:"));
  tft.println(apName);
  tft.println(FS("\nPW: " WIFI_ACCESS_POINT_PASSWORD));
  tft.setFreeFont(FM9);
  tft.println(FS("\nhttp://192.168.4.1"));
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  // tft.drawString(config.module.module_name, 120, 250, 4);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString(getWifiMac(), 120, 290, 4);
}

void display_wifi_screen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLUE);
  tft.fillRect(0, 0, 240, 30, TFT_BLUE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(F("Initializing"), 120, 2, 4); // Font 4 for fast drawing with background
  IPAddress ip = getLocalIp();
  sprintf_P(buffer, FS("%d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString(buffer, 120, 100, 4);
  getTimeStr(buffer, FS("%H:%M:%S   %d.%m.%Y"));
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(buffer, 120, 160, 4);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  // tft.drawString(config.module.module_name, 120, 250, 4);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString(getWifiMac(), 120, 290, 4);
}

void display_main_screen_start() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  IPAddress ip = getLocalIp();
  sprintf_P(buffer, FS("IP: %d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);
  tft.drawString(buffer, 0, 1, 1);

  tft.drawLine(0, 12, 240, 12, TFT_LIGHTGREY );

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(FS("Oxygen:"), 0, 18, 4);
  tft.drawString(FS("Flow:"), 0, 48, 4);
  tft.drawString(FS("Temp:"), 0, 78, 4);
  
  tft.drawString(FS("Cycle:"), 0, 108, 4);
  tft.drawString(FS("BME280:"), 0, 300, 2);

  
  old_valve = ~current_valve_states;
  old_bme280 = bme280_2.getTemperature() + bme280_2.getPressure() + bme280_2.getHumidity();
  
  next_display_update_ms = millis();
}

void display_main_screen_update() {
  if (millis() < next_display_update_ms) { return; }
  next_display_update_ms += 100;

  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TR_DATUM);
  getTimeStr(buffer, FS("%d.%m.%y | %H:%M:%S"));
  tft.drawString(buffer, 240, 1, 1);

  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  getTimeStr(buffer, FS("%d.%m.%y | %H:%M:%S"));
  tft.drawString(buffer, 240, 2, 1);

  tft.setTextDatum(TR_DATUM);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  sprintf(buffer, FS("%0.1f %%"), o2s_concentration);
  tft.drawString(buffer, 240, 18, 4);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  sprintf(buffer, FS("%0.1f l/m"), o2s_flow);
  tft.drawString(buffer, 240, 48, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  sprintf(buffer, FS("%0.1f °C"), o2s_temperature);
  tft.drawString(buffer, 240, 78, 4);

  tft.setTextColor(TFT_RED, TFT_BLACK);
  sprintf(buffer, FS("%d"), concentrator_cycle);
  tft.drawString(buffer, 240, 108, 4);

  tft.setTextColor(TFT_RED, TFT_BLACK);
  sprintf(buffer, FS("%d"), concentrator_cycle);
  tft.drawString(buffer, 240, 108, 4);

  for (size_t i=0; i<8; i++) {
    if (( (current_valve_states ^ old_valve) >> i) & 1)
    tft.fillRoundRect(200 - i * 16, 112, 14, 14, 4, (current_valve_states >> i) & 1 ? TFT_GREEN : TFT_DARKGREY );
  }
  old_valve = current_valve_states;

  float ftmp = bme280_2.getTemperature() + bme280_2.getPressure() + bme280_2.getHumidity();
  if (ftmp != old_bme280) {
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    bme280_2.getDataString(buffer, FS(" %0.1fC | %.0fkPa | %0.1f%%"));
    tft.drawString(buffer, 240, 300, 2);
    old_bme280 = ftmp;
  }

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  shtc3.getDataString(buffer, FS(" %0.1fC | %0.1f%%"));
  tft.drawString(buffer, 240, 280, 2);

}
