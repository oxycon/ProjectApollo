#include "Arduino.h"

#include "Hardware.h"
#include "Config.h"
#include "Error.h"
#include "CLI.h"
#include <ArduinoJson.h>

#include "Valve.h"
#include "Concentrator.h"
#include "OxygenSensor.h"
#include "SensorManager.h"
#include "Display.h"
#include "Wifi.h"


#include <string.h>

const char* help_text = FS("\
led <0|1|on|off|true|false>            Set LED state\r\n\
valve <n> [0|1|on|off|true|false]      Set or get valve state\r\n\
valve-drivers [count]                  Number of installed DRV-8806 valve driver chips\r\n\
concentrator [0|1|on|off|true|false]   Enable or disable concentrator stage\r\n\
stage-duration <stage> [miliseconds]   Set or get the duration of a specifc stage\r\n\
stage-durations [miliseconds] [...]    Set or get the stage durations\r\n\
stage-valves <stage> [valves]          Set or get stage valve state bit-map\r\n\
stage-valve-mask <mask>                Set or get bit-masks of which valves should switch during stages\r\n\
oxygen                                 Get results of last oxygen sensor measurements\r\n\
pressure                               Get results of last pressure sensor measurements\r\n\
humidity                               Get results of last humidity sensor measurements\r\n\
temperature                            Get results of last temperature sensor measurements\r\n\
color                                  Get results of last color sensor measurements\r\n\
errors                                 Get error log\r\n\
adr-ambient [address]                  Set or get the address of the ambient humidity, temperture, pressure sensor\r\n\
adr-intake [address]                   Set or get the address of the intake humidity, temperture sensor\r\n\
adr-desiccant [address]                Set or get the address of the desiccant humidity, temperture sensor\r\n\
adr-output [address]                   Set or get the address of the output humidity, temperture sensor\r\n\
adr-in-pressure [address]              Set or get the address of the intake pressure sensor\r\n\
adr-color [address]                    Set or get the address of the color sensor\r\n\
adr-out-pressure [address]             Set or get the address of the output pressure sensor\r\n\
data-log [0|1|on|off|true|false]       Enable or disable concentrator data logging\r\n\
cycle-stats [0|1|on|off|true|false]    Enable or disable cycle stats logging\r\n\
stats [0|1|on|off|true|false]          Enable or disable long term stats logging\r\n\
debug [0|1|on|off|true|false]          Enable or disable debug logging\r\n\
wifi-enabled [0|1|on|off|true|false]   Enable or disable WIFI on next restart\r\n\
ssid                                   Set or get WIFI SSID\r\n\
wifi-password                          Set or get WIFI password\r\n\
wifi-ip                                Set or get fixed WIFI IP address\r\n\
dns                                    Set or get fixed WIFI DNS\r\n\
gateway                                Set or get fixed WIFI gateway\r\n\
subnet                                 Set or get fixed WIFI subnet\r\n\
time-format                            Set or get format for time representation\r\n\
date-format                            Set or get format for date representation\r\n\
save                                   Save current configuration to FLASH\r\n\
load                                   Restore configuration from FLASH\r\n\
config                                 Return configuration as JSON\r\n\
data                                   Return current sensor data as JSON\r\n\
ip                                     Get local-IP address\r\n\
mac                                    Get MAC address\r\n\
time                                   Get current time\r\n\
timezone [time zone]                   Set or get the local time zone\r\n\
brighness [value]                      Set or get the display brightness 0 to 100\r\n\
restart                                Restart the controller\r\n\
help                                   Print help\r\n\
?                                      Print help\r\n\
\r\n");

