#include "Arduino.h"

#include "hardware.h"
#include "config.h"
#include "CLI.h"

#include "Valve.h"
#include "Concentrator.h"
#include "Wifi.h"


#include <string.h>

const char* help_text = FS("\
led <0|1|on|off|true|false>            Set LED state\r\n\
valve <n> [0|1|on|off|true|false]      Set or get valve state\r\n\
concentrator <0|1|on|off|true|false>   Enable or disable concentrator cycle\r\n\
cycle-duration <cycle> [miliseconds]   Set or get the duration of a cycle\r\n\
cycle-valves <cycle> [valves]          Set or get cycle valve state bit-map\r\n\
cycle-valve-mask <mask>                Set or get bit-masks of which valves should switch during cycles\r\n\
ssid                                   Set or get WIFI SSID\r\n\
wifi-password                          Set or get WIFI password\r\n\
wifi-ip                                Set or get fixed WIFI IP address\r\n\
dns                                    Set or get fixed WIFI DNS\r\n\
gateway                                Set or get fixed WIFI gateway\r\n\
subnet                                 Set or get fixed WIFI subnet\r\n\
save                                   Save current configuration to FLASH\r\n\
load                                   Restore configuration from FLASH\r\n\
ip                                     Get local-IP address\r\n\
mac                                    Get MAC address\r\n\
time                                   Get current time\r\n\
restart                                Restart the controller\r\n\
help                                   Print help\r\n\
?                                      Print help\r\n\
\r\n");

char serial_command[COMMAND_BUFFER_SIZE];
size_t serial_command_index = 0;
CommandLineInterpreter serial_cli = CommandLineInterpreter();

void ReadSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    serial_command[serial_command_index++] = c;
    if (c == '\n' || c == '\r') { 
      serial_command[serial_command_index-1] = '\0';
      serial_command_index = 0;
      DEBUG_print(F("Execute: "));
      DEBUG_println(serial_command);
      DEBUG_println(serial_cli.execute(serial_command));
    }
    if (serial_command_index > COMMAND_BUFFER_SIZE-1) {
      serial_command_index = COMMAND_BUFFER_SIZE-1;
    }
  }
}

const char* CommandLineInterpreter::execute(const char* cmd) {
  error = nullptr;
  size_t n = 0; 
  if (*cmd == '#') { return FS(""); } // Comment line
  if (n = tryRead(FS("HELP"), cmd)) { return help_text; }
  if (n = tryRead(FS("?"), cmd)) { return help_text; }
  if (n = tryRead(FS("LED"), cmd)) { return setOutput(LED_PIN, cmd+n); }
  if (n = tryRead(FS("VALVE"), cmd)) { return setValve(cmd+n); }
  if (n = tryRead(FS("CONCENTRATOR"), cmd)) { return controlConcentrator(cmd+n); }
  if (n = tryRead(FS("CYCLE-DURATION"), cmd)) { return cycleDuration(cmd+n); }
  if (n = tryRead(FS("CYCLE-VALVES"), cmd)) { return cycleValves(cmd+n);  }
  if (n = tryRead(FS("CYCLE-VALVE-MASK"), cmd)) { return cycleValveMask(cmd+n); }
  if (n = tryRead(FS("SSID"), cmd)) { return wifiSSID(cmd+n); }
  if (n = tryRead(FS("WIFI-PASSWORD"), cmd)) { return wifiPassword(cmd+n); }
  if (n = tryRead(FS("WIFI-IP"), cmd)) { return wifiIP(cmd+n); }
  if (n = tryRead(FS("DNS"), cmd)) { return wifiDNS(cmd+n); }
  if (n = tryRead(FS("GATEWAY"), cmd)) { return wifiGateway(cmd+n); }
  if (n = tryRead(FS("SUBNET"), cmd)) { return wifiSubnet(cmd+n); }
  if (n = tryRead(FS("SAVE"), cmd)) { return saveConfiguration(); }
  if (n = tryRead(FS("LOAD"), cmd)) { return loadConfiguration(); }
  if (n = tryRead(FS("IP"), cmd)) { return getIP(); }
  if (n = tryRead(FS("MAC"), cmd)) { return getMAC(); }
  if (n = tryRead(FS("TIME"), cmd)) { return getTime(); }
  if (n = tryRead(FS("RESTART"), cmd)) { return restart(); }
  return setError(FS("invalid command"));
}

const char* CommandLineInterpreter::setOutput(int pin, const char* cmd) {
  bool state = false;
  size_t n = readBool(cmd, &state);
  if (error) { return error; }
  digitalWrite(pin, state); 
  return FS("OK");  
}

const char* CommandLineInterpreter::setValve(const char* cmd) {
  int valve = 0;
  bool state = false;
  size_t n = readInteger(cmd, &valve);
  if (error) { return error; } else { cmd += n; }
  if (valve < 0 or valve > 8) { return setError(FS("invalid valve number")); }     
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("%d"), get_valve(valve));
    return buffer;
  }
  n = readBool(cmd, &state);
  if (error) { return error; }
  set_valve(valve, state);
  return FS("OK");
}

