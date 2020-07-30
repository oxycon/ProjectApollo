#include "Arduino.h"

#include "hardware.h"
#include "config.h"
#include "CLI.h"

#include <string.h>

const char* help_text = FS("\
led <0|1|on|off|true|false>\r\n\
5-way <0|1|on|off|true|false>\r\n\
2-way <0|1|on|off|true|false>\r\n\
relief <0|1|on|off|true|false>\r\n\
help\r\n\
?\r\n\
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
  error = false;
  size_t n = 0; 
  if (n = tryRead(FS("HELP"), cmd)) { return help_text; }
  if (n = tryRead(FS("?"), cmd)) { return help_text; }
  if (n = tryRead(FS("LED"), cmd)) { return setOutput(LED_PIN, cmd+n); }
  if (n = tryRead(FS("2-WAY"), cmd)) { return setOutput(VALVE_2_WAY_PIN, cmd+n); }
  if (n = tryRead(FS("5-WAY"), cmd)) { return setOutput(VALVE_5_WAY_PIN, cmd+n); }
  if (n = tryRead(FS("RELIEF"), cmd)) { return setOutput(VALVE_RELIEF_PIN, cmd+n); }
  return setError(FS("invalid command"));
}


const char* CommandLineInterpreter::setOutput(int pin, const char* cmd) {
  bool state = false;
  size_t n = readBool(cmd, &state);
  if (n) { 
    digitalWrite(pin, state); 
    return FS("OK");
  }
  return setError(FS("invalid boolean"));
}


size_t CommandLineInterpreter::readBool(const char* cmd, bool* result) {
  size_t n = 0; 
  if (n = tryRead(FS("0"), cmd)) { *result = false; return n; }
  if (n = tryRead(FS("false"), cmd)) { *result = false; return n; }
  if (n = tryRead(FS("off"), cmd)) { *result = false; return n; }
  if (n = tryRead(FS("1"), cmd)) { *result = true; return n; }
  if (n = tryRead(FS("true"), cmd)) { *result = true; return n; }
  if (n = tryRead(FS("on"), cmd)) { *result = true; return n; }
  return 0;
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