char serial_command[COMMAND_BUFFER_SIZE];
size_t serial_command_index = 0;
CommandLineInterpreter serial_cli = CommandLineInterpreter(&Serial);

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
  if (n = tryRead(FS("VALVE-DRIVERS"), cmd)) { return valveDrivers(cmd+n); }
  if (n = tryRead(FS("CONCENTRATOR"), cmd)) { return controlConcentrator(cmd+n); }
  if (n = tryRead(FS("STAGE-DURATION"), cmd)) { return stageDuration(cmd+n); }
  if (n = tryRead(FS("STAGE-DURATIONS"), cmd)) { return stageDurations(cmd+n); }
  if (n = tryRead(FS("STAGE-VALVES"), cmd)) { return stageValves(cmd+n);  }
  if (n = tryRead(FS("STAGE-VALVE-MASK"), cmd)) { return stageValveMask(cmd+n); }
  if (n = tryRead(FS("OXYGEN"), cmd)) { return getOxygenSensorData(cmd+n); }
  if (n = tryRead(FS("PRESSURE"), cmd)) { return getPressureSensorData(cmd+n); }
  if (n = tryRead(FS("HUMIDITY"), cmd)) { return getHumiditySensorData(cmd+n); }
  if (n = tryRead(FS("TEMPERATURE"), cmd)) { return getTemperatureSensorData(cmd+n); }
  if (n = tryRead(FS("COLOR"), cmd)) { return getColorSensorData(cmd+n); }
  if (n = tryRead(FS("ERRORS"), cmd)) { return getErrorLog(cmd+n); }
  if (n = tryRead(FS("ADR-AMBIENT"), cmd)) { return ambientAdr(cmd+n); }
  if (n = tryRead(FS("ADR-INTAKE"), cmd)) { return intakeAdr(cmd+n); }
  if (n = tryRead(FS("ADR-DESICCANT"), cmd)) { return desiccantAdr(cmd+n); }
  if (n = tryRead(FS("ADR-OUTPUT"), cmd)) { return outputAdr(cmd+n); }
  if (n = tryRead(FS("ADR-COLOR"), cmd)) { return colorAdr(cmd+n); }
  if (n = tryRead(FS("ADR-IN-PRESSURE"), cmd)) { return inPressureAdr(cmd+n); }
  if (n = tryRead(FS("ADR-OUT-PRESSURE"), cmd)) { return outPressureAdr(cmd+n); }
  if (n = tryRead(FS("DATA-LOG"), cmd)) { return controlDataLog(cmd+n); }
  if (n = tryRead(FS("CYCLE-STATS"), cmd)) { return controlCycleStats(cmd+n); }
  if (n = tryRead(FS("STATS"), cmd)) { return controlStats(cmd+n); }
  if (n = tryRead(FS("DEBUG"), cmd)) { return controlDebug(cmd+n); }
  if (n = tryRead(FS("WIFI-ENABLED"), cmd)) { return wifiEnabled(cmd+n); }
  if (n = tryRead(FS("SSID"), cmd)) { return wifiSSID(cmd+n); }
  if (n = tryRead(FS("WIFI-PASSWORD"), cmd)) { return wifiPassword(cmd+n); }
  if (n = tryRead(FS("WIFI-IP"), cmd)) { return wifiIP(cmd+n); }
  if (n = tryRead(FS("DNS"), cmd)) { return wifiDNS(cmd+n); }
  if (n = tryRead(FS("GATEWAY"), cmd)) { return wifiGateway(cmd+n); }
  if (n = tryRead(FS("SUBNET"), cmd)) { return wifiSubnet(cmd+n); }
  if (n = tryRead(FS("TIME-FORMAT"), cmd)) { return timeFormat(cmd+n); }
  if (n = tryRead(FS("DATE-FORMAT"), cmd)) { return dateFormat(cmd+n); }
  if (n = tryRead(FS("SAVE"), cmd)) { return saveConfiguration(); }
  if (n = tryRead(FS("LOAD"), cmd)) { return loadConfiguration(); }
  if (n = tryRead(FS("CONFIG"), cmd)) { return jsonConfig(); }
  if (n = tryRead(FS("DATA"), cmd)) { return jsonData(); }
  if (n = tryRead(FS("IP"), cmd)) { return getIP(); }
  if (n = tryRead(FS("MAC"), cmd)) { return getMAC(); }
  if (n = tryRead(FS("TIME"), cmd)) { return getTime(); }
  if (n = tryRead(FS("TIME-ZONE"), cmd)) { return timeZone(cmd+n); }
  if (n = tryRead(FS("BRIGHTNESS"), cmd)) { return brightness(cmd+n); }
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

