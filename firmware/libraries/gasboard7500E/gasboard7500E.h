#ifndef _GASBOARD7500E_H_
#define _GASBOARD7500E_H_

#define O2SENSE_NEED_METADATA

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define float_type double // change to float for smaller memory

//#define O2SENSE_NEED_METADATA // version and serial number, disable to save memory
#define O2SENSE_NEED_CHECKSUM // disable for saving maybe a dozen bytes of memory

#define O2SENSE_BAUD_RATE 9600

void o2sens_init(void);            // call when initializing
void o2sens_feedUartByte(uint8_t); // call whenever UART receives a byte
char o2sens_hasNewData(void);      // check immediately after o2sens_feedUartByte
void o2sens_clearNewData(void);    // clear if new data is handled

// note: 16 bit values are x10 of the actual value, so 1000 means 100%, 100C, and 100L/min
// note: float values are actual value, so 100 means 100%, 100C, and 100L/min
uint16_t    o2sens_getConcentration16(void);
float_type  o2sens_getConcentration(void);
uint16_t    o2sens_getFlowRate16(void);
float_type  o2sens_getFlowRate(void);
uint16_t    o2sens_getTemperature16(void);
float_type  o2sens_getTemperature(void);

uint8_t*    o2sens_getRawBuffer(void);

#ifdef O2SENSE_NEED_METADATA

#define O2SENSE_CMD_VERSIONNUMBER    0x11, 0x01, 0x1E, 0xD0
#define O2SENSE_CMD_SERIALNUMBER     0x11, 0x01, 0x1F, 0xCF

// note: calling layer is responsible for issuing commands over UART, use command definitions as initializers of constant byte array

char* o2sens_getVersionNumber(void);   // length is 8 when available, 0 when not available
uint8_t* o2sens_getSerialNumber(void); // length is always 5, non-zero when available

#endif

#ifdef __cplusplus
}
#endif

#endif
