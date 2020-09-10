
#pragma once

const int OUTPUT_BUFFER_SIZE = 64;
extern char output_buffer[OUTPUT_BUFFER_SIZE];

const int MAX_FLOAT_BUFFER_SIZE = 12;


#define DBG_print(...) Serial.print(__VA_ARGS__); 
#define DBG_println(...) Serial.println(__VA_ARGS__); 
#define DBG_print_buffered(...) { sprintf(output_buffer, __VA_ARGS__); Serial.print(output_buffer); }
#define DBG_println_buffered(...) { sprintf(output_buffer, __VA_ARGS__); Serial.println(output_buffer); }

void DBG_print_float(float floatNum, int numDigitsAfteDecimal);
void DBG_println_float(char* prefix, float floatNum, char* postfix, int numDigitsAfteDecimal);
