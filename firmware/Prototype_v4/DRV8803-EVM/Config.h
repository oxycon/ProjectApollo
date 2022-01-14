//******************************************************************************
//  LT Thomas
//  Texas Instruments Inc.
//  May 2011
//  Built with CCS4
//******************************************************************************
#include <msp430f2616.h>
#include "main.h"
#include "AppDef.h"
#include "COMM.h"
#include "WaveForms.h"

//***********************************************************************************
// Internal Time References                                                         *
//***********************************************************************************

#define     XT1_CLK_SPEED       16000000
#define     ACLK_SPEED          XT1_CLK_SPEED

//***********************************************************************************
// Multi Tasking and RTOS Related Variables                                         *
//***********************************************************************************

#define     NUMBER_OF_TASKS     8
#define     TIME_PER_TASK       125         // in us
#define     TASK_TIME           (XT1_CLK_SPEED/ 1000000)  * TIME_PER_TASK

//***********************************************************************************
// Basic Enable Disable Definitions                                                 *
//***********************************************************************************
#define     ENABLE          (0xFF)
#define     DISABLE         (0x00)
#define     INPUT           (0x00)
#define     OUTPUT          (0x01)
#define     IO_FUNCTION     (0x00)
#define     PERIPHERAL      (0x01)
#define     RISING          (0x00)
#define     FALLING         (0x01)

//***********************************************************************************
// Basic Clock System DCO Control Register                                          *
//                                                                                  *
// Configures the Resistor Select, Auxiliary Clock Divider, External Oscillator 1   *
// Mode, External Oscillator 2 State                                                *
//***********************************************************************************
#define     DCO_CONF        (0x03) << 5     // A number from 0 to 7 (defines frequency)
#define     MOD_CONF        (0x00)          // A number from 0 to 31 (defines modulation)

//***********************************************************************************
// Basic Clock System Control Register 1                                            *
//                                                                                  *
// Configures the Resistor Select, Auxiliary Clock Divider, External Oscillator 1   *
// Mode, External Oscillator 2 State                                                *
//***********************************************************************************

//External Oscillator 2 Enable/Disable (0 enables XT2, 1 disables XT2)
#define     XT2_ENABLE      (0x00)
#define     XT2_DISABLE     (0x80)
//Choose from above
#define     XT2OFF_CONF     XT2_DISABLE

//External Oscillator 1 Mode Frequency Select (0 Low Frequency, 1 for High Frequency)
#define     LFTX1_LOW_FREQ  (0x00)
#define     LFTX1_HIGH_FREQ (0x40)
//Choose from above
#define     XTS_CONF        LFTX1_HIGH_FREQ

//Auxiliary Clock Divider
#define     DIVA_DIV1       (0x00)
#define     DIVA_DIV2       (0x10)
#define     DIVA_DIV4       (0x20)
#define     DIVA_DIV8       (0x30)
//Choose from above for Auxiliary Clock Divider
#define     DIVA_CONF       DIVA_DIV1

//DCO Resistor Select (For the 8 Frequency Steps)
#define     RSEL_0          (0x00)
#define     RSEL_1          (0x01)
#define     RSEL_2          (0x02)
#define     RSEL_3          (0x03)
#define     RSEL_4          (0x04)
#define     RSEL_5          (0x05)
#define     RSEL_6          (0x06)
#define     RSEL_7          (0x07)
#define     RSEL_8          (0x08)
#define     RSEL_9          (0x09)
#define     RSEL_A          (0x0A)
#define     RSEL_B          (0x0B)
#define     RSEL_C          (0x0C)
#define     RSEL_D          (0x0D)
#define     RSEL_E          (0x0E)
#define     RSEL_F          (0x0F)
//Choose from above to trim DCO frequency
#define     RSEL_CONF       RSEL_C

//***********************************************************************************
// Basic Clock System Control Register 2                                            *
//                                                                                  *
// Configures the Master Clock Source, Master Clock Divider, Sub Main Clock Source  *
// Sub Main Clock Divider and DCO Resistor Selection                                *
//***********************************************************************************

//Master Clock Source Select SELM0 and SELM1 on BCSCTL2
#define     SELM_DCO1       (0x00)
#define     SELM_DCO2       (0x40)
#define     SELM_XT2CLK     (0x80)
#define     SELM_LFXT1CLK   (0xC0)
//Choose from Above for Master Clock Source
#define     SELM_CONF       SELM_DCO1

//Divider for Master Clock DIVM0 and DIVM1 on BCSCTL2
#define     DIVM_DIV1       (0x00)
#define     DIVM_DIV2       (0x10)
#define     DIVM_DIV4       (0x20)
#define     DIVM_DIV8       (0x30)
//Choose from above for Master Clock Divider
#define     DIVM_CONF       DIVM_DIV1

//Select Sub Main Clock Source (0 for DCO, 1 for XT2CLK/LFXT1CLK)
#define     SELS_DCLK       (0x00)
#define     SELS_XT2        (0x08)
#define     SELS_CONF       SELS_DCLK

//Divider for Sub Main Clock DIVS0 and DIVS1 on BCSCTL2
#define     DIVS_DIV1       (0x00)
#define     DIVS_DIV2       (0x02)
#define     DIVS_DIV4       (0x04)
#define     DIVS_DIV8       (0x06)
//Choose from above for Sub Main Clock Divider
#define     DIVS_CONF       DIVS_DIV1

//Select External/Internal DCO Resistor (0 for Internal, 1 for External)
#define     DCOR_INTERN     (0x00)
#define     DCOR_EXTERN     (0x01)
#define     DCOR_CONF       DCOR_INTERN

