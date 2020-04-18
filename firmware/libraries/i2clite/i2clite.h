#ifndef i2clite_h
#define i2clite_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <util/twi.h>

void i2c_init();
void i2c_deinit();
void i2c_start(uint8_t address);
void i2c_stop();
void i2c_write(uint8_t data);
uint8_t i2c_readAck();
uint8_t i2c_readNak();
void i2c_waitTx();
void i2c_writeReg(uint8_t add, uint8_t reg, uint8_t val);
uint8_t i2c_readReg(uint8_t add, uint8_t reg);
void i2c_writeBlock(uint8_t add, uint8_t* data, uint8_t len);
void i2c_readBlock(uint8_t add, uint8_t* data, uint8_t len);
void i2c_writeBlock_P(uint8_t add, const uint8_t* data, uint16_t len);

extern uint8_t i2c_errorFlag;

#ifdef __cplusplus
}
#endif

#endif