const char* CommandLineInterpreter::valveDrivers(const char* cmd) {
  int count = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("%d"), config.concentrator.drv8806_count);
    return buffer;
  }
  size_t n = readInteger(cmd, &count);
  if (error) { return error; } else { cmd += n; }
  if (count < 0 or count > 2) { return setError(FS("invalid driver count")); }     
  set_valve_driver_count(count);
  return FS("OK");
}


const char* CommandLineInterpreter::stageDuration(const char* cmd) {
  int stage = 0;
  int duration = 0;
  size_t n = readInteger(cmd, &stage);
  if (error) { return error; } else { cmd += n; }
  if (stage < 0 or stage > config.concentrator.stage_count) { return setError(FS("invalid stage number")); }     
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("%d"), config.concentrator.duration_ms[stage]);
    return buffer;
  }
  n = readInteger(cmd, &duration);
  if (error) { return error; }
  config.concentrator.duration_ms[stage] = duration;
  return FS("OK");
}

const char* CommandLineInterpreter::stageDurations(const char* cmd) {
  if ( cmd[0] == '\0' ) {
    size_t n = 0;
    for (size_t stage=0; stage < config.concentrator.stage_count; stage++) {
      n += snprintf_P(buffer+n, sizeof(buffer)-n, FS("%d, "), config.concentrator.duration_ms[stage]);
    }
    buffer[n-2] = '\0';
    return buffer;
  }
  int stage = 0;
  int duration = 0;

  while (stage < config.concentrator.stage_count) {
    size_t n = readInteger(cmd, &duration);
    if (error) { return error; } else { cmd += n; }
    config.concentrator.duration_ms[stage] = duration;
    stage++;
    if ( cmd[0] == '\0' ) { break; }
  }
  return FS("OK");
}

const char* CommandLineInterpreter::stageValves(const char* cmd) {
  int stage = 0;
  int mask = 0;
  size_t n = readInteger(cmd, &stage);
  if (error) { return error; } else { cmd += n; }
  if (stage < 0 or stage > config.concentrator.stage_count) { return setError(FS("invalid stage number")); }     
  if ( cmd[0] == '\0' ) {
    n = sprintf_P(buffer, FS("0x%02x"), config.concentrator.valve_state[stage]);
    return buffer;
  }
  n = readInteger(cmd, &mask);
  if (error) { return error; }
  config.concentrator.valve_state[stage] = (uint8_t) mask;
  return FS("OK");
}

const char* CommandLineInterpreter::stageValveMask(const char* cmd) {
  int mask = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("0x%02x"), config.concentrator.stage_valve_mask);
    return buffer;
  }
  readInteger(cmd, &mask);
  if (error) { return error; }
  config.concentrator.stage_valve_mask = (uint8_t) mask;
  return FS("OK");
}

const char* CommandLineInterpreter::controlConcentrator(const char* cmd) {
  bool state = false;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("%d"), concentrator_is_enabled);
    return buffer;
  }
  size_t n = readBool(cmd, &state);
  if (error) { return error; }
  if (state) {
    concentrator_start();
  } else {
    concentrator_stop();
  }
  return FS("OK");  
}

const char* CommandLineInterpreter::controlDataLog(const char* cmd) {
  bool state = false;
  if ( cmd[0] == '\0' ) {
    size_t n = csv_concentrator_data_header(buffer, sizeof(buffer)-1);
    buffer[n++] = '\n'; buffer[n] = '\0';
    n += csv_concentrator_data(buffer+n, sizeof(buffer)-n);
    return buffer;
  }
  size_t n = readBool(cmd, &state);
  if (error) { return error; }
  buffer[0] = '\0';
  if (state) { csv_concentrator_data_header(buffer, sizeof(buffer)-1); } 
  concentrator_data_stream = state ? stream : nullptr;
  return buffer;  
}