//***********************************************************************************
// Basic Clock System Control Register 3                                            *
//                                                                                  *
// Configures the External Oscillators                                              *
//***********************************************************************************

//XT2 Range Select
#define     XT2S_1MXTAL       (0x00)
#define     XT2S_3MXTAL       (0x40)
#define     XT2S_16MXTAL      (0x80)
#define     XT2S_XCLOCK       (0xC0)
//Choose from Above for XT2 Frequency Range
#define     XT2S_CONF       XT2S_16MXTAL

//LFXT1S Low Frequency Clock Select and LFXT1 Range Select
#define     LFXT1S_1MXTAL       (0x00)
#define     LFXT1S_3MXTAL       (0x10)
#define     LFXT1S_16MXTAL      (0x20)
#define     LFXT1S_XCLOCK       (0x30)     
//Choose from above for LFXT1 range select
#define     LFXT1S_CONF         LFXT1S_16MXTAL

//XCAP Oscillator Capacitor Selection
#define     XCAP_1pf            (0x00)
#define     XCAP_6pf            (0x04)
#define     XCAP_10pf           (0x08)
#define     XCAP_12pf           (0x0C)
//Choose from above for Oscillator Capacitor Selection.
#define     XCAP_CONF           XCAP_1pf

//***********************************************************************************
// Interrupt Enable Registers                                                       *
//                                                                                  *
// Configures the Watchdog Timer, Oscillator Fault, Non Maskable and Flass Access   *
// interrupt enables as found on the IE1 register.                                  *
//***********************************************************************************
#define     WDTIEEN         DISABLE         //Watchdog Timer Interrupt Enable
#define     OFIEEN          DISABLE         //Oscillator Fault Interrupt Enable
#define     NMIIEEN         DISABLE         //Non Maskable Interrupt Enable
#define     ACCVIEEN        DISABLE         //Flash Memory Access Violation

//***********************************************************************************
// Interrupt Enable Registers                                                       *
//                                                                                  *
// Configures the Watchdog Timer, Oscillator Fault, Non Maskable and Flass Access   *
// interrupt enables as found on the IE1 register.                                  *
//***********************************************************************************
#define     WDTIEEN         DISABLE         //Watchdog Timer Interrupt Enable
#define     OFIEEN          DISABLE         //Oscillator Fault Interrupt Enable
#define     NMIIEEN         DISABLE         //Non Maskable Interrupt Enable
#define     ACCVIEEN        DISABLE         //Flash Memory Access Violation

#define     UCA0RXIEEN      ENABLE         //USCI A0 Receive Interrupt
#define     UCA0TXIEEN      DISABLE         //USCI A0 Transmit Interrupt
#define     UCB0RXIEEN      DISABLE         //USCI B0 Receive Interrupt
#define     UCB0TXIEEN      DISABLE         //USCI B0 Transmit Interrupt

//***********************************************************************************
// USCI Configuration                                                               *
//                                                                                  *
// Configures the Universal Serial Communications Interface                         *
//***********************************************************************************
#define     UCPEN_PARDIS    (0x00)
#define     UCPEN_PAREN     (0x80)
#define     UCPENEN         UCPEN_PARDIS

#define     UCPAR_ODD       (0x00)
#define     UCPAR_EVEN      (0x40)
#define     UCPAREN         UCPAR_ODD

#define     UCMSB_LSB       (0x00)
#define     UCMSB_MSB       (0x20)
#define     UCMSBEN         UCMSB_LSB

#define     UC7BIT_8        (0x00)
#define     UC7BIT_7        (0x10)
#define     UC7BITEN        UC7BIT_8

#define     UCSPB_1         (0x00)
#define     UCSPB_2         (0x08)
#define     UCSPBEN         UCSPB_1

#define     UCMODE0_UART    (0x00)
#define     UCMODE0_IDLE    (0x02)
#define     UCMODE0_ADDRESS (0x04)
#define     UCMODE0_AUTO    (0x06)
#define     UCMODE0CONF     UCMODE0_UART

#define     UCSYNC_ASYNC    (0x00)
#define     UCSYNC_SYNC     (0x01)
#define     UCSYNCEN        UCSYNC_ASYNC

// USCI A Control Register 1

#define     UCSSEL0_UCLK    (0x00)
#define     UCSSEL0_ACLK    (0x40)
#define     UCSSEL0_SMCLK1  (0x80)
#define     UCSSEL0_SMCLK2  (0xC0)
#define     UCSSEL0CONF     UCSSEL0_SMCLK1

#define     UCRXEIE_DIS     (0x00)
#define     UCRXEIE_EN      (0x20)
#define     UCRXEIEEN       UCRXEIE_DIS

#define     UCBRKIE_DIS     (0x00)
#define     UCBRKIE_EN      (0x10)
#define     UCBRKIEEN       UCBRKIE_DIS

#define     UCDORM_NOT      (0x00)
#define     UCDORM_YES      (0X08)
#define     UCDORMEN        UCDORM_NOT

#define     UCTXADDR_DATA   (0x00)
#define     UCTXADDR_ADDR   (0X04)
#define     UCTXADDREN      UCTXADDR_DATA

#define     UCTXBRK_NBRK    (0x00)
#define     UCTXBRK_BREAK   (0X02)
#define     UCTXBRKEN       UCTXBRK_NBRK

#define     UCSWRST_DIS     (0x00)
#define     UCSWRST_EN      (0X01)
#define     UCSWRSTEN       UCSWRST_DIS

//Modulation Control Register

#define     UCBRF0CONF      0
#define     UCBRSCONF       6

