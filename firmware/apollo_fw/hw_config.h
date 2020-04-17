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
#define OXY_HW_TIMEOUT_LONG    10000 // max time for oxygen sensor to boot
#define OXY_HW_TIMEOUT_SHORT   2000  // max time between oxygen sensor data packets

#define PIN_12V_MONITOR    A3
#define VMON_R1            10000
#define VMON_R2            620
#define VMON_THRESH_LOW    10000 // undervoltage limit in mV
#define VMON_THRESH_HIGH   13000 // overvoltage  limit in mV
#define VMON_THRESH_HYSTER 500   // hysteresis         in mV

#define PIN_COIL_5WAY      PIN_COIL_1
#define PIN_COIL_2WAY      PIN_COIL_2
#define PIN_COILCHECK_5WAY PIN_COILCHECK_1
#define PIN_COILCHECK_2WAY PIN_COILCHECK_2

#define COIL_SETTLE_TIME   500 // physical time for solenoid to move, and for the back-EMF to dissipate

#define PIN_I2C_SCL        A5
#define PIN_I2C_SDA        A4

#define I2CADDR_TCA9548A (0x70 << 1)
#define I2CADDR_MCP23017 (0x20 << 1)
#define I2CADDR_MPR      (0x18 << 1)
#define PRESSURE_SENSOR_MAX_CNT    8

#define PIN_MPR_RST        D12
#define PIN_MPR_EOC        A0
#define PIN_AUX2           A1
#define PIN_AUX3           A2

#define SPI_BITBANG_DLY    1 // bit delay for SPI bit-bang, might need to be long as the bus is open-drain (shared with I2C)
#define MPR_SAMPLE_TIME    200

#endif