const char* CommandLineInterpreter::controlCycleStats(const char* cmd) {
  bool state = false;
  if ( cmd[0] == '\0' ) {
    size_t n = csv_stats_header(buffer, sizeof(buffer)-1);
    buffer[n++] = '\n'; buffer[n] = '\0';
    n += csv_stats(buffer+n, cycle_stats, sizeof(buffer)-n);
    return buffer;
  }
  size_t n = readBool(cmd, &state);
  if (error) { return error; }
  buffer[0] = '\0';
  if (state) { csv_stats_header(buffer, sizeof(buffer)-1); } 
  cycle_stats_stream = state ? stream : nullptr;
  return buffer;  
}

const char* CommandLineInterpreter::controlStats(const char* cmd) {
  bool state = false;
  if ( cmd[0] == '\0' ) {
    size_t n = csv_stats_header(buffer, sizeof(buffer)-1);
    buffer[n++] = '\n'; buffer[n] = '\0';
    n += csv_stats(buffer+n, concentrator_stats, sizeof(buffer)-n);
    return buffer;
  }
  size_t n = readBool(cmd, &state);
  if (error) { return error; }
  buffer[0] = '\0';
  if (state) { csv_stats_header(buffer, sizeof(buffer)-1); } 
  concentrator_stats_stream = state ? stream : nullptr;
  return buffer;  
}

const char* CommandLineInterpreter::controlDebug(const char* cmd) {
  bool state = false;
  if ( cmd[0] == '\0' ) {
    if (debugStream == nullptr) { 
      return FS("off");
    } else if (debugStream == &Serial) {
      return FS("serial");
    } else if (debugStream == stream) {
      return FS("here");
    } else {
      return FS("on");
    }
  }
  size_t n = readBool(cmd, &state);
  if (error) { return error; }
  debugStream = state ? stream : nullptr;
  return FS("OK");  
}

const char* CommandLineInterpreter::wifiEnabled(const char* cmd) {
  bool state = false;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("%d"), !config.wifi.is_disabled);
    return buffer;
  }
  size_t n = readBool(cmd, &state);
  if (error) { return error; }
  config.wifi.is_disabled = !state;
  return FS("OK");  
}

const char* CommandLineInterpreter::getOxygenSensorData(const char* cmd) {
  o2_sensor_data2csv(buffer, sizeof(buffer));
  return buffer;  
}

const char* CommandLineInterpreter::getPressureSensorData(const char* cmd) {
  size_t n=0;
  for (size_t i=0; i<installed_sensor_count; i++) {
    Sensor* s = installed_sensors[i];
    if (s->hasPressure()) {
      if (i>0 && n<sizeof(buffer)-3) {buffer[n++] = '\n'; buffer[n++] = '\r';}
      n += snprintf_P(buffer+n, sizeof(buffer)-n, "%s, %s, %0.1f hPa", s->name, s->getTypeName(), s->getPressure());
    }
  }
  buffer[sizeof(buffer)-1] = '\0';
  return buffer;  
}

const char* CommandLineInterpreter::getHumiditySensorData(const char* cmd) {
  size_t n=0;
  for (size_t i=0; i<installed_sensor_count; i++) {
    Sensor* s = installed_sensors[i];
    if (s->hasHumidity()) {
      if (i>0 && n<sizeof(buffer)-3) {buffer[n++] = '\n'; buffer[n++] = '\r';}
      n += snprintf_P(buffer+n, sizeof(buffer)-n, "%s, %s, %0.1f %%", s->name, s->getTypeName(), s->getHumidity());
    }
  }
  buffer[sizeof(buffer)-1] = '\0';  
  return buffer;  
}

