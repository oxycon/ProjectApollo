#ifndef CLI_H
#define CLI_H

void ReadSerial();

class CommandLineInterpreter {
public:
  CommandLineInterpreter() { }
  const char* execute(const char* cmd);
  inline bool isWhiteSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
  inline bool isWhiteSpaceOrEnd(char c) { return c == '\0' || c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
  inline const char* setError(const char* text) { error = true; return text; }

protected:
  const char* setOutput(int pin, const char* cmd);
  size_t readBool(const char* cmd, bool* result);
  size_t tryRead(const char* str, const char* cmd);

  char buffer[256];
  bool error;
};


#endif // CLI_H
