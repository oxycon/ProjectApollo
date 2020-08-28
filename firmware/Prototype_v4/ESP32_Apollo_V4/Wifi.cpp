#include "Wifi.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Config.h"
#include "Error.h"
#include "Secrets.h"
#include <time.h>
#include <sys/time.h>

bool gotNtp = false;
char apName[32];
char hostName[32];

void WifiConnect() {
  if (config.wifi.ip[0] || config.wifi.ip[1] || config.wifi.ip[2] || config.wifi.ip[3]) {
    // Static IP details...
    IPAddress ip(config.wifi.ip);
    IPAddress gateway(config.wifi.gateway);
    IPAddress subnet(config.wifi.subnet);
    IPAddress dns(config.wifi.dns);
    WiFi.config(ip, dns, gateway, subnet);
  }
  snprintf_P(hostName, sizeof(hostName), FS("apollo-42"));
  WiFi.setHostname(hostName);
  
  WiFi.mode(WIFI_MODE_STA);
  
  //DO NOT TOUCH 
  // This is here to force the ESP32 to reset the WiFi and initialise correctly. 
  #if 0
  Serial.print(F("WIFI status = ")); 
  Serial.println(WiFi.getMode()); 
  WiFi.disconnect(true); 
  delay(1000); 
  WiFi.mode(WIFI_MODE_STA); 
  delay(1000); 
  Serial.print(F("WIFI status = ")); 
  Serial.println(WiFi.getMode()); 
  #endif 
  // End silly stuff !!!  

  DEBUG_print(F("WIFI: "));
  DEBUG_print(config.wifi.ssid);
  DEBUG_print(F(" PW: "));
  DEBUG_println(config.wifi.password);
  WiFi.begin(config.wifi.ssid, config.wifi.password);   //WiFi connection
  configTime(0, 0, NTP_SERVER_1, NTP_SERVER_2);
  //setenv("TZ", config.time_zone, 1);
  setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);
  tzset();
}

bool WifiWait() {
  if (WiFi.status() == WL_CONNECTED) { return true; }
  DEBUG_print(F("\nMAC: "));
  DEBUG_println(WiFi.macAddress());
  DEBUG_println(F("Waiting for WIFI connection"));
  uint32_t wifiStartTime = millis();
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    //digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
    DEBUG_print(F("."));
    if (millis() - wifiStartTime > 20000) {
      DEBUG_println(F("\nFailed to connect to WiFi!"));
      setError(WIFI_NOT_FOUND);
      // ESP.restart();
      return false;
    }
  }
  DEBUG_print(F("\nConnected!"));
  DEBUG_print(F(" | MAC: "));
  DEBUG_print(WiFi.macAddress());
  DEBUG_print(F(" | IP address: "));
  DEBUG_print(WiFi.localIP());
  DEBUG_print(" | RSS: ");
  DEBUG_print(WiFi.RSSI());
  DEBUG_print(F(" | Time to connect: "));
  DEBUG_println((millis() - wifiStartTime) * 0.001);
  return true;
}

IPAddress getLocalIp() { return WiFi.localIP(); }
String getWifiMac() { return WiFi.macAddress(); }
int getRSSI() { return WiFi.RSSI(); }

void getNtpTime() {
  uint32_t start = millis();
  gotNtp = false;
  while(getEpochTime() < 50000) {
    if ((millis()-start) > 6000) {
      DEBUG_println(F("\nFailed to get NTP time."));
      break;
    }
    delay(1);
  }
  gotNtp = true;
  char iso[32];
  getIsoTime(iso);
  DEBUG_print(F("NTP: "));
  DEBUG_print(gotNtp);
  DEBUG_print(F(" Time: "));
  DEBUG_print(iso);
  DEBUG_print(F(" Epoch: "));
  DEBUG_print(getEpochTime());
  DEBUG_print(F(" | NTP duration: "));
  DEBUG_println((millis() - start) * 0.001);  
  char bu[64];
  getTimeStr(bu, FS("%d.%m.%y %H:%M"));
  DEBUG_println(bu);
}

size_t getWifiId(char* buffer, size_t bSize) {
  size_t n = 0;
  const char* o = WiFi.macAddress().c_str();
  while (*o && n < bSize-1) {
    if (*o == ':') { o++; continue; }
    buffer[n++] = *o++;
  }
  buffer[n] = 0;
  return n;
}

void setupWiFiAccessPoint() {
  const char WiFiAPPSK[] PROGMEM = WIFI_ACCESS_POINT_PASSWORD; 
#ifdef ARDUINO_ARCH_ESP32
  WiFi.mode(WIFI_MODE_AP);
#endif
#ifdef ARDUINO_ARCH_ESP8266
  WiFi.mode(WIFI_AP);
#endif

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[6];
  WiFi.softAPmacAddress(mac);
  sprintf_P(apName, FS("Apollo-%02X:%02X"), mac[4], mac[5]);
  DEBUG_print(F("\n\nPlease connect to Access Point: "));
  DEBUG_print(apName);
  DEBUG_println(F("\nPassword: " WIFI_ACCESS_POINT_PASSWORD "\nhttp://192.168.4.1"));
  WiFi.softAP(apName, WiFiAPPSK);
  snprintf_P(hostName, sizeof(hostName), FS("apollo-42"));
  WiFi.setHostname(hostName);
}

void WifiDisconnect() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}

void WifiSleep() {
#ifdef ARDUINO_ARCH_ESP8266
  WiFi.forceSleepBegin();  // send wifi to sleep to reduce power consumption
#endif
  yield();
}

void WifiWake() {
#ifdef ARDUINO_ARCH_ESP8266
  WiFi.forceSleepWake();
  WiFi.mode(WIFI_STA);
  ESP.deepSleep(10, WAKE_RFCAL);
#endif
  yield();
}


time_t getEpochTime() {
  time_t now = 0;
  time(&now);
  return now;
}

uint32_t getMillisDelay(uint32_t freq) {
  timeval tv;
  timezone tz;
  gettimeofday(&tv, &tz);
  struct tm * tm = localtime(&tv.tv_sec);
  uint32_t millis = (tm->tm_hour * 60*60*1000) + (tm->tm_min * 60*1000) + (tm->tm_sec * 1000) + (tv.tv_usec/1000); 
  return freq - (millis % freq) + 1;
}

size_t getIsoTime(char* buffer) {
  // 2018-05-27T21:33:19Z
  time_t now = 0;
  time(&now);
  return strftime(buffer, 32, FS("%FT%TZ"), gmtime(&now));
}

size_t getTimeStr(char* buffer, const char* fmt) {
  time_t now = 0;
  time(&now);
  return strftime(buffer, 32, fmt, localtime(&now));
}