const char* CommandLineInterpreter::getTemperatureSensorData(const char* cmd) {
  size_t n=0;
  for (size_t i=0; i<installed_sensor_count; i++) {
    Sensor* s = installed_sensors[i];
    if (s->hasTemperature()) {
      if (i>0 && n<sizeof(buffer)-3) {buffer[n++] = '\n'; buffer[n++] = '\r';}
      n += snprintf_P(buffer+n, sizeof(buffer)-n, "%s, %s, %0.1f C", s->name, s->getTypeName(), s->getTemperature());
    }
  }
  buffer[sizeof(buffer)-1] = '\0';
  return buffer;  
}

const char* CommandLineInterpreter::getColorSensorData(const char* cmd) {
  if (!color_sensor) {
    return FS("No color sensor found");
  }
  color_sensor->getDataCsv(buffer, sizeof(buffer));
  return buffer;  
}

const char* CommandLineInterpreter::getErrorLog(const char* cmd) {
  if (!error_count) {
    return FS("No errors found.");
  }

  size_t n = 0;
  size_t i = 0;
  while (true) {
    size_t nn = get_error_log(i++, buffer+n, sizeof(buffer)-n-1);
    n += nn;
    buffer[n++] = '\n'; buffer[n] = '\0';
    if (!nn) { break; }
  } 
  buffer[sizeof(buffer)-1] = '\0';
  return buffer;

}

const char* CommandLineInterpreter::ambientAdr(const char* cmd) {
  int address = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("0x%X"), config.concentrator.ambient_sensor_address);
    return buffer;
  }
  readInteger(cmd, &address);
  if (error) { return error; }
  config.concentrator.ambient_sensor_address = address;
  return FS("OK");
}

const char* CommandLineInterpreter::intakeAdr(const char* cmd) {
  int address = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("0x%X"), config.concentrator.intake_sensor_address);
    return buffer;
  }
  readInteger(cmd, &address);
  if (error) { return error; }
  config.concentrator.intake_sensor_address = address;
  return FS("OK");
}

const char* CommandLineInterpreter::desiccantAdr(const char* cmd) {
  int address = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("0x%X"), config.concentrator.desiccant_sensor_address);
    return buffer;
  }
  readInteger(cmd, &address);
  if (error) { return error; }
  config.concentrator.desiccant_sensor_address = address;
  return FS("OK");
}

const char* CommandLineInterpreter::outputAdr(const char* cmd) {
  int address = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("0x%X"), config.concentrator.output_sensor_address);
    return buffer;
  }
  readInteger(cmd, &address);
  if (error) { return error; }
  config.concentrator.output_sensor_address = address;
  return FS("OK");
}

const char* CommandLineInterpreter::colorAdr(const char* cmd) {
  int address = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("0x%X"), config.concentrator.color_sensor_address);
    return buffer;
  }
  readInteger(cmd, &address);
  if (error) { return error; }
  config.concentrator.color_sensor_address = address;
  return FS("OK");
}

const char* CommandLineInterpreter::inPressureAdr(const char* cmd) {
  int address = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("0x%X"), config.concentrator.in_pressure_address);
    return buffer;
  }
  readInteger(cmd, &address);
  if (error) { return error; }
  config.concentrator.in_pressure_address = address;
  return FS("OK");
}

const char* CommandLineInterpreter::outPressureAdr(const char* cmd) {
  int address = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("0x%X"), config.concentrator.out_pressure_address);
    return buffer;
  }
  readInteger(cmd, &address);
  if (error) { return error; }
  config.concentrator.out_pressure_address = address;
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

const char* CommandLineInterpreter::timeFormat(const char* cmd) {
  if ( cmd[0] == '\0' ) {
    return config.time_format;
  }
  while (isWhiteSpace(*cmd)) { cmd++; }
  strncpy(config.time_format, cmd, sizeof(config.time_format));
  config.wifi.ssid[sizeof(config.time_format) - 1]= '\0';
  return FS("OK");  
}