#define     UCOS16_DIS      (0x00)
#define     UCOS16_EN       (0x01)
#define     UCOS16EN        UCOS16_DIS

#define     BAUDRATE0_LO    0x82
#define     BAUDRATE0_HI    0x06
//***********************************************************************************
// Watch Dog Timer Configuration                                                    *
//                                                                                  *
// Configures the Watch Dog Timer firing rate and different operation modes         *
//***********************************************************************************
#define     WDTIS_32768     (0x00)
#define     WDTIS_8192      (0x01)
#define     WDTIS_512       (0x02)
#define     WDTIS_64        (0x03)
// Select from above to configure Watchdog Timer Interval
#define     WDTIS_CONF      WDTIS_32768

#define     WDTSSEL_SMCLK   (0x00)
#define     WDTSSEL_ACLK    (0x04)
//Select from above to select Watchdog Timer source
#define     WDTSSEL_CONF    WDTSSEL_ACLK

#define     WDTCNTCL_NOCLR  (0x00)
#define     WDTCNTCL_CLEAR  (0x08)
#define     WDTCNTCL_CONF   WDTCNTCL_NOCLR

#define     WDTTMSEL_WDT    (0x00)
#define     WDTTMSEL_ITM    (0x10)
#define     WDTTMSEL_CONF   WDTTMSEL_WDT

#define     WDTNMI_RST      (0x00)
#define     WDTNMI_NMI      (0x20)
#define     WDTNMI_CONF     WDTNMI_RST

#define     WDTNMIES_RISE   (0x00)
#define     WDTNMIES_FALL   (0x40)
#define     WDTNMIES_CONF   WDTNMIES_RISE

#define     WDTHOLD_NS      (0x00)
#define     WDTHOLD_STP     (0x80)
#define     WDTHOLD_CONF    WDTHOLD_STP

//***********************************************************************************
// Digital I/O Direction Configuration                                              *
//                                                                                  *
// Configures the direction of each port pin as an OUTPUT (1) or an INPUT (0)       *
//***********************************************************************************

// Port 1 Direction Configure
#define     P1DIR7          OUTPUT
#define     P1DIR6          OUTPUT
#define     P1DIR5          OUTPUT
#define     P1DIR4          OUTPUT
#define     P1DIR3          OUTPUT
#define     P1DIR2          OUTPUT
#define     P1DIR1          OUTPUT
#define     P1DIR0          OUTPUT

// Port 2 Direction Configure
#define     P2DIR7          OUTPUT
#define     P2DIR6          OUTPUT
#define     P2DIR5          OUTPUT
#define     P2DIR4          OUTPUT
#define     P2DIR3          OUTPUT
#define     P2DIR2          OUTPUT
#define     P2DIR1          OUTPUT
#define     P2DIR0          OUTPUT

// Port 3 Direction Configure
#define     P3DIR7          OUTPUT
#define     P3DIR6          OUTPUT
#define     P3DIR5          OUTPUT
#define     P3DIR4          OUTPUT
#define     P3DIR3          OUTPUT
#define     P3DIR2          OUTPUT
#define     P3DIR1          INPUT
#define     P3DIR0          INPUT

// Port 4 Direction Configure
#define     P4DIR7          OUTPUT
#define     P4DIR6          OUTPUT
#define     P4DIR5          OUTPUT
#define     P4DIR4          OUTPUT
#define     P4DIR3          OUTPUT
#define     P4DIR2          OUTPUT
#define     P4DIR1          OUTPUT
#define     P4DIR0          OUTPUT

// Port 5 Direction Configure
#define     P5DIR7          OUTPUT
#define     P5DIR6          OUTPUT
#define     P5DIR5          OUTPUT
#define     P5DIR4          OUTPUT
#define     P5DIR3          OUTPUT
#define     P5DIR2          OUTPUT
#define     P5DIR1          OUTPUT
#define     P5DIR0          OUTPUT

// Port 6 Direction Configure
#define     P6DIR7          OUTPUT
#define     P6DIR6          OUTPUT
#define     P6DIR5          OUTPUT
#define     P6DIR4          OUTPUT
#define     P6DIR3          OUTPUT
#define     P6DIR2          OUTPUT
#define     P6DIR1          OUTPUT
#define     P6DIR0          OUTPUT

//***********************************************************************************
// Digital I/O Resistor Enable                                                      *
//                                                                                  *
// Configures the Availability of a Pull Up/Down Resistor (0) disable, (1) enable   *
//***********************************************************************************

// Port 1 Pull Up/Down Enable/Disable Configure
#define     P1REN7          DISABLE
#define     P1REN6          DISABLE
#define     P1REN5          DISABLE
#define     P1REN4          DISABLE
#define     P1REN3          DISABLE
#define     P1REN2          DISABLE
#define     P1REN1          DISABLE
#define     P1REN0          DISABLE

// Port 2 Pull Up/Down Enable/Disable Configure
#define     P2REN7          DISABLE
#define     P2REN6          DISABLE
#define     P2REN5          DISABLE
#define     P2REN4          DISABLE
#define     P2REN3          DISABLE
#define     P2REN2          DISABLE
#define     P2REN1          DISABLE
#define     P2REN0          DISABLE

// Port 3 Pull Up/Down Enable/Disable Configure
#define     P3REN7          DISABLE
#define     P3REN6          DISABLE
#define     P3REN5          DISABLE
#define     P3REN4          DISABLE
#define     P3REN3          DISABLE
#define     P3REN2          DISABLE
#define     P3REN1          DISABLE
#define     P3REN0          DISABLE

