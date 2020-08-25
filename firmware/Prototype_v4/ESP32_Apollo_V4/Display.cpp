#include "SPI.h"

#include "TFT_eSPI.h"
#include <TJpg_Decoder.h>

#include "FreeFonts.h"
#include "ApolloLogo.h"

#include "Hardware.h"
#include "Config.h" // Warning: Must be included after TFT_eSPI.h to avoid FS() macro conflicts
#include "Error.h"
#include "wifi.h"
#include "Secrets.h"

#include "Valve.h"
#include "Concentrator.h"
#include "OxygenSensor.h"
#include "SensorManager.h"


const static int TFT_L1 = 64;
const static int TFT_LH = 32;
const static int TFT_GRID_COLOR = TFT_LIGHTGREY;

#define TFT_DARKBLUE        0x000F 

TFT_eSPI tft = TFT_eSPI();

uint16_t* tft_buffer;
bool      buffer_loaded = false;
uint16_t  spr_width = 0;

bool has_touch = false;
uint32_t next_display_update_ms = 0;

TFT_eSPI_Button concentrator_control_button;

uint8_t old_valve;
float old_oxygen = -1.0;
float old_o2s_flow = -1.0;
float old_o2s_temp = -1.0;
float old_in_pressure = 0.0;
float old_out_pressure = 0.0;
float old_ambient_sensor = 0.0;
float old_intake_sensor = 0.0;
float old_desiccant_sensor = 0.0;
float old_output_sensor = 0.0;
float old_color_sensor = 0.0;

static const float MAX_O2 = 100.0;
static const float MAX_O2_FLOW = 20.0;
static const int BAR_SIZE_X = TFT_WIDTH - 8;
static const int BAR_SIZE_Y = 20;

#define BAR_BACK_COLOR 0x4040

void draw_bar_border(int x, int y, int w, int h);
void draw_bar(int x, int y, int w, int h, float max_value, float value, float old, uint16_t color);

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

void calibrate_touch() {
  bool touch_configured = false;
  for (size_t n=0; n<(sizeof(config.touch_calibartion_data)/sizeof(uint16_t)); n++) {
    if (config.touch_calibartion_data[n] != 0) {
      touch_configured = true;
      break;
    }
  }
  if (touch_configured) {
      tft.setTouch(config.touch_calibartion_data);
      has_touch = true;
  } else {
    // data not valid. recalibrate
    DEBUG_println(F("Calibrating touch"));
    tft.setTextDatum(TC_DATUM);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString(F("Touch Calibration"), TFT_WIDTH/2, 100, 4); // Font 4 for fast drawing with background
    tft.drawString(F("Touch red corners with white arrow."), TFT_WIDTH/2, 140, 2); // Font 4 for fast drawing with background
    tft.calibrateTouch(config.touch_calibartion_data, TFT_WHITE, TFT_RED, 15);
    for (size_t n=0; n<(sizeof(config.touch_calibartion_data)/sizeof(uint16_t)); n++) {
      // DEBUG_printf(FS("%d "), config.touch_calibartion_data[n]);      
      if (config.touch_calibartion_data[n] > 1) {
        touch_configured = true;
        DEBUG_println(F("Calibration valid"));
        saveConfig();
        has_touch = true;
        break;
      }
    }
  }
  if (!touch_configured) {
    // This will only work if touch was never calibrated.
    DEBUG_println(F("Touch controller not found"));
    setError(TOUCH_CONTROLLER_NOT_FOUND);
  }
}

size_t getTouchCalibrationJson(char* buffer, size_t bSize) {
  return snprintf_P(buffer, bSize, FS("[%d, %d, %d, %d, %d, %d]"), 
    config.touch_calibartion_data[0], config.touch_calibartion_data[1],
    config.touch_calibartion_data[2], config.touch_calibartion_data[3],
    config.touch_calibartion_data[4], config.touch_calibartion_data[5]);
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
  tft.begin();
  tft.setRotation(0);
  calibrate_touch();
}

void set_display_brightness(uint16_t value) {
  if (value > 100) return;
  config.display_brightness = value;
  if (LCD_LED_PIN > -1) {
    ledcWrite(LCD_LED_PWM_CHANNEL, pow(1023.0, (float)value / 100.0));
  }
}

void display_boot_screen() {
  tft.fillScreen(TFT_BLACK);

    // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The jpeg decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  // Draw the logo
  TJpgDec.drawJpg((TFT_WIDTH-240)/2, 40, apollo_logo, sizeof(apollo_logo));

  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString(F("ESP-32 Apollo"), TFT_WIDTH/2, 2, 4); // Font 4 for fast drawing with background

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString(F("Oxygen Concentrator"), TFT_WIDTH/2, TFT_HEIGHT, 4); // Font 4 for fast drawing with background  
}

