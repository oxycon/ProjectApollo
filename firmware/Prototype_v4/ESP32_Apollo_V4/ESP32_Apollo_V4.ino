#include "Arduino.h"

#include "hardware.h"
#include "config.h"
#include "Wifi.h"
#include "CLI.h"
#include "TcpServer.h"

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
  WifiConnect();
  WifiWait();
  getNtpTime(); 
  tcpServer = new TcpServer();
  tcpServer->begin();
}

void loop() {
  ReadSerial();
  tcpServer->run();
}