const char* CommandLineInterpreter::dateFormat(const char* cmd) {
  if ( cmd[0] == '\0' ) {
    return config.date_format;
  }
  while (isWhiteSpace(*cmd)) { cmd++; }
  strncpy(config.date_format, cmd, sizeof(config.date_format));
  config.wifi.ssid[sizeof(config.date_format) - 1]= '\0';
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

const char* CommandLineInterpreter::jsonConfig() {
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<2048> doc;

  JsonObject concentrator_obj = doc.createNestedObject("concentrator");
  concentrator_obj[FS("drv8806_count")] = config.concentrator.drv8806_count;
  concentrator_obj[FS("stage_count")] = config.concentrator.stage_count;
  size_t n = 0; buffer[n++] = '[';
  for (size_t i=0; i<MAX_CONCENTRATOR_STAGES; i++) { n += snprintf_P(buffer+n, sizeof(buffer)-n-1, FS("%d,"), config.concentrator.valve_state[i]); }
  buffer[n-1] = ']';
  concentrator_obj[FS("valve_state")] = serialized(buffer);
  n = 0; buffer[n++] = '[';
  for (size_t i=0; i<MAX_CONCENTRATOR_STAGES; i++) { n += snprintf_P(buffer+n, sizeof(buffer)-n-1, FS("%d,"), config.concentrator.duration_ms[i]); }
  buffer[n-1] = ']';
  concentrator_obj[FS("duration_ms")] = serialized(buffer);  
  concentrator_obj[FS("stage_valve_mask")] = config.concentrator.stage_valve_mask;

  concentrator_obj[FS("ambient_sensor_address")] = config.concentrator.ambient_sensor_address; 
  concentrator_obj[FS("intake_sensor_address")] = config.concentrator.intake_sensor_address;
  concentrator_obj[FS("desiccant_sensor_address")] = config.concentrator.desiccant_sensor_address;
  concentrator_obj[FS("output_sensor_address")] = config.concentrator.output_sensor_address;   
  concentrator_obj[FS("color_sensor_address")] = config.concentrator.color_sensor_address;   
  concentrator_obj[FS("in_pressure_address")] = config.concentrator.in_pressure_address;   
  concentrator_obj[FS("out_pressure_address")] = config.concentrator.out_pressure_address;   
  concentrator_obj[FS("mprls_min_pressure")] = config.concentrator.mprls_min_pressure;
  concentrator_obj[FS("mprls_max_pressure")] = config.concentrator.mprls_max_pressure;   

  JsonObject wifi_obj = doc.createNestedObject("wifi");
  wifi_obj[FS("SSID")] = config.wifi.ssid;
  wifi_obj[FS("password")] = config.wifi.password;
  sprintf_P(buffer, FS("%d.%d.%d.%d"), config.wifi.ip[0], config.wifi.ip[1], config.wifi.ip[2], config.wifi.ip[3]);
  wifi_obj[FS("IP")] = buffer;
  sprintf_P(buffer, FS("%d.%d.%d.%d"), config.wifi.dns[0], config.wifi.dns[1], config.wifi.dns[2], config.wifi.dns[3]);
  wifi_obj[FS("DNS")] = buffer;
  sprintf_P(buffer, FS("%d.%d.%d.%d"), config.wifi.gateway[0], config.wifi.gateway[1], config.wifi.gateway[2], config.wifi.gateway[3]);
  wifi_obj[FS("gateway")] = buffer;
  sprintf_P(buffer, FS("%d.%d.%d.%d"), config.wifi.subnet[0], config.wifi.subnet[1], config.wifi.subnet[2], config.wifi.subnet[3]);
  wifi_obj[FS("subnet")] = buffer;
  wifi_obj[FS("disabled")] = config.wifi.is_disabled;

  doc[FS("time_zone")] = config.time_zone;
  getTouchCalibrationJson(buffer);
  doc[FS("touch_calibration")] = serialized(buffer);
  doc[FS("time_format")] = config.time_format;
  doc[FS("date_format")] = config.date_format;
  doc[FS("brightness")] = config.display_brightness;
  doc[FS("adc_calibration")] = config.adc_calibration;
  doc[FS("config_size")] = config.config_size;

  JsonObject dynamic_obj = doc.createNestedObject("dynamic");
  getIsoTime(buffer);
  dynamic_obj[FS("time")] = buffer;
  dynamic_obj[FS("MAC")] = getWifiMac();
  IPAddress ip = getLocalIp();
  sprintf_P(buffer, FS("%d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);
  dynamic_obj[FS("IP")] = buffer;
  dynamic_obj[FS("RSSI")] = getRSSI();
  dynamic_obj[FS("running")] = concentrator_is_enabled;
  dynamic_obj[FS("debug")] = debugStream == nullptr ? FS("off") : debugStream == &Serial ? FS("serial") : debugStream == stream ? FS("here") : FS("on");
  for (size_t i=0; i<installed_sensor_count; i++) {
    Sensor* s = installed_sensors[i];
    s->getSensorJson(buffer); 
    dynamic_obj[s->name] = serialized(buffer); 
  }
  if (color_sensor){
    color_sensor->getSensorJson(buffer); 
    dynamic_obj["color_sensor"] = serialized(buffer);     
  }

  dynamic_obj[FS("error_count")] = error_count;
  if (error_count > 0) {
    JsonArray log_array = dynamic_obj.createNestedArray("error_log");
    size_t n = 0;
    while (true) {
      bool got_log = get_error_json(n++, buffer, sizeof(buffer));
      if (!got_log) { break; }
      log_array.add(serialized(buffer));
    } 
  }

  JsonArray error_array = doc.createNestedArray("error_map");
  for (size_t n; n<MAX_ERROR; n++) {
     error_array.add(ERROR_STRING[n]);
  }

  if (stream != nullptr) {
    serializeJsonPretty(doc, *stream);
    return FS("");
  } else {
    serializeJsonPretty(doc, buffer, sizeof(buffer));
    return buffer;
  }
}

const char* CommandLineInterpreter::jsonData() {
  StaticJsonDocument<1024> doc;
  doc[FS("up_time_ms")] = millis();
  doc[FS("stage")] = concentrator_stage;
  // doc[FS("next_stage_ms")] = concentrator_is_enabled ? max((int)(next_stage_ms - millis()), 0) : -1;
  doc[FS("valve_state")] = current_valve_states;
  
  JsonObject oxy_obj = doc.createNestedObject("oxygen");
  oxy_obj[FS("concentration")] = o2s_concentration;
  oxy_obj[FS("flow")] = o2s_flow;
  oxy_obj[FS("temperature")] = o2s_temperature;

  for (size_t i=0; i<installed_sensor_count; i++) {
    Sensor* s = installed_sensors[i];
    s->getDataJson(buffer); 
    doc[s->name] = serialized(buffer); 
  }
  if (color_sensor) { color_sensor->getDataJson(buffer); doc[FS("color")] = serialized(buffer); }
  doc[FS("error_count")] = error_count;
  doc[FS("error_flags")] = getErrorFlags(); 

  if (stream != nullptr) {
    serializeJsonPretty(doc, *stream);
    return FS("");
  } else {
    serializeJsonPretty(doc, buffer, sizeof(buffer));
    return buffer;
  }
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

const char* CommandLineInterpreter::brightness(const char* cmd) {
  int value = 0;
  if ( cmd[0] == '\0' ) {
    sprintf_P(buffer, FS("%d"), config.display_brightness);
    return buffer;
  }
  size_t n = readInteger(cmd, &value);
  if (error) { return error; } else { cmd += n; }
  if (value < 0 or value > 100) { return setError(FS("invalid brightness")); }     
  set_display_brightness(value);
  return FS("OK");
}

const char* CommandLineInterpreter::getTime() {
  getTimeStr(buffer, FS("%H:%M:%S  |  %d.%m.%Y"));
  return buffer;
}

const char* CommandLineInterpreter::timeZone(const char* cmd) {
  if ( cmd[0] == '\0' ) {
    return config.time_zone;
  }
  while (isWhiteSpace(*cmd)) { cmd++; }
  strncpy(config.time_zone, cmd, sizeof(config.time_zone));
  config.time_zone[sizeof(config.time_zone) - 1]= '\0';
  return FS("OK");  
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
