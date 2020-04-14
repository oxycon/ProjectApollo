#ifndef _DEFS_H_
#define _DEFS_H_

#include <stdint.h>

typedef struct
{
	uint8_t magic;
	int16_t time_stage_5way;
	int16_t time_stage_2way;
	int32_t odometer_hours;
	uint16_t checksum;
}
nvm_t;

#define NVM_MAGIC 0xA5

#endif