// Port 4 Pull Up/Down Enable/Disable Configure
#define     P4REN7          DISABLE
#define     P4REN6          DISABLE
#define     P4REN5          DISABLE
#define     P4REN4          DISABLE
#define     P4REN3          DISABLE
#define     P4REN2          DISABLE
#define     P4REN1          DISABLE
#define     P4REN0          DISABLE

// Port 5 Pull Up/Down Enable/Disable Configure
#define     P5REN7          DISABLE
#define     P5REN6          DISABLE
#define     P5REN5          DISABLE
#define     P5REN4          DISABLE
#define     P5REN3          DISABLE
#define     P5REN2          DISABLE
#define     P5REN1          DISABLE
#define     P5REN0          DISABLE

// Port 6 Pull Up/Down Enable/Disable Configure
#define     P6REN7          DISABLE
#define     P6REN6          DISABLE
#define     P6REN5          DISABLE
#define     P6REN4          DISABLE
#define     P6REN3          DISABLE
#define     P6REN2          DISABLE
#define     P6REN1          DISABLE
#define     P6REN0          DISABLE

//***********************************************************************************
// Alternate Peripheral Module Function Enable                                      *
//                                                                                  *
// Configures the Alternate Peripheral Mode for a particular pin (if available)     *
// Choose between IO_FUNCTION (0) or PERIPHERAL (1)                                 *
//***********************************************************************************
// Port 1 Alternate Function Select
#define     P1SEL7          IO_FUNCTION
#define     P1SEL6          IO_FUNCTION
#define     P1SEL5          IO_FUNCTION
#define     P1SEL4          IO_FUNCTION
#define     P1SEL3          IO_FUNCTION
#define     P1SEL2          IO_FUNCTION
#define     P1SEL1          IO_FUNCTION
#define     P1SEL0          IO_FUNCTION

// Port 2 Alternate Function Select
#define     P2SEL7          IO_FUNCTION
#define     P2SEL6          IO_FUNCTION
#define     P2SEL5          IO_FUNCTION
#define     P2SEL4          IO_FUNCTION
#define     P2SEL3          IO_FUNCTION
#define     P2SEL2          IO_FUNCTION
#define     P2SEL1          IO_FUNCTION
#define     P2SEL0          IO_FUNCTION

// Port 3 Alternate Function Select
#define     P3SEL7          IO_FUNCTION
#define     P3SEL6          IO_FUNCTION
#define     P3SEL5          PERIPHERAL
#define     P3SEL4          PERIPHERAL
#define     P3SEL3          IO_FUNCTION
#define     P3SEL2          IO_FUNCTION
#define     P3SEL1          IO_FUNCTION
#define     P3SEL0          IO_FUNCTION

// Port 4 Alternate Function Select
#define     P4SEL7          IO_FUNCTION
#define     P4SEL6          IO_FUNCTION
#define     P4SEL5          IO_FUNCTION
#define     P4SEL4          IO_FUNCTION
#define     P4SEL3          IO_FUNCTION
#define     P4SEL2          IO_FUNCTION
#define     P4SEL1          IO_FUNCTION
#define     P4SEL0          IO_FUNCTION

// Port 5 Alternate Function Select
#define     P5SEL7          IO_FUNCTION
#define     P5SEL6          IO_FUNCTION
#define     P5SEL5          IO_FUNCTION
#define     P5SEL4          IO_FUNCTION
#define     P5SEL3          IO_FUNCTION
#define     P5SEL2          IO_FUNCTION
#define     P5SEL1          IO_FUNCTION
#define     P5SEL0          IO_FUNCTION

// Port 6 Alternate Function Select
#define     P6SEL7          IO_FUNCTION
#define     P6SEL6          IO_FUNCTION
#define     P6SEL5          IO_FUNCTION
#define     P6SEL4          IO_FUNCTION
#define     P6SEL3          IO_FUNCTION
#define     P6SEL2          IO_FUNCTION
#define     P6SEL1          IO_FUNCTION
#define     P6SEL0          IO_FUNCTION

//***********************************************************************************
// PORT 1 and 2 Interrupt Enable and Edge Select                                    *
//                                                                                  *
// Configures the Pin On Change Interrupt capability and triggering edge            *
// This function precedes the Alternate Function Select                             *
//***********************************************************************************

// Port 1 Pin On Change Interrupt Enable
#define     P1IE7          DISABLE
#define     P1IE6          DISABLE
#define     P1IE5          DISABLE
#define     P1IE4          DISABLE
#define     P1IE2          DISABLE
#define     P1IE3          DISABLE
#define     P1IE1          DISABLE
#define     P1IE0          DISABLE

// Port 2 Pin On Change Interrupt Enable
#define     P2IE7          DISABLE
#define     P2IE6          DISABLE
#define     P2IE5          DISABLE
#define     P2IE4          DISABLE
#define     P2IE2          DISABLE
#define     P2IE3          DISABLE
#define     P2IE1          DISABLE
#define     P2IE0          DISABLE

// Port 1 Pin On Change Interrupt Edge Select
#define     P1IES7          RISING
#define     P1IES6          RISING
#define     P1IES5          RISING
#define     P1IES4          RISING
#define     P1IES2          RISING
#define     P1IES3          RISING
#define     P1IES1          RISING
#define     P1IES0          RISING

// Port 2 Pin On Change Interrupt Edge Select
#define     P2IES7          RISING
#define     P2IES6          RISING
#define     P2IES5          RISING
#define     P2IES4          RISING
#define     P2IES2          RISING
#define     P2IES3          RISING
#define     P2IES1          RISING
#define     P2IES0          RISING

