#include "gasboard7500E.h"
#include <string.h> // for memcpy and memset

// private definitions

enum
{
	O2SENSE_OPCODE_DATA = 0x01,
	O2SENSE_OPCODE_VERSIONNUMBER = 0x1E,
	O2SENSE_OPCODE_SERIALNUMBER = 0x1F,
};

#define HEADER_BYTE 0x16

// private variables

#define BUFFER_SIZE 16 // only 12 is needed
static uint8_t buffer[BUFFER_SIZE];
static uint8_t buffer_idx;
#ifdef O2SENSE_NEED_CHECKSUM
static uint8_t buffer_chksum;
#endif

static char new_flag;
static uint16_t concentration;
static uint16_t flow_rate;
static uint16_t temperature;

#ifdef O2SENSE_NEED_METADATA
static uint8_t serial_number[5];
static char version_number[9];
#endif

// functions

void o2sens_init(void)
{
	buffer_idx = 0;
	new_flag = 0;
	#ifdef O2SENSE_NEED_METADATA
	memset((void*)serial_number, 0, 5);
	version_number[0] = 0; // null terminate for empty string
	version_number[8] = 0; // null terminate for full string
	#endif
}

void o2sens_feedUartByte(uint8_t x)
{
	if (buffer_idx == 0)
	{
		if (x == HEADER_BYTE) // wait for header byte
		{
			buffer[buffer_idx] = x;
			buffer_idx++;
			#ifdef O2SENSE_NEED_CHECKSUM
			buffer_chksum = x;
			#endif
		}
		else
		{
			return; // not the header, ignore, wait for header byte next time
		}
	}
	else // buffer_idx now is 1 or greater
	{
		uint8_t len, opcode;
		buffer[buffer_idx] = x;
		len = buffer[1] + 2; // include header and checksum
		opcode = buffer[2];

		if (len > (9 + 2)) // this is impossible for a valid data packet
		{
			buffer_idx = 0; // reset, wait for header again
			return;
		}

		if (buffer_idx == 2 && opcode != O2SENSE_OPCODE_DATA && opcode != O2SENSE_OPCODE_VERSIONNUMBER && opcode != O2SENSE_OPCODE_SERIALNUMBER)
		{
			// wrong opcode, must've lost sync
			// reset, wait for header again
			buffer_idx = 0; // reset, wait for header again
			return;
		}

		if (buffer_idx >= len) // reached end
		{
			#ifdef O2SENSE_NEED_CHECKSUM
			uint16_t checksum;
			#endif
			buffer_idx = 0; // reset, we can't continue
			#ifdef O2SENSE_NEED_CHECKSUM
			checksum = 0x100;
			checksum -= buffer_chksum; // documentation says to compliment

			if (x == checksum)
			#endif
			{
				// begin to interpret data packet
				if (opcode == O2SENSE_OPCODE_DATA)
				{
					// data format obtained from documentation
					concentration = buffer[3]; concentration <<= 8; concentration += buffer[4];
					flow_rate = buffer[5]; flow_rate <<= 8; flow_rate += buffer[6];
					temperature = buffer[7]; temperature <<= 8; temperature += buffer[8];
					new_flag = 1; // set the flag to notify caller
				}
				#ifdef O2SENSE_NEED_METADATA
				else if (opcode == O2SENSE_OPCODE_VERSIONNUMBER)
				{
					memcpy((void*)version_number, (void*)(&(buffer[3])), 8);
				}
				else if (opcode == O2SENSE_OPCODE_SERIALNUMBER)
				{
					memcpy((void*)serial_number, (void*)(&(buffer[3])), 5);
				}
				#endif
				return; // idx is 0, meaning we are waiting for header again
			}
			#ifdef O2SENSE_NEED_CHECKSUM
			else // checksum mismatch
			{
				return; // idx is 0, meaning we are waiting for header again
			}
			#endif
		}
		else // has not reached end
		{
			buffer_idx++; // next byte
			#ifdef O2SENSE_NEED_CHECKSUM
			buffer_chksum += x; // include in checksum
			#endif
		}
	}
}

char o2sens_hasNewData(void)
{
	return new_flag;
}

void o2sens_clearNewData(void)
{
	new_flag = 0;
}

uint16_t o2sens_getConcentration16(void)
{
	return concentration;
}

float_type o2sens_getConcentration(void)
{
	float_type x = o2sens_getConcentration16();
	return x / 10.0;
}

uint16_t o2sens_getFlowRate16(void)
{
	return flow_rate;
}

float_type o2sens_getFlowRate(void)
{
	float_type x = o2sens_getFlowRate16();
	return x / 10.0;
}

uint16_t o2sens_getTemperature16(void)
{
	return temperature;
}

float_type o2sens_getTemperature(void)
{
	float_type x = o2sens_getTemperature16();
	return x / 10.0;
}

uint8_t* o2sens_getRawBuffer(void)
{
	return buffer;
}

#ifdef O2SENSE_NEED_METADATA
char* o2sens_getVersionNumber(void)
{
	return (char*)version_number;
}

uint8_t* o2sens_getSerialNumber(void)
{
	return (uint8_t*)serial_number;
}
#endif