const char* CommandLineInterpreter::cycleDuration(const char* cmd) {
  int cycle = 0;
  int duration = 0;
  size_t n = readInteger(cmd, &cycle);
  if (error) { return error; } else { cmd += n; }
  if (cycle < 0 or cycle > config.concentrator.cycle_count) { return setError(FS("invalid cycle number")); }     
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("%d"), config.concentrator.duration_ms[cycle]);
    return buffer;
  }
  n = readInteger(cmd, &duration);
  if (error) { return error; }
  config.concentrator.duration_ms[cycle] = duration;
  return FS("OK");
}

const char* CommandLineInterpreter::cycleValves(const char* cmd) {
  int cycle = 0;
  int mask = 0;
  size_t n = readInteger(cmd, &cycle);
  if (error) { return error; } else { cmd += n; }
  if (cycle < 0 or cycle > config.concentrator.cycle_count) { return setError(FS("invalid cycle number")); }     
  if ( cmd[0] == '\0' ) {
    n = sprintf_P(buffer, FS("0x%02x"), config.concentrator.valve_state[cycle]);
    return buffer;
  }
  n = readInteger(cmd, &mask);
  if (error) { return error; }
  config.concentrator.valve_state[cycle] = (uint8_t) mask;
  return FS("OK");
}

const char* CommandLineInterpreter::cycleValveMask(const char* cmd) {
  int mask = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("0x%02x"), config.concentrator.cycle_valve_mask);
    return buffer;
  }
  readInteger(cmd, &mask);
  if (error) { return error; }
  config.concentrator.cycle_valve_mask = (uint8_t) mask;
  return FS("OK");
}

const char* CommandLineInterpreter::controlConcentrator(const char* cmd) {
  bool state = false;
  size_t n = readBool(cmd, &state);
  if (error) { return error; }
  if (state) {
    concentrator_start();
  } else {
    concentrator_stop();
  }
  return FS("OK");  
}

const char* CommandLineInterpreter::wifiSSID(const char* cmd) {
  if ( cmd[0] == '\0' ) {
    return config.wifi.ssid;
  }
  while (isWhiteSpace(*cmd)) { cmd++; }
  strncpy(config.wifi.ssid, cmd, sizeof(config.wifi.ssid));
  config.wifi.ssid[sizeof(config.wifi.ssid) - 1]= '\0';
  return FS("OK");  
}

const char* CommandLineInterpreter::wifiPassword(const char* cmd) {
  if ( cmd[0] == '\0' ) {
    return config.wifi.password;
  }
  while (isWhiteSpace(*cmd)) { cmd++; }
  strncpy(config.wifi.password, cmd, sizeof(config.wifi.password));
  config.wifi.password[sizeof(config.wifi.password) - 1]= '\0';
  return FS("OK");  
}

const char* CommandLineInterpreter::wifiIP(const char* cmd) {
  if ( cmd[0] == '\0' ) {
      sprintf_P(buffer, FS("%d.%d.%d.%d"), config.wifi.ip[0], config.wifi.ip[1], config.wifi.ip[2], config.wifi.ip[3]);
      return buffer;
  }
  uint8_t ip[4];
  readIpAddr(cmd, ip);
  if (error) { return error; }
  memcpy(config.wifi.ip, ip, 4);
  return FS("OK");  
}

const char* CommandLineInterpreter::wifiDNS(const char* cmd) {
  if ( cmd[0] == '\0' ) {
      sprintf_P(buffer, FS("%d.%d.%d.%d"), config.wifi.dns[0], config.wifi.dns[1], config.wifi.dns[2], config.wifi.dns[3]);
      return buffer;
  }
  uint8_t ip[4];
  readIpAddr(cmd, ip);
  if (error) { return error; }
  memcpy(config.wifi.dns, ip, 4);
  return FS("OK");  
}

const char* CommandLineInterpreter::wifiGateway(const char* cmd) {
  if ( cmd[0] == '\0' ) {
      sprintf_P(buffer, FS("%d.%d.%d.%d"), config.wifi.gateway[0], config.wifi.gateway[1], config.wifi.gateway[2], config.wifi.gateway[3]);
      return buffer;
  }
  uint8_t ip[4];
  readIpAddr(cmd, ip);
  if (error) { return error; }
  memcpy(config.wifi.gateway, ip, 4);
  return FS("OK");  
}

const char* CommandLineInterpreter::wifiSubnet(const char* cmd) {
  if ( cmd[0] == '\0' ) {
      sprintf_P(buffer, FS("%d.%d.%d.%d"), config.wifi.subnet[0], config.wifi.subnet[1], config.wifi.subnet[2], config.wifi.subnet[3]);
      return buffer;
  }
  uint8_t ip[4];
  readIpAddr(cmd, ip);
  if (error) { return error; }
  memcpy(config.wifi.subnet, ip, 4);
  return FS("OK");  
}


const char* CommandLineInterpreter::saveConfiguration() {
  saveConfig();
  return FS("OK");  
}

