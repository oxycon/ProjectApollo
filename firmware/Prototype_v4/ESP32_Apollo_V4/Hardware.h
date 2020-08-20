#ifndef HARDWARE_H
#define HARDWARE_H

#define MCU_TYPE "ESP32"
#define ADC_CALIBRATION 0.00093965752

#define LED_PIN 5
#define BUTTON_PIN 34

#define VALVE_0_PIN 25
#define VALVE_1_PIN 26
#define VALVE_2_PIN 27
#define VALVE_3_PIN 32
#define VALVE_4_PIN 33

#define VALVE_SCK_PIN 25
#define VALVE_MOSI_PIN 26
#define VALVE_MISO_PIN 27
#define VALVE_LATCH_PIN 32
#define VALVE_ALARM_PIN 33
#define VALVE_SPI_FREQUENCY 10000000

#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

#define O2_RXD_PIN 16 
#define O2_TXD_PIN 17

//  !! Warning !!
//  These must match the TFT_eSPI library. Edit Documents\Arduino\libraries\TFT_eSPI\User_Setup.h 
#define LCD_SCK_PIN 18
#define LCD_MOSI_PIN 23
#define LCD_RST_PIN -1
#define LCD_DC_PIN 2
#define LCD_CS_PIN 15
#define LCD_LED_PIN 12
#define LCD_LED_PWM_CHANNEL 0

#define TOUCH_CS_PIN 14
#define TOUCH_CS_IRQ 13

// Pressure sensor MPRLS
#define MPRLS_CS_PIN 4
#define MPRLS_SPI_FREQUENCY 500000


#endif // HARDWARE_H
