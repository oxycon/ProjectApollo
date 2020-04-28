
#include <Arduino.h>
#include "debug_console.h"

char output_buffer[OUTPUT_BUFFER_SIZE];

const int maxFloatBufferSize = 8;


void DBG_print_float(float floatNum, int numDigitsAfteDecimal)
{
	char resultBuffer[maxFloatBufferSize]; // Buffer big enough for 7-character float
	memset (resultBuffer, 0, maxFloatBufferSize);

	dtostrf(floatNum, maxFloatBufferSize - 1, numDigitsAfteDecimal, resultBuffer); // Leave room for too large numbers!
	DBG_print(resultBuffer);
}


void DBG_println_float(char* prefix, float floatNum, char* postfix, int numDigitsAfteDecimal)
{
	char resultBuffer[maxFloatBufferSize]; // Buffer big enough for 7-character float
	memset (resultBuffer, 0, maxFloatBufferSize);

	dtostrf(floatNum, maxFloatBufferSize - 1, numDigitsAfteDecimal, resultBuffer); // Leave room for too large numbers!
	DBG_println_buffered("%s%s%s", prefix, resultBuffer, postfix);
}