const char* CommandLineInterpreter::loadConfiguration() {
  loadConfig();
  return FS("OK");  
}

const char* CommandLineInterpreter::restart() {
  ESP.restart();
  return FS("OK");  
}

const char* CommandLineInterpreter::getIP() {
  IPAddress ip = getLocalIp();
  sprintf_P(buffer, FS("%d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);
  return buffer;
}

const char* CommandLineInterpreter::getMAC() {
  getWifiId(buffer, sizeof(buffer));
  return buffer;
}

const char* CommandLineInterpreter::getTime() {
  getTimeStr(buffer, FS("%H:%M:%S  |  %d.%m.%Y"));
  return buffer;
}

size_t CommandLineInterpreter::readBool(const char* cmd, bool* result) {
  size_t n = 0; 
  if (n = tryRead(FS("0"), cmd)) { *result = false; return n; }
  if (n = tryRead(FS("false"), cmd)) { *result = false; return n; }
  if (n = tryRead(FS("off"), cmd)) { *result = false; return n; }
  if (n = tryRead(FS("1"), cmd)) { *result = true; return n; }
  if (n = tryRead(FS("true"), cmd)) { *result = true; return n; }
  if (n = tryRead(FS("on"), cmd)) { *result = true; return n; }
  setError(FS("invalid boolean"));
  return 0;
}

size_t CommandLineInterpreter::readInteger(const char* cmd, int* result) {
  size_t n = 0;
  int tmp = 0;
  bool is_negative = false;
  if (cmd[n] == '-') {
    is_negative = true;
    n++;
  }
  if (cmd[n] == '0' && cmd[n+1] == 'b') { return readBinary(cmd+n+2, result, is_negative); }
  if (cmd[n] == '0' && cmd[n+1] == 'x') { return readHex(cmd+n+2, result, is_negative); }

  while(!isWhiteSpaceOrEnd(cmd[n])) {
    char c = cmd[n]; 
    if (c < '0' || c > '9') { 
      setError(FS("invalid integer"));
      return 0; 
    }
    tmp = tmp * 10 + (c - '0');
    n++;
  }
  if (is_negative) { tmp = -tmp; }
  while (isWhiteSpace(cmd[n])) { n++; } 
  *result = tmp;
  return n;
}

size_t CommandLineInterpreter::readBinary(const char* cmd, int* result, bool is_negative) {
  size_t n = 0;
  int tmp = 0;
  if (!is_negative && cmd[n] == '-') {
    is_negative = true;
    n++;
  }
  while(!isWhiteSpaceOrEnd(cmd[n])) {
    char c = cmd[n]; 
    if (c < '0' || c > '1') { 
      setError(FS("invalid binary"));
      return 0; 
    }
    tmp = (tmp << 1) + (c - '0');
    n++;
  }
  if (is_negative) { tmp = -tmp; }
  while (isWhiteSpace(cmd[n])) { n++; } 
  *result = tmp;
  return n;
}

size_t CommandLineInterpreter::readHex(const char* cmd, int* result, bool is_negative) {
  size_t n = 0;
  int tmp = 0;
  if (!is_negative && cmd[n] == '-') {
    is_negative = true;
    n++;
  }
  while(!isWhiteSpaceOrEnd(cmd[n])) {
    char c = toupper(cmd[n]);
    if (c >= '0' && c <= '9') { 
      tmp = (tmp << 4) + (c - '0');    
    } else if (c >= 'A' && c <= 'F') {
      tmp = (tmp << 4) + (c - 'A' + 10);    
    } else {
      setError(FS("invalid hex"));
      return 0; 
    }
    n++;
  }
  if (is_negative) { tmp = -tmp; }
  while (isWhiteSpace(cmd[n])) { n++; } 
  *result = tmp;
  return n;
}

size_t CommandLineInterpreter::readIpAddr(const char* cmd, uint8_t ip[4]) {
  size_t n = 0;
  size_t i = 0;
  while(cmd[n] && i<4) {
    int tmp = atoi(&cmd[n]);
    if (tmp > 255 || tmp < -1) {
      setError(FS("invalid ip address"));
      return 0;
    }
    ip[i++] = tmp;
    while(cmd[n] == ' ' || (cmd[n] >= '0' && cmd[n] <= '9')) { n++; }
    if (cmd[n] == 0) { break; }
    if (cmd[n] == '.') { n++; }
    else {
      setError(FS("invalid ip address"));
      return 0;
    }
  }
  if (i != 4) {
    setError(FS("too short ip address"));
    return 0;
  }
  return n;
}

size_t CommandLineInterpreter::tryRead(const char* str, const char* cmd) {
  size_t n = 0;
  while (true) {
    if (str[n] == '\0') {
      if (!isWhiteSpaceOrEnd(cmd[n])) {
        return 0;
      }
      while (isWhiteSpace(cmd[n])) { n++; } 
      return n;
    }
    if (tolower(cmd[n]) != tolower(str[n])) { return 0; }    
    n++;
  }
}
