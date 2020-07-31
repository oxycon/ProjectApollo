#ifndef CLI_H
#define CLI_H

void ReadSerial();

class CommandLineInterpreter {
public:
  CommandLineInterpreter() { }
  const char* execute(const char* cmd);
  inline bool isWhiteSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
  inline bool isWhiteSpaceOrEnd(char c) { return c == '\0' || c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
  inline const char* setError(const char* text) { error = text; return text; }

protected:
  const char* setOutput(int pin, const char* cmd);
  const char* setValve(const char* cmd);
  const char* controlConcentrator(const char* cmd);
  const char* cycleDuration(const char* cmd);
  const char* cycleValves(const char* cmd);  
  const char* cycleValveMask(const char* cmd);  
  const char* saveConfiguration();
  const char* getIP();
  const char* getMAC();
  const char* getTime();
  const char* restart();
    
  size_t readBool(const char* cmd, bool* result);
  size_t readInteger(const char* cmd, int* result);
  size_t tryRead(const char* str, const char* cmd);

  char buffer[256];
  const char* error;
};


#endif // CLI_H