//***********************************************************************************
// Timer A Control Registers                                                        *
//                                                                                  *
// Configures Timer A and it's different operating modes                            *
//***********************************************************************************

#define     TASSEL_TACLK    (0x0000)
#define     TASSEL_ACLK     (0x0100)
#define     TASSEL_SMCLK    (0x0200)
#define     TASSEL_INCLK    (0x0300)
// Choose from above to configure Timer A Clock Source
#define     TASSEL_CONF     TASSEL_ACLK

#define     TA_ID_DIV1      (0x0000)
#define     TA_ID_DIV2      (0x0040)
#define     TA_ID_DIV4      (0x0080)
#define     TA_ID_DIV8      (0x00C0)
// Choose from above to configure Timer A Clock Divide
#define     TA_ID_CONF      TA_ID_DIV2//TA_ID_DIV1

#define     TA_MC_STOP      (0x0000)
#define     TA_MC_UP        (0x0010)
#define     TA_MC_CONT      (0x0020)
#define     TA_MC_UPDN      (0x0030)
// Choose from above to configure Timer A Counting Mode
#define     TA_MC_CONF      TA_MC_CONT

#define     TAIE_DISABLE    (0x0000)
#define     TAIE_ENABLE     (0x0002)
// Choose from above to enable or disable Timer A Interrupt
#define     TAIE_CONF       TAIE_ENABLE

//***********************************************************************************
// Timer A Capture?Compare Control Register                                         *
//                                                                                  *
// Configures The Capture and Compare engines                                       *
//***********************************************************************************

#define     TA_CM_NONE         (0x0000)
#define     TA_CM_RISE         (0x4000)
#define     TA_CM_FALL         (0x8000)
#define     TA_CM_BOTH         (0xC000)
// Choose from above to configure Timer A's Capture Engine
#define     TA_CM0_CONF        TA_CM_NONE
#define     TA_CM1_CONF        TA_CM_NONE
#define     TA_CM2_CONF        TA_CM_NONE

#define     TA_CCIS_CCIA       (0x0000)
#define     TA_CCIS_CCIB       (0x1000)
#define     TA_CCIS_GND        (0x2000)
#define     TA_CCIS_VCC        (0x3000)
// Choose from above to configure Timer A's Capture Input
#define     TA_CCIS0_CONF      TA_CCIS_CCIA
#define     TA_CCIS1_CONF      TA_CCIS_CCIA
#define     TA_CCIS2_CONF      TA_CCIS_CCIA

#define     TA_SCS_ASYNCH      (0x0000)
#define     TA_SCS_SYNCH       (0x0800)
// Choose from above to configure Synchronous or Asynchronous CAPCOM
#define     TA_SCS0_CONF       TA_SCS_ASYNCH
#define     TA_SCS1_CONF       TA_SCS_ASYNCH
#define     TA_SCS2_CONF       TA_SCS_ASYNCH

#define     TA_CAP_COMP        (0x0000)
#define     TA_CAP_CAPT        (0x0100)
// Choose from above to configure as either Capture or Compare
#define     TA_CAP0_CONF       TA_CAP_COMP
#define     TA_CAP1_CONF       TA_CAP_COMP
#define     TA_CAP2_CONF       TA_CAP_COMP

#define     TA_OUTMOD_BIT      (0x0000)
#define     TA_OUTMOD_SET      (0x0020)
#define     TA_OUTMOD_TOGRES   (0x0040)
#define     TA_OUTMOD_SETRES   (0x0060)
#define     TA_OUTMOD_TOGGLE   (0x0080)
#define     TA_OUTMOD_RESET    (0x00A0)
#define     TA_OUTMOD_TOGSET   (0x00C0)
#define     TA_OUTMOD_RESSET   (0x00E0)
// Choose from above to configure Output Mode while on Compare Mode
#define     TA_OUTMOD0_CONF    TA_OUTMOD_SET
#define     TA_OUTMOD1_CONF    TA_OUTMOD_RESSET
#define     TA_OUTMOD2_CONF    TA_OUTMOD_RESSET

#define     TA_CCIE_DISABLE    (0x0000)
#define     TA_CCIE_ENABLE     (0x0010)
// Choose from above to enable or disable Interrupt
#define     TA_CCIE0_CONF      TA_CCIE_ENABLE
#define     TA_CCIE1_CONF      TA_CCIE_DISABLE
#define     TA_CCIE2_CONF      TA_CCIE_DISABLE

#define     TA_OUT_LOW         (0x0000)
#define     TA_OUT_HIGH        (0x0004)
// Choose from above to choose output pin polarity
#define     TA_OUT0_CONF       TA_OUT_LOW
#define     TA_OUT1_CONF       TA_OUT_LOW
#define     TA_OUT2_CONF       TA_OUT_LOW

//***********************************************************************************
// Timer B Control Registers                                                        *
//                                                                                  *
// Configures Timer B and it's different operating modes                            *
//***********************************************************************************

#define     TBCLGRP_EACH    (0x0000)
#define     TBCLGRP_DUAL    (0x2000)
#define     TBCLGRP_TRIO    (0x4000)
#define     TBCLGRP_ALL     (0xC000)
// Choose from above to configure Latch Loading Grouping
#define     TBCLGRP_CONF    TBCLGRP_EACH

#define     TB_CNTL_16      (0x0000)
#define     TB_CNTL_12      (0x0800)
#define     TB_CNTL_10      (0x1000)
#define     TB_CNTL_08      (0x1800)
// Choose from above to configure Timer B Length
#define     TB_CNTL_CONF     TB_CNTL_16

