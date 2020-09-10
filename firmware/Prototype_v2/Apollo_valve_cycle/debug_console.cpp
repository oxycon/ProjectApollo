
#include <Arduino.h>
#include "debug_console.h"

char output_buffer[OUTPUT_BUFFER_SIZE];


void DBG_print_float(float floatNum, int numDigitsAfteDecimal)
{
	char resultBuffer[MAX_FLOAT_BUFFER_SIZE]; // Buffer big enough for 7-character float
	memset (resultBuffer, 0, MAX_FLOAT_BUFFER_SIZE);

	dtostrf(floatNum, MAX_FLOAT_BUFFER_SIZE - 1, numDigitsAfteDecimal, resultBuffer); // Leave room for too large numbers!
	DBG_print(resultBuffer);
}


void DBG_println_float(char* prefix, float floatNum, char* postfix, int numDigitsAfteDecimal)
{
	char resultBuffer[MAX_FLOAT_BUFFER_SIZE]; // Buffer big enough for 7-character float
	memset (resultBuffer, 0, MAX_FLOAT_BUFFER_SIZE);

	dtostrf(floatNum, MAX_FLOAT_BUFFER_SIZE - 1, numDigitsAfteDecimal, resultBuffer); // Leave room for too large numbers!
	DBG_println_buffered("%s%s%s", prefix, resultBuffer, postfix);
}
