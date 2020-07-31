#include "SPI.h"

#include "TFT_eSPI.h"
#include <TJpg_Decoder.h>

#include "FreeFonts.h"
#include "ApolloLogo.h"

#include "Hardware.h"
#include "Config.h" // Warning: Must be included after TFT_eSPI.h to avoid FS() macro conflicts
#include "wifi.h"
#include "Secrets.h"


const static int TFT_L1 = 64;
const static int TFT_LH = 32;
const static int TFT_GRID_COLOR = TFT_LIGHTGREY;

#define TFT_DARKBLUE        0x000F 

TFT_eSPI tft = TFT_eSPI();

char buffer[64];

uint16_t* tft_buffer;
bool      buffer_loaded = false;
uint16_t  spr_width = 0;

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