#define     TBSSEL_TBCLK    (0x0000)
#define     TBSSEL_ACLK     (0x0100)
#define     TBSSEL_SMCLK    (0x0200)
#define     TBSSEL_INCLK    (0x0300)
// Choose from above to configure Timer B Clock Source
#define     TBSSEL_CONF     TBSSEL_ACLK//TBSSEL_SMCLK

#define     TB_ID_DIV1      (0x0000)
#define     TB_ID_DIV2      (0x0040)
#define     TB_ID_DIV4      (0x0080)
#define     TB_ID_DIV8      (0x00C0)
// Choose from above to configure Timer B Clock Divide
#define     TB_ID_CONF      TB_ID_DIV4

#define     TB_MC_STOP      (0x0000)
#define     TB_MC_UP        (0x0010)
#define     TB_MC_CONT      (0x0020)
#define     TB_MC_UPDN      (0x0030)
// Choose from above to configure Timer B Counting Mode
#define     TB_MC_CONF      TB_MC_CONT

#define     TBIE_DISABLE    (0x0000)
#define     TBIE_ENABLE     (0x0002)
// Choose from above to enable or disable Timer B Interrupt
#define     TBIE_CONF       TBIE_ENABLE

//***********************************************************************************
// Timer A Capture?Compare Control Register                                         *
//                                                                                  *
// Configures The Capture and Compare engines                                       *
//***********************************************************************************

#define     TB_CM_NONE         (0x0000)
#define     TB_CM_RISE         (0x4000)
#define     TB_CM_FALL         (0x8000)
#define     TB_CM_BOTH         (0xC000)
// Choose from above to configure Timer B's Capture Engine
#define     TB_CM0_CONF        TB_CM_BOTH
#define     TB_CM1_CONF        TB_CM_RISE           //STEP Input
#define     TB_CM2_CONF        TB_CM_BOTH           //RESET INDEX Input
#define     TB_CM3_CONF        TB_CM_BOTH           //ENABLE STEPPER Input
#define     TB_CM4_CONF        TB_CM_BOTH           //DIRECTION Input
#define     TB_CM5_CONF        TB_CM_RISE           //Acceleration
#define     TB_CM6_CONF        TB_CM_NONE

#define     TB_CCIS_CCIA       (0x0000)
#define     TB_CCIS_CCIB       (0x1000)
#define     TB_CCIS_GND        (0x2000)
#define     TB_CCIS_VCC        (0x3000)
// Choose from above to configure Timer B's Capture Input
#define     TB_CCIS0_CONF      TB_CCIS_CCIB
#define     TB_CCIS1_CONF      TB_CCIS_CCIB
#define     TB_CCIS2_CONF      TB_CCIS_CCIB
#define     TB_CCIS3_CONF      TB_CCIS_CCIB
#define     TB_CCIS4_CONF      TB_CCIS_CCIA
#define     TB_CCIS5_CONF      TB_CCIS_CCIB
#define     TB_CCIS6_CONF      TB_CCIS_CCIA

#define     TB_SCS_ASYNCH      (0x0000)
#define     TB_SCS_SYNCH       (0x0800)
// Choose from above to configure Synchronous or Asynchronous CAPCOM
#define     TB_SCS0_CONF       TB_SCS_ASYNCH
#define     TB_SCS1_CONF       TB_SCS_ASYNCH
#define     TB_SCS2_CONF       TB_SCS_ASYNCH
#define     TB_SCS3_CONF       TB_SCS_ASYNCH
#define     TB_SCS4_CONF       TB_SCS_ASYNCH
#define     TB_SCS5_CONF       TB_SCS_ASYNCH
#define     TB_SCS6_CONF       TB_SCS_ASYNCH

#define     TB_CLLD_00          (0x0000)        //TBCLx loads on write to TBCCRx
#define     TB_CLLD_01          (0x0200)        //TBCLx loads when TBR counts to 0
#define     TB_CLLD_10          (0x0400)        //TBCLx loads when: TBR counts to 0 (up or continuous mode) or counts to TBCL0 or to 0 (up/down mode)
#define     TB_CLLD_11          (0x0600)        //TBCLx loads then TBR counts to TBCLx
// Choose from above to configure Compare Latch Load
#define     TB_CLLD0_CONF       TB_CLLD_00
#define     TB_CLLD1_CONF       TB_CLLD_00
#define     TB_CLLD2_CONF       TB_CLLD_00
#define     TB_CLLD3_CONF       TB_CLLD_00
#define     TB_CLLD4_CONF       TB_CLLD_00
#define     TB_CLLD5_CONF       TB_CLLD_00
#define     TB_CLLD6_CONF       TB_CLLD_00

#define     TB_CAP_COMP        (0x0000)
#define     TB_CAP_CAPT        (0x0100)
// Choose from above to configure as either Capture or Compare
#define     TB_CAP0_CONF       TB_CAP_COMP
#define     TB_CAP1_CONF       TB_CAP_COMP 
#define     TB_CAP2_CONF       TB_CAP_COMP 
#define     TB_CAP3_CONF       TB_CAP_COMP  
#define     TB_CAP4_CONF       TB_CAP_COMP 
#define     TB_CAP5_CONF       TB_CAP_COMP
#define     TB_CAP6_CONF       TB_CAP_COMP

