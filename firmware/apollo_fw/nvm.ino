#include "defs.h"
#include "config.h"
#include <EEPROM.h>

bool nvm_read(nvm_t* x)
{
	uint8_t* ptr = (uint8_t*)(x);
	uint8_t i;
	uint16_t checksum;
	for (i = 0; i < sizeof(nvm_t); i++)
	{
		ptr[i] = EEPROM.read(NVM_START_ADDR + i);
	}
	if (x->magic != NVM_MAGIC) {
		return false;
	}
	checksum = nvm_checksum(x);
	if (checksum != x->checksum) {
		return false;
	}
	return true;
}

bool nvm_read_or_default(nvm_t* x)
{
	if (nvm_read(x) != false)
	{
		return true;
	}
	nvm_reset(x);
	return false;
}

void nvm_write(nvm_t* x)
{
	uint8_t* ptr = (uint8_t*)(x);
	uint8_t i;
	x->magic = NVM_MAGIC;
	x->checksum = nvm_checksum(x);
	for (i = 0; i < sizeof(nvm_t); i++)
	{
		EEPROM.update(NVM_START_ADDR + i, ptr[i]);
	}
}

void nvm_reset(nvm_t* x)
{
	x->magic = NVM_MAGIC;
	x->time_stage_5way = DEFAULT_TIME_STAGE_5WAY;
	x->time_stage_2way = DEFAULT_TIME_STAGE_2WAY;
	x->odometer_hours = 0;
	x->checksum = nvm_checksum(x);
	nvm_write(x);
}

uint16_t nvm_checksum(nvm_t* x)
{
	uint8_t* ptr = (uint8_t*)(x);
	uint8_t i;
	uint16_t sum1 = 0, sum2 = 0;
	for (i = 0; i < sizeof(nvm_t) - sizeof(uint16_t); i++)
	{
		sum1 = (sum1 + ptr[i]) % 255;
		sum2 = (sum2 + sum1) % 255;
	}
	return sum1 | (sum2 << 8);;
}

void nvm_dump()
{
	uint8_t i;
	for (i = 0; i < sizeof(nvm_t); i++)
	{
		char tbuff[4];
		sprintf_P(tbuff, PSTR("%02X "), EEPROM.read(NVM_START_ADDR + i));
		Serial.print(tbuff);
	}
}
