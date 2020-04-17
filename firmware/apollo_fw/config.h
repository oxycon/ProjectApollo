#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include "defs.h"
#include "hw_config.h"

#define NVM_START_ADDR    0x0000
#define FAULTS_START_ADDR 0x0040

#define DEFAULT_TIME_STAGE_5WAY 5000
#define DEFAULT_TIME_STAGE_2WAY 7000

#define CMD_BUFFER_SIZE 64

#define BUTTON_HOLD_TIME 1000

#endif
