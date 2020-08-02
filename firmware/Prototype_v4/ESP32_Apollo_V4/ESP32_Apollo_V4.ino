#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Wifi.h"
#include "CLI.h"
#include "TcpServer.h"
#include "Display.h"
#include "Valve.h"
#include "Concentrator.h"
#include "OxygenSensor.h"

#include "time.h"
#include "sys/time.h"

static bool configured = false;
TcpServer* tcpServer;

const char version_number[] PROGMEM = "0.1";
const char version_date[] PROGMEM = __DATE__;
const char version_time[] PROGMEM = __TIME__;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.begin(SERIAL_SPEED);
  delay(10);
  
  DEBUG_printf(FS("\n\nApollo Oxygen Concentrator Version %s | %s | %s\n\n"), version_number, version_date, version_time);
  configured = loadConfig();
  if (!digitalRead(BUTTON_PIN)) {
    DEBUG_println("Button press detected. Entering congif mode"); 
    configured = false;
  }
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
  o2_sensor_setup();
  concentrator_start();
}

void loop() {
  ReadSerial();
  tcpServer->run();
  concentrator_run();
  o2_sensor_run();
}