#define     TB_OUTMOD_BIT      (0x0000)
#define     TB_OUTMOD_SET      (0x0020)
#define     TB_OUTMOD_TOGRES   (0x0040)
#define     TB_OUTMOD_SETRES   (0x0060)
#define     TB_OUTMOD_TOGGLE   (0x0080)
#define     TB_OUTMOD_RESET    (0x00A0)
#define     TB_OUTMOD_TOGSET   (0x00C0)
#define     TB_OUTMOD_RESSET   (0x00E0)
// Choose from above to configure Output Mode while on Compare Mode
#define     TB_OUTMOD0_CONF    TB_OUTMOD_BIT
#define     TB_OUTMOD1_CONF    TB_OUTMOD_BIT
#define     TB_OUTMOD2_CONF    TB_OUTMOD_BIT
#define     TB_OUTMOD3_CONF    TB_OUTMOD_BIT
#define     TB_OUTMOD4_CONF    TB_OUTMOD_BIT
#define     TB_OUTMOD5_CONF    TB_OUTMOD_BIT
#define     TB_OUTMOD6_CONF    TB_OUTMOD_BIT

#define     TB_CCIE_DISABLE    (0x0000)
#define     TB_CCIE_ENABLE     (0x0010)
// Choose from above to enable or disable Interrupt
#define     TB_CCIE0_CONF      TB_CCIE_DISABLE
#define     TB_CCIE1_CONF      TB_CCIE_DISABLE
#define     TB_CCIE2_CONF      TB_CCIE_DISABLE
#define     TB_CCIE3_CONF      TB_CCIE_DISABLE
#define     TB_CCIE4_CONF      TB_CCIE_DISABLE
#define     TB_CCIE5_CONF      TB_CCIE_DISABLE
#define     TB_CCIE6_CONF      TB_CCIE_ENABLE

#define     TB_OUT_LOW         (0x0000)
#define     TB_OUT_HIGH        (0x0004)
// Choose from above to choose output pin polarity
#define     TB_OUT0_CONF       TB_OUT_LOW
#define     TB_OUT1_CONF       TB_OUT_LOW
#define     TB_OUT2_CONF       TB_OUT_LOW
#define     TB_OUT3_CONF       TB_OUT_LOW
#define     TB_OUT4_CONF       TB_OUT_LOW
#define     TB_OUT5_CONF       TB_OUT_LOW
#define     TB_OUT6_CONF       TB_OUT_LOW
//***********************************************************************************
// Digital to Analog  Converter Control and Data Registers                          *
//                                                                                  *
// Configures the 8/12 bit Digital to Analog Converter                              *
//***********************************************************************************

#define     DAC12SREF_VREF  0x0000
#define     DAC12SREF_VEREF 0x6000
#define     DAC12SREF0_CONF  DAC12SREF_VEREF
#define     DAC12SREF1_CONF  DAC12SREF_VEREF

#define     DAC12RES_12     0x0000
#define     DAC12RES_8      0x1000
#define     DAC12RES0_CONF   DAC12RES_12
#define     DAC12RES1_CONF   DAC12RES_12

#define     DAC12LSEL_AUTO  0x0000
#define     DAC12LSEL_DATA  0x0400
#define     DAC12LSEL_RISEA 0x0800
#define     DAC12LSEL_RISEB 0x0C00
#define     DAC12LSEL0_CONF  DAC12LSEL_AUTO
#define     DAC12LSEL1_CONF  DAC12LSEL_AUTO

#define     DAC12IR_3X      0x0000
#define     DAC12IR_1X      0x0100
#define     DAC12IR0_CONF    DAC12IR_1X
#define     DAC12IR1_CONF    DAC12IR_1X

#define     DAC12AMP_OFFZ   0x0000
#define     DAC12AMP_OFF0   0x0020
#define     DAC12AMP_LOLO   0x0040
#define     DAC12AMP_LOMD   0x0030
#define     DAC12AMP_LOHI   0x0080
#define     DAC12AMP_MDMD   0x00A0
#define     DAC12AMP_MDHI   0x00C0
#define     DAC12AMP_HIHI   0x00E0
#define     DAC12AMP0_CONF   DAC12AMP_OFFZ
#define     DAC12AMP1_CONF   DAC12AMP_OFFZ

#define     DAC12DF_BIN     0x0000
#define     DAC12DF_2SC     0x0010
#define     DAC12DF0_CONF    DAC12DF_BIN
#define     DAC12DF1_CONF    DAC12DF_BIN

#define     DAC12ENC_DIS    0x0000
#define     DAC12ENC_ENA    0x0002
#define     DAC12ENC0_CONF   DAC12ENC_DIS
#define     DAC12ENC1_CONF   DAC12ENC_DIS

#define     DAC12GRP_NOT    0x0000
#define     DAC12GRP_GRP    0x0001
#define     DAC12GRP0_CONF   DAC12GRP_NOT
#define     DAC12GRP1_CONF   DAC12GRP_NOT

//***********************************************************************************
// Analog to Digital Converter Control Register 0                                   *
//                                                                                  *
// Configures the 8/12 bit Analog to Digital Converter                              *
//***********************************************************************************
#define     SHT1_04         0x0000
#define     SHT1_08         0x1000
#define     SHT1_16         0x2000
#define     SHT1_32         0x3000
#define     SHT1_64         0x4000
#define     SHT1_96         0x5000
#define     SHT1_128        0x6000
#define     SHT1_192        0x7000
#define     SHT1_256        0x8000
#define     SHT1_384        0x9000
#define     SHT1_512        0xA000
#define     SHT1_768        0xB000
#define     SHT1_1024_1     0xC000
#define     SHT1_1024_2     0xD000
#define     SHT1_1024_3     0xE000
#define     SHT1_1024_4     0xF000
#define     SHT1_CONF       SHT1_192

