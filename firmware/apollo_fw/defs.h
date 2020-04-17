#ifndef _DEFS_H_
#define _DEFS_H_

#include <stdint.h>

typedef struct
{
	uint8_t  magic;
	uint16_t time_stage_5way;
	uint16_t time_stage_2way;
	uint16_t time_stage_pause;
	uint32_t odometer_hours;
	uint8_t  debug_mode;
	uint16_t checksum;
}
nvm_t;

#define NVM_MAGIC 0xA5

typedef struct
{
	uint8_t seq;
	uint32_t odometer_hours;
	uint32_t millis;
	uint32_t fault_code;
}
fault_t;

enum
{
	FAULTCODE_NONE                  = 0x00,
	FAULTCODE_START_PLACEHOLDER     = 0x01 << 0, // this isn't actually a fault, it's used to pad the LED blinking
	FAULTCODE_UNDERVOLTAGE          = 0x01 << 1,
	FAULTCODE_OVERVOLTAGE           = 0x01 << 2,
	FAULTCODE_COIL_5WAY_SHORT       = 0x01 << 3,
	FAULTCODE_COIL_5WAY_OPEN        = 0x01 << 4,
	FAULTCODE_COIL_2WAY_SHORT       = 0x01 << 5,
	FAULTCODE_COIL_2WAY_OPEN        = 0x01 << 6,
	FAULTCODE_OXY_HW_FAIL           = 0x01 << 7, // oxygen sensor failed to respond
	FAULTCODE_OXY_LOW               = 0x01 << 8,
	FAULTCODE_FLOW_LOW              = 0x01 << 9,
	FAULTCODE_PRESSURE_HW_FAIL      = 0x01 << 10,
	FAULTCODE_PRESSURE_LOW          = 0x01 << 11,
	FAULTCODE_PRESSURE_HIGH         = 0x01 << 12,
	FAULTCODE_END                   = 0x01 << 13,
};

// valve phases, defined in order because the state-machine uses addition to goto next state
enum
{
	PHASE_IDLE,
	PHASE_5WAY_OFF_2WAY_OFF,
	PHASE_5WAY_OFF_2WAY_ON,
	PHASE_5WAY_OFF_2WAY_PAUSE, // optional, may be skipped
	PHASE_5WAY_ON_2WAY_OFF,
	PHASE_5WAY_ON_2WAY_ON,
	PHASE_5WAY_ON_2WAY_PAUSE, // optional, may be skipped
	PHASE_WRAP, // when this is reached, the state-machine wraps back to the top
	PHASE_FAULT,
};

#define TIME_HAS_ELAPSED(now_, stamp_, period_) ((now_) - (stamp_) >= (period_)) // this will account for overflow

// MPR sensor state machine phases
enum
{
	MPRPHASE_START,
	MPRPHASE_WAIT,
	MPRPHASE_READ,
};

enum
{
	MPR_MODE_NONE,
	MPR_MODE_I2C,
	MPR_MODE_SPI,
	MPR_MODE_OTHER,
};

#define MPR_STATUS_MASK    0xF7
#define MPR_STATUS_POWERED 0x40
#define MPR_STATUS_ERROR   0x04

#endif