void display_config_screen() {
  char buffer[64];
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
  char buffer[64];
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
  char buffer[64];
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  IPAddress ip = getLocalIp();
  sprintf_P(buffer, FS("IP: %d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);
  tft.drawString(buffer, 0, 1, 1);

  tft.drawLine(0, 12, TFT_WIDTH-1, 12, TFT_LIGHTGREY );

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(FS("Cycle:"), 0, 18, 4);
  tft.drawString(FS("Oxygen:"), 0, 48, 4);
  tft.drawString(FS("Flow:"), 0, 108, 4);
  

  tft.drawString(FS("In Pressure:"), 0, 180, 2);
  tft.drawString(FS("Out Pressure:"), 0, 200, 2);
  tft.drawString(FS("Oxygen:"), 0, 220, 2);
  tft.drawString(FS("Intake:"), 0, 240, 2);
  tft.drawString(FS("Desiccant:"), 0, 260, 2);
  tft.drawString(FS("Output:"), 0, 280, 2);
  tft.drawString(FS("Ambient:"), 0, 300, 2);
  tft.drawString(FS("Color:"), 0, 320, 2);

  old_valve = ~current_valve_states;

  draw_bar_border(0, 78, TFT_WIDTH, 24);
  draw_bar_border(0, 138, TFT_WIDTH, 24);

  if (has_touch) {
    // tft.setFreeFont(&FreeSansBold12pt7b);
    // x, y, w, h, outline, fill, text
    concentrator_control_button.initButton(&tft, TFT_WIDTH/2, 380, 100, 40, 
                        TFT_WHITE, TFT_RED, TFT_BLACK,
                        FS("Stop"), 2);
    concentrator_control_button.drawButton();
  }
  
  next_display_update_ms = millis();
}

void run_touch() {
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Pressed will be set true is there is a valid touch on the screen
  boolean pressed = tft.getTouch(&t_x, &t_y);

  // / Check if any key coordinate boxes contain the touch coordinates
  if (pressed && concentrator_control_button.contains(t_x, t_y)) {
    concentrator_control_button.press(true);  // tell the button it is pressed
  } else {
    concentrator_control_button.press(false);  // tell the button it is NOT pressed
  }
  
  if (concentrator_control_button.justReleased()) { 
    if (concentrator_is_enabled) {
      concentrator_control_button.initButton(&tft, TFT_WIDTH/2, 380, 100, 40, 
                          TFT_WHITE, TFT_RED, TFT_BLACK,
                          FS("Stop"), 2);
    } else {
      concentrator_control_button.initButton(&tft, TFT_WIDTH/2, 380, 100, 40, 
                      TFT_WHITE, TFT_GREEN, TFT_BLACK,
                      FS("Start"), 2);
    }
    concentrator_control_button.drawButton(); // draw normal
  }     
  if (concentrator_control_button.justPressed()) {
    concentrator_control_button.drawButton(true);  // draw invert
    if (concentrator_is_enabled) {
        concentrator_stop();
    } else {
      concentrator_start();  
    }
  }
}

void display_main_screen_update() {
  if (millis() < next_display_update_ms) { return; }
  next_display_update_ms += 100;
  if (has_touch) run_touch();

  char buffer[64];
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  size_t n = getTimeStr(buffer, config.date_format);
  buffer[n++] = ' '; buffer[n++] = '|'; buffer[n++] = ' ';
  getTimeStr(buffer+n, config.time_format);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(buffer, TFT_WIDTH-1, 1, 1);

  tft.setTextDatum(TR_DATUM);

  tft.setTextColor(TFT_RED, TFT_BLACK);
  sprintf(buffer, FS(" %d"), concentrator_cycle);
  tft.drawString(buffer, TFT_WIDTH-1, 18, 4);
  for (size_t i=0; i<8; i++) {
    if (( (current_valve_states ^ old_valve) >> i) & 1)
    tft.fillRoundRect(200 - i * 16, 22, 14, 14, 4, (current_valve_states >> i) & 1 ? TFT_GREEN : TFT_DARKGREY );
  }
  old_valve = current_valve_states;

  if (old_oxygen != o2s_concentration) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    sprintf(buffer, FS("   %0.1f %%"), o2s_concentration);
    tft.drawString(buffer, TFT_WIDTH-1, 48, 4);
    draw_bar(0, 78, TFT_WIDTH, 24, MAX_O2, o2s_concentration, old_oxygen, TFT_GREEN);
    old_oxygen = o2s_concentration;
  }

  if (old_o2s_flow != o2s_flow) {
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    sprintf(buffer, FS("   %0.1f l/m"), o2s_flow);
    tft.drawString(buffer, TFT_WIDTH-1, 108, 4);
    draw_bar(0, 138, TFT_WIDTH, 24, MAX_O2_FLOW, o2s_flow, old_o2s_flow, TFT_BLUE);
    old_o2s_flow = o2s_flow;
  }


  if (in_pressure_sensor) {
    float ftmp = in_pressure_sensor->getHash();
    if (ftmp != old_in_pressure) {
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      in_pressure_sensor->getDataDisplay(buffer);
      tft.drawString(buffer, TFT_WIDTH-1, 180, 2);
      old_in_pressure = ftmp;
    }
  }

  if (out_pressure_sensor) {
    float ftmp = out_pressure_sensor->getHash();
    if (ftmp != old_out_pressure) {
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      out_pressure_sensor->getDataDisplay(buffer);
      tft.drawString(buffer, TFT_WIDTH-1, 200, 2);
      old_out_pressure = ftmp;
    }
  }

  if (o2s_temperature != old_o2s_temp) {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    sprintf(buffer, FS(" %0.1f C"), o2s_temperature);
    tft.drawString(buffer, TFT_WIDTH-1, 220, 2);
    old_o2s_temp = o2s_temperature;
  }

  if (intake_sensor) {
    float ftmp = intake_sensor->getHash();
    if (ftmp != old_intake_sensor) {
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      intake_sensor->getDataDisplay(buffer);
      tft.drawString(buffer, TFT_WIDTH-1, 240, 2);
      old_intake_sensor = ftmp;
    }
  }

  if (desiccant_sensor) {
    float ftmp = desiccant_sensor->getHash();
    if (ftmp != old_desiccant_sensor) {
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      desiccant_sensor->getDataDisplay(buffer);
      tft.drawString(buffer, TFT_WIDTH-1, 260, 2);
      old_desiccant_sensor = ftmp;
    }
  }

  if (output_sensor) {
    float ftmp = output_sensor->getHash();
    if (ftmp != old_output_sensor) {
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      output_sensor->getDataDisplay(buffer);
      tft.drawString(buffer, TFT_WIDTH-1, 280, 2);
      old_output_sensor = ftmp;
    }
  }

  if (ambient_sensor) {
    float ftmp = ambient_sensor->getHash();
    if (ftmp != old_ambient_sensor) {
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      ambient_sensor->getDataDisplay(buffer);
      tft.drawString(buffer, TFT_WIDTH-1, 300, 2);
      old_ambient_sensor = ftmp;
    }
  }

  if (color_sensor) {
    float ftmp = color_sensor->getHash();
    if (ftmp != old_color_sensor) {
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      color_sensor->getDataDisplay(buffer);
      tft.drawString(buffer, TFT_WIDTH-1, 320, 2);
      tft.fillRect(60, 320, 30, 18, tft.color565(color_sensor->color_r, color_sensor->color_g, color_sensor->color_b));
      tft.fillRect(100, 320, 30, 18, tft.color565(color_sensor->r_comp>>3, color_sensor->g_comp>>3, color_sensor->g_comp>>3));
      old_color_sensor = ftmp;
    }
  }
}


void draw_bar_border(int x, int y, int w, int h) {
  w--; h--;
  tft.drawLine(x+1, y+1, x+w-1, y+1, TFT_WHITE );
  tft.drawLine(x+1, y+1, x+1, y+h-1, TFT_WHITE );
  tft.drawLine(x+1, y+h, x+w, y+h, TFT_WHITE );
  tft.drawLine(x+w, y+1, x+w, y+h, TFT_WHITE );

  tft.drawLine(x, y, x+w, y, TFT_LIGHTGREY );
  tft.drawLine(x, y, x, y+h, TFT_LIGHTGREY );
  tft.drawLine(x+2, y+h-1, x+w-1, y+h-1, TFT_LIGHTGREY );
  tft.drawLine(x+w-1, y+1, x+w-1, y+h-1, TFT_LIGHTGREY );

  tft.fillRect(x+2, y+2, w-3, h-3, TFT_BLACK);
  tft.fillRect(x+4, y+4, w-7, h-7, TFT_DARKGREY);
}

void draw_bar(int x, int y, int w, int h, float max_value, float value, float old, uint16_t color) {
  if ( value == old ) { return; }
  int bar_size = (float)(w-8) / max_value * value;
  w--; h --;
  tft.fillRect(x+4, y+4, bar_size, h-7, color);
  int black_size = (float)(w-8) - bar_size;
  if (black_size > 0) {
    tft.fillRect(x+5+bar_size, y+4, black_size, h-7, BAR_BACK_COLOR);    
  }
}
