//***********************************************************************************
// App Extern Globals                                                               *
//***********************************************************************************
extern unsigned int count, FrequencyA, FrequencyB, TmpFrequency, AccelTimeBase, AccelPPS, TmpAccelPPS;
extern char StepperIndex, PhaseDutyCycle, Device, Speed;
extern bool StepperDirection;
//***********************************************************************************
// Firmware Revision                                                                *
//***********************************************************************************
#define     FIRMWARE_REVISION   1
//***********************************************************************************
// Application Specific Hardware Definitions                                        *
//***********************************************************************************
#define     StatusLEDPin    0x20
#define     SPI_MISO    (0x40)      //PORT4.6
#define     SPI_MOSI    (0x02)      //PORT4.1
#define     SPI_CLK     (0x04)      //PORT4.2
#define     SPI_LATCH   (0x08)      //PORT4.3

#define		DRV8803		(0x00)
#define		DRV8804		(0x02)
#define		DRV8805		(0x01)
#define         DRV8806         (0x03)

#define		ACCEL		(0x00)
#define		DCCEL		(0x01)

#define     P1OUT_INIT  0
#define     P2OUT_INIT  0
#define     P3OUT_INIT  0
#define     P4OUT_INIT  0
#define     P5OUT_INIT  0
#define     P6OUT_INIT  0


//***********************************************************************************
// Application Specific Definitions                                                 *
//***********************************************************************************
#define     OPCODE      SerialBuffer[0]
//***********************************************************************************
// Application Specific Opcodes                                                     *
//***********************************************************************************
#define     WRITE_GPIO              0x00
#define     PULSE_TMR               0x01
#define     FREQ_TMR                0x02
#define     FREQ_TMR_STOP           0x03
#define     UNIV_SPI                0x04
#define     CONFIG_DEVICE           0x05
#define     SET_TMR_OUT             0x06
#define     ENABLE_PWM              0x07
#define     DISABLE_PWM             0x08
#define     UPDATE_PWM              0x09
#define     PARALLEL_STEPPER        0x0A
#define     SERIAL_STEPPER          0x0B
#define     PARALLEL_STEPPER_STOP   0x0C
#define     SERIAL_STEPPER_STOP     0x0D
#define	    WRITE_ACCEL	            0x0E
#define     READ_DIAG               0x0F

//Memory Access Opcodes
#define     READ_MEM                0xE0
#define     WRITE_WMEM              0xE1
#define     WRITE_BMEM              0xE2

// System Opcodes
#define     GET_FW_REV              0xF0
#define     RESET_MCU               0xF1
#define     SHOW_CLKS               0xF2

//***********************************************************************************
// Application Specific Externs                                                     *
//***********************************************************************************

extern char StatusLED;
extern void Send_SPI_Data(int);
