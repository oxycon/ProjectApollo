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

#include "Hardware.h"
#include "Config.h"
#include "Error.h"
#include "Wifi.h"
#include "CLI.h"
#include "TcpServer.h"
#include "Display.h"
#include "Valve.h"
#include "Concentrator.h"
#include "OxygenSensor.h"
#include "PulseOximeterService.h"

#include "SensorManager.h"

#include "time.h"
#include "sys/time.h"

static bool configured = false;
TcpServer* tcpServer;

const char version_number[] PROGMEM = "0.1";
const char version_date[] PROGMEM = __DATE__;
const char version_time[] PROGMEM = __TIME__;

uint8_t old_valve_alarms = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(MPRLS_CS_PIN, OUTPUT);
  digitalWrite(MPRLS_CS_PIN, HIGH); // Chip select high (inactive)

  
  Serial.begin(SERIAL_SPEED);
  delay(10);
  
  DEBUG_printf(FS("\n\nApollo Oxygen Concentrator Version %s | %s | %s\n\n"), version_number, version_date, version_time);
  configured = loadConfig();
  if (!digitalRead(BUTTON_PIN)) {
    DEBUG_println("Button press detected. Entering congif mode"); 
    configured = false;
  }
  display_setup();
  display_boot_screen();  
  DEBUG_print(F("Screen Done\n"));
  valve_setup();
  if (config.wifi.is_disabled) {
    DEBUG_print(F("WIFI is disabled\n"));  
  } else {
    WifiConnect();
    WifiWait();
    getNtpTime();
    display_wifi_screen(); 
    tcpServer = new TcpServer();
    tcpServer->begin();
  }
  PulseOximeterService::Instance().Start();
  o2_sensor_setup();
  sensor_setup();

  concentrator_start();
  set_display_brightness(config.display_brightness);
  display_main_screen_start();
}

void loop() {
  ReadSerial();
  tcpServer->run();
  PulseOximeterService::Instance().Tick();
  o2_sensor_run();
  sensor_run();
  run_stats();
  display_main_screen_update();
  if (valve_alarms != old_valve_alarms) {
    if (valve_alarms) {
      char buffer[32];
      sprintf(buffer, FS("%02X"), valve_alarms);
      setError(VALVE_FAULT, buffer);
    } else {
      resetError(VALVE_FAULT);
    }
  }
  delay(1);
}
