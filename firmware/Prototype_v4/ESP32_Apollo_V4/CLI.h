#ifndef CLI_H
#define CLI_H

#include <Stream.h>

void ReadSerial();

class CommandLineInterpreter {
public:
  CommandLineInterpreter(Stream* stream=nullptr) : stream(stream) { }
  const char* execute(const char* cmd);
  inline bool isWhiteSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
  inline bool isWhiteSpaceOrEnd(char c) { return c == '\0' || c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
  inline const char* setError(const char* text) { error = text; return text; }

protected:
  const char* setOutput(int pin, const char* cmd);
  const char* setValve(const char* cmd);
  const char* valveDrivers(const char* cmd);
  const char* controlConcentrator(const char* cmd);
  const char* cycleDuration(const char* cmd);
  const char* cycleValves(const char* cmd);  
  const char* cycleValveMask(const char* cmd);
  const char* getOxygenSensorData(const char* cmd);
  const char* oxygenSensorEnable(const char* cmd);
  const char* oxygenSensorPeriod(const char* cmd);
  const char* ambientAdr(const char* cmd);
  const char* intakeAdr(const char* cmd);
  const char* desiccantAdr(const char* cmd);
  const char* outputAdr(const char* cmd);
  const char* controlDebug(const char* cmd);
  const char* wifiEnabled(const char* cmd);
  const char* wifiSSID(const char* cmd);
  const char* wifiPassword(const char* cmd);
  const char* wifiIP(const char* cmd);
  const char* wifiDNS(const char* cmd);
  const char* wifiGateway(const char* cmd);
  const char* wifiSubnet(const char* cmd);
  const char* saveConfiguration();
  const char* loadConfiguration();
  const char* jsonConfig();
  const char* jsonData();
  const char* getIP();
  const char* getMAC();
  const char* brightness(const char* cmd);
  const char* getTime();
  const char* timeZone(const char* cmd);
  const char* restart();
    
  size_t readBool(const char* cmd, bool* result);
  size_t readInteger(const char* cmd, int* result);
  size_t readBinary(const char* cmd, int* result, bool is_negative=false);
  size_t readHex(const char* cmd, int* result, bool is_negative=false);
  size_t readIpAddr(const char* cmd, uint8_t ip[4]);
  size_t tryRead(const char* str, const char* cmd);

  Stream* stream;
  char buffer[256];
  const char* error;
};


#endif // CLI_H
