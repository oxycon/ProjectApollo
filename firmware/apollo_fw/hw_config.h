#ifndef _HW_CONFIG_H_
#define _HW_CONFIG_H_

#include <stdint.h>

#define PIN_LED_RED        D9
#define PIN_LED_GREEN      D10
#define PIN_BUTTON         D11
#define PIN_BUZZER         D8
#define PIN_COIL_1         D4
#define PIN_COIL_2         D6
#define PIN_COILCHECK_1    D5
#define PIN_COILCHECK_2    D7
#define PIN_O2SENS_RX      D3
#define PIN_O2SENS_TX      D2
#define PIN_12V_MONITOR    A3

#define PIN_COIL_5WAY      PIN_COIL_1
#define PIN_COIL_2WAY      PIN_COIL_2
#define PIN_COILCHECK_5WAY PIN_COILCHECK_1
#define PIN_COILCHECK_2WAY PIN_COILCHECK_2

#define PIN_I2C_SCL        A5
#define PIN_I2C_SDA        A4

#define PIN_MPR_RST        D12
#define PIN_MPR_EOC        A0
#define PIN_AUX2           A1
#define PIN_AUX3           A2

#endif
