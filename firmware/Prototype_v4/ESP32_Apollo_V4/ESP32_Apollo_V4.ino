#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Wifi.h"
#include "CLI.h"
#include "TcpServer.h"
#include "Display.h"

#include "time.h"
#include "sys/time.h"

static bool configured = false;
TcpServer* tcpServer;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(VALVE_2_WAY_PIN, OUTPUT);
  digitalWrite(VALVE_2_WAY_PIN, LOW);
  pinMode(VALVE_5_WAY_PIN, OUTPUT);
  digitalWrite(VALVE_5_WAY_PIN, LOW);
  pinMode(VALVE_RELIEF_PIN, OUTPUT);
  digitalWrite(VALVE_5_WAY_PIN, LOW);

  Serial.begin(SERIAL_SPEED);
  delay(10);
  
  DEBUG_print(F("\n\nHello World\n\n"));
  configured = loadConfig();

  display_boot_screen();  
  DEBUG_print(F("Screen Done\n"));
  WifiConnect();
  WifiWait();
  getNtpTime();
  display_wifi_screen(); 
  tcpServer = new TcpServer();
  tcpServer->begin();
}

void loop() {
  ReadSerial();
  tcpServer->run();
}