#define     SHT0_04         0x0000
#define     SHT0_08         0x0100
#define     SHT0_16         0x0200
#define     SHT0_32         0x0300
#define     SHT0_64         0x0400
#define     SHT0_96         0x0500
#define     SHT0_128        0x0600
#define     SHT0_192        0x0700
#define     SHT0_256        0x0800
#define     SHT0_384        0x0900
#define     SHT0_512        0x0A00
#define     SHT0_768        0x0B00
#define     SHT0_1024_1     0x0C00
#define     SHT0_1024_2     0x0D00
#define     SHT0_1024_3     0x0E00
#define     SHT0_1024_4     0x0F00
#define     SHT0_CONF       SHT0_192

#define     MSC_DISABLE     0x0000
#define     MSC_ENABLE      0x0080
#define     MSC_CONF        MSC_DISABLE

#define     REF2_5V_1P5V    0x0000
#define     REF2_5V_2P5V    0x0040
#define     REF2_5V_CONF    REF2_5V_2P5V

#define     REFON_DISABLE   0x0000
#define     REFON_ENABLE    0x0020
#define     REFON_CONF      REFON_ENABLE

#define     ADC12ON_DISABLE 0x0000
#define     ADC12ON_ENABLE  0x0010
#define     ADC12ON_CONF    ADC12ON_ENABLE

#define     ADC12OVIE_DISABLE   0x0000
#define     ADC12OVIE_ENABLE    0x0008
#define     ADC12OVIE_CONF      ADC12OVIE_DISABLE

#define     ADC12TOVIE_DISABLE  0x0000
#define     ADC12TOVIE_ENABLE   0x0004
#define     ADC12TOVIE_CONF     ADC12TOVIE_DISABLE

#define     ENC_DISABLE     0x0000
#define     ENC_ENABLE      0x0002
#define     ENC_CONF        ENC_DISABLE

#define     ADC12SC_DISABLE 0x0000
#define     ADC12SC_ENABLE  0x0001
#define     ADC12SC_CONF    ADC12SC_DISABLE

//***********************************************************************************
// Analog to Digital Converter Control Register 1                                   *
//                                                                                  *
// Configures the 8/12 bit Analog to Digital Converter                              *
//***********************************************************************************

#define     CSTARTADD_00    0x0000
#define     CSTARTADD_01    0x1000
#define     CSTARTADD_02    0x2000
#define     CSTARTADD_03    0x3000
#define     CSTARTADD_04    0x4000
#define     CSTARTADD_05    0x5000
#define     CSTARTADD_06    0x6000
#define     CSTARTADD_07    0x7000
#define     CSTARTADD_08    0x8000
#define     CSTARTADD_09    0x9000
#define     CSTARTADD_0A    0xA000
#define     CSTARTADD_0B    0xB000
#define     CSTARTADD_0C    0xC000
#define     CSTARTADD_0D    0xD000
#define     CSTARTADD_0E    0xE000
#define     CSTARTADD_0F    0xF000
#define     CSTARTADD_CONF  CSTARTADD_00

#define     SHS_ADC12SC     0x0000
#define     SHS_TAOUT1      0x0400
#define     SHS_TBOUT0      0x0800
#define     SHS_TBOUT1      0x0C00
#define     SHS_CONF        SHS_ADC12SC

#define     SHP_INPUT       0x0000
#define     SHP_TIMER       0x0200
#define     SHP_CONF        SHP_TIMER

#define     ISSH_NONINV     0x0000
#define     ISSH_INVERT     0x0100
#define     ISSH_CONF       ISSH_NONINV

#define     ADC12DIV_01       0x0000
#define     ADC12DIV_02       0x0020
#define     ADC12DIV_03       0x0040
#define     ADC12DIV_04       0x0060
#define     ADC12DIV_05       0x0080
#define     ADC12DIV_06       0x00A0
#define     ADC12DIV_07       0x00C0
#define     ADC12DIV_08       0x00E0
#define     ADC12DIV_CONF   ADC12DIV_01

#define     ADC12SSEL_OSC   0x0000
#define     ADC12SSEL_ACLK  0x0008
#define     ADC12SSEL_MCLK  0x0010
#define     ADC12SSEL_SMCLK 0x0018
#define     ADC12SSEL_CONF  ADC12SSEL_OSC

#define     CONSEQ_SINGLE   0x0000
#define     CONSEQ_SEQCHAN  0x0002
#define     CONSEQ_REPSIN   0x0004
#define     CONSEQ_REPSEQ   0x0006
#define     CONSEQ_CONF     CONSEQ_SINGLE

//***********************************************************************************
// Analog to Digital Converter Memory Control Register                              *
//                                                                                  *
// Configures the 8/12 bit Analog to Digital Converter                              *
//***********************************************************************************

#define     SREF_AVCC_AVSS  0x00
#define     SREF_VREF_AVSS  0x10
#define     SREF_VEREF_AVSS 0x20
#define     SREF_AVCC_VREF  0x40
#define     SREF_VREF_VREF  0x50
#define     SREF_VEREF_VREF 0x60
#define     SREF_CONF       SREF_AVCC_AVSS

#define     INCH_A0         0x00
#define     INCH_A1         0x01
#define     INCH_A2         0x02
#define     INCH_A3         0x03
#define     INCH_A4         0x04
#define     INCH_A5         0x05
#define     INCH_A6         0x06
#define     INCH_A7         0x07
#define     INCH_VEREFP     0x08
#define     INCH_VREFM      0x09
#define     INCH_TEMP       0x0A
#define     INCH_HALF       0x0B
#define     INCH_CONF       INCH_A0
