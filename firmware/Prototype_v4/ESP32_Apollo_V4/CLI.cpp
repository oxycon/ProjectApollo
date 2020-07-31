#include "Arduino.h"

#include "hardware.h"
#include "config.h"
#include "CLI.h"

#include "Valve.h"
#include "Concentrator.h"


#include <string.h>

const char* help_text = FS("\
led <0|1|on|off|true|false>            Set LED state\r\n\
valve <n> [0|1|on|off|true|false]      Set or read valve state\r\n\
concentrator <0|1|on|off|true|false>   Enable/disable concentrator cycle\r\n\
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
  if (n = tryRead(FS("HELP"), cmd)) { return help_text; }
  if (n = tryRead(FS("?"), cmd)) { return help_text; }
  if (n = tryRead(FS("LED"), cmd)) { return setOutput(LED_PIN, cmd+n); }
  if (n = tryRead(FS("VALVE"), cmd)) { return setValve(cmd+n); }
  if (n = tryRead(FS("CONCENTRATOR"), cmd)) { return controlConcentrator(cmd+n); }
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

  while(!isWhiteSpaceOrEnd(cmd[n])) {
    char c = cmd[n]; 
    if (n==0 && c == '-') {
      is_negative = true;
      n++;
      continue;
    }
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
