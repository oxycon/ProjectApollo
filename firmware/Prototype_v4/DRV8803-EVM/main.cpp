//******************************************************************************
//  LT Thomas
//  Texas Instruments Inc.
//  May 2011
//  Built with CCS4
//******************************************************************************
#include "Config.h"
#include "AppDef.h"

int TaskPointer;
int TStatePointer[8];
int TDelay[8];
int main()
{
int i;

// Basic Clock Control Registers
//DCOCTL = DCO_CONF + MOD_CONF;
DCOCTL = CALDCO_16MHZ;  
BCSCTL1 = CALBC1_16MHZ + XTS_CONF;//XT2OFF_CONF + XTS_CONF + DIVA_CONF + RSEL_CONF;
BCSCTL2 = SELM_CONF + DIVM_CONF + SELS_CONF + DIVS_CONF + DCOR_CONF;
BCSCTL3 = XT2S_CONF + LFXT1S_CONF + XCAP_CONF;
do
  {
  IFG1 &= ~OFIFG;                           // Clear OSCFault flag
  for (i = 0xFF; i > 0; i--);               // Time for flag to set
  }
while ((IFG1 & OFIFG));                   // OSCFault flag still set?

// Ports 1 through 6 Direction Select
P1DIR = (P1DIR7 << 7) + (P1DIR6 << 6) + (P1DIR5 << 5) + (P1DIR4 << 4) + (P1DIR3 << 3) + (P1DIR2 << 2) + (P1DIR1 << 1) + P1DIR0;
P2DIR = (P2DIR7 << 7) + (P2DIR6 << 6) + (P2DIR5 << 5) + (P2DIR4 << 4) + (P2DIR3 << 3) + (P2DIR2 << 2) + (P2DIR1 << 1) + P2DIR0;
P3DIR = (P3DIR7 << 7) + (P3DIR6 << 6) + (P3DIR5 << 5) + (P3DIR4 << 4) + (P3DIR3 << 3) + (P3DIR2 << 2) + (P3DIR1 << 1) + P3DIR0;
P4DIR = (P4DIR7 << 7) + (P4DIR6 << 6) + (P4DIR5 << 5) + (P4DIR4 << 4) + (P4DIR3 << 3) + (P4DIR2 << 2) + (P4DIR1 << 1) + P4DIR0;
P5DIR = (P5DIR7 << 7) + (P5DIR6 << 6) + (P5DIR5 << 5) + (P5DIR4 << 4) + (P5DIR3 << 3) + (P5DIR2 << 2) + (P5DIR1 << 1) + P5DIR0;
P6DIR = (P6DIR7 << 7) + (P6DIR6 << 6) + (P6DIR5 << 5) + (P6DIR4 << 4) + (P6DIR3 << 3) + (P6DIR2 << 2) + (P6DIR1 << 1) + P6DIR0;

// Ports 1 through 6 Pull Up/Down Enable/Disable Select
P1REN = (P1REN7 << 7) + (P1REN6 << 6) + (P1REN5 << 5) + (P1REN4 << 4) + (P1REN3 << 3) + (P1REN2 << 2) + (P1REN1 << 1) + P1REN0;
P2REN = (P2REN7 << 7) + (P2REN6 << 6) + (P2REN5 << 5) + (P2REN4 << 4) + (P2REN3 << 3) + (P2REN2 << 2) + (P2REN1 << 1) + P2REN0;
P3REN = (P3REN7 << 7) + (P3REN6 << 6) + (P3REN5 << 5) + (P3REN4 << 4) + (P3REN3 << 3) + (P3REN2 << 2) + (P3REN1 << 1) + P3REN0;
P4REN = (P4REN7 << 7) + (P4REN6 << 6) + (P4REN5 << 5) + (P4REN4 << 4) + (P4REN3 << 3) + (P4REN2 << 2) + (P4REN1 << 1) + P4REN0;
P5REN = (P5REN7 << 7) + (P5REN6 << 6) + (P5REN5 << 5) + (P5REN4 << 4) + (P5REN3 << 3) + (P5REN2 << 2) + (P5REN1 << 1) + P5REN0;
P6REN = (P6REN7 << 7) + (P6REN6 << 6) + (P6REN5 << 5) + (P6REN4 << 4) + (P6REN3 << 3) + (P6REN2 << 2) + (P6REN1 << 1) + P6REN0;

// Ports 1 through 6 Alternate Peripheral Function Enable
P1SEL = (P1SEL7 << 7) + (P1SEL6 << 6) + (P1SEL5 << 5) + (P1SEL4 << 4) + (P1SEL3 << 3) + (P1SEL2 << 2) + (P1SEL1 << 1) + P1SEL0;
P2SEL = (P2SEL7 << 7) + (P2SEL6 << 6) + (P2SEL5 << 5) + (P2SEL4 << 4) + (P2SEL3 << 3) + (P2SEL2 << 2) + (P2SEL1 << 1) + P2SEL0;
P3SEL = (P3SEL7 << 7) + (P3SEL6 << 6) + (P3SEL5 << 5) + (P3SEL4 << 4) + (P3SEL3 << 3) + (P3SEL2 << 2) + (P3SEL1 << 1) + P3SEL0;
P4SEL = (P4SEL7 << 7) + (P4SEL6 << 6) + (P4SEL5 << 5) + (P4SEL4 << 4) + (P4SEL3 << 3) + (P4SEL2 << 2) + (P4SEL1 << 1) + P4SEL0;
P5SEL = (P5SEL7 << 7) + (P5SEL6 << 6) + (P5SEL5 << 5) + (P5SEL4 << 4) + (P5SEL3 << 3) + (P5SEL2 << 2) + (P5SEL1 << 1) + P5SEL0;
P6SEL = (P6SEL7 << 7) + (P6SEL6 << 6) + (P6SEL5 << 5) + (P6SEL4 << 4) + (P6SEL3 << 3) + (P6SEL2 << 2) + (P6SEL1 << 1) + P6SEL0;

// Ports 1 and 2 Pin On Change Interrupt Enable
P1IE = (P1IE7 << 7) + (P1IE6 << 6) + (P1IE5 << 5) + (P1IE4 << 4) + (P1IE3 << 3) + (P1IE2 << 2) + (P1IE1 << 1) + P1IE0;
P2IE = (P2IE7 << 7) + (P2IE6 << 6) + (P2IE5 << 5) + (P2IE4 << 4) + (P2IE3 << 3) + (P2IE2 << 2) + (P2IE1 << 1) + P2IE0;
P1IES = (P1IES7 << 7) + (P1IES6 << 6) + (P1IES5 << 5) + (P1IES4 << 4) + (P1IES3 << 3) + (P1IES2 << 2) + (P1IES1 << 1) + P1IES0;
P2IES = (P2IES7 << 7) + (P2IES6 << 6) + (P2IES5 << 5) + (P2IES4 << 4) + (P2IES3 << 3) + (P2IES2 << 2) + (P2IES1 << 1) + P2IES0;

P1IFG = 0;
P2IFG = 0;
IFG2 &= ~UCA0TXIFG;

//USCI
UCA0CTL0 = (UCPENEN & UCPEN) + (UCPAREN & UCPAR) + (UCMSBEN & UCMSB) + (UC7BITEN & UC7BIT) + (UCSPBEN & UCSPB) + (UCMODE0CONF) + (UCSYNCEN & UCSYNC); 
UCA0CTL1 = (UCSSEL0CONF) + (UCRXEIEEN & UCRXEIE) + (UCBRKIEEN & UCBRKIE) + (UCDORMEN & UCDORM) + (UCTXADDREN & UCTXADDR) + (UCTXBRKEN & UCTXBRK) + (UCSWRSTEN & UCSWRST); 

UCA0MCTL = UCBRF0CONF + UCBRSCONF + (UCOS16EN & UCOS16);

UCA0BR0 = BAUDRATE0_LO;
UCA0BR1 = BAUDRATE0_HI;


// Interrupt Enable Register 1 and 2
IE1 = (WDTIEEN & WDTIE) + (OFIEEN & OFIE) + (NMIIEEN & NMIIE) + (ACCVIEEN & ACCVIE);
IE2 = (UCA0RXIEEN & UCA0RXIE) + (UCA0TXIEEN & UCA0TXIE) + (UCB0RXIEEN & UCB0RXIE) + (UCB0RXIEEN & UCB0RXIE);

// Watchdog Timer Control Register
WDTCTL = WDTPW + WDTHOLD_CONF + WDTNMIES_CONF + WDTNMI_CONF + WDTTMSEL_CONF + WDTCNTCL_CONF + WDTSSEL_CONF + WDTIS_CONF;

// Timer A Configuration
TACTL = TASSEL_CONF + TA_ID_CONF + TAIE_CONF; //TA_MC_CONF occurs below in order to start Tasks after Init
TACCTL0 = TA_CM0_CONF + TA_CCIS0_CONF + TA_SCS0_CONF + TA_CAP0_CONF + TA_OUTMOD0_CONF + TA_CCIE0_CONF + TA_OUT0_CONF;
TACCTL1 = TA_CM1_CONF + TA_CCIS1_CONF + TA_SCS1_CONF + TA_CAP1_CONF + TA_OUTMOD1_CONF + TA_CCIE1_CONF + TA_OUT1_CONF;
TACCTL2 = TA_CM2_CONF + TA_CCIS2_CONF + TA_SCS2_CONF + TA_CAP2_CONF + TA_OUTMOD2_CONF + TA_CCIE2_CONF + TA_OUT2_CONF;

// Timer B Configuration
TBCTL = TBCLGRP_CONF + TB_CNTL_CONF + TBSSEL_CONF + TB_ID_CONF; // TB_MC_CONF occurs below in order to start Tasks after Init
TBCCTL0 = TB_CM0_CONF + TB_CCIS0_CONF + TB_SCS0_CONF + TB_CLLD0_CONF + TB_CAP0_CONF + TB_OUTMOD0_CONF + TB_CCIE0_CONF + TB_OUT0_CONF;
TBCCTL1 = TB_CM1_CONF + TB_CCIS1_CONF + TB_SCS1_CONF + TB_CLLD1_CONF + TB_CAP1_CONF + TB_OUTMOD1_CONF + TB_CCIE1_CONF + TB_OUT1_CONF;
TBCCTL2 = TB_CM2_CONF + TB_CCIS2_CONF + TB_SCS2_CONF + TB_CLLD2_CONF + TB_CAP2_CONF + TB_OUTMOD2_CONF + TB_CCIE2_CONF + TB_OUT2_CONF;
TBCCTL3 = TB_CM3_CONF + TB_CCIS3_CONF + TB_SCS3_CONF + TB_CLLD3_CONF + TB_CAP3_CONF + TB_OUTMOD3_CONF + TB_CCIE3_CONF + TB_OUT3_CONF;
TBCCTL4 = TB_CM4_CONF + TB_CCIS4_CONF + TB_SCS4_CONF + TB_CLLD4_CONF + TB_CAP4_CONF + TB_OUTMOD4_CONF + TB_CCIE4_CONF + TB_OUT4_CONF;
TBCCTL5 = TB_CM5_CONF + TB_CCIS5_CONF + TB_SCS5_CONF + TB_CLLD5_CONF + TB_CAP5_CONF + TB_OUTMOD5_CONF + TB_CCIE5_CONF + TB_OUT5_CONF;
TBCCTL6 = TB_CM6_CONF + TB_CCIS6_CONF + TB_SCS6_CONF + TB_CLLD6_CONF + TB_CAP6_CONF + TB_OUTMOD6_CONF + TB_CCIE6_CONF + TB_OUT6_CONF;

// Digital to Analog Converter Configuration
DAC12_0CTL = DAC12SREF0_CONF  + DAC12RES0_CONF + DAC12LSEL0_CONF  + DAC12IR0_CONF + DAC12AMP0_CONF + DAC12DF0_CONF + DAC12ENC0_CONF + DAC12GRP0_CONF;
DAC12_1CTL = DAC12SREF1_CONF  + DAC12RES1_CONF + DAC12LSEL1_CONF  + DAC12IR1_CONF + DAC12AMP1_CONF + DAC12DF1_CONF + DAC12ENC1_CONF + DAC12GRP1_CONF;

// Analog to Digital Converter Configuration
ADC12CTL0 = SHT1_CONF + SHT0_CONF + MSC_CONF + REF2_5V_CONF + REFON_CONF + ADC12ON_CONF + ADC12OVIE_CONF + ADC12TOVIE_CONF + ENC_CONF + ADC12SC_CONF;
ADC12CTL1 = CSTARTADD_CONF + SHS_CONF + SHP_CONF + ISSH_CONF + ADC12DIV_CONF + ADC12SSEL_CONF + CONSEQ_CONF;
ADC12MCTL0 = SREF_CONF + INCH_CONF;

//RTOS Specific Initialization
TACCR0 = TASK_TIME;

TaskPointer = 0;
SerialPointer = 0;
TACTL |= TA_MC_CONF;                        // Timer A Counter Starts as configured
TBCTL |= TB_MC_CONF;


//Application Specific Initialization
AppInit();

__bis_SR_register(GIE);                     // Enable all Interrupts

switch (P3IN & (BIT1 + BIT0))
    {
    case (DRV8803): //DRV8803 Parallel Mode PIN3[1:0] = 0b00
          P4SEL = 0x1E;
          P4DIR = 0xBF;
          Device = DRV8803;
        break;
    case (DRV8804): //DRV8804 Serial Mode PIN3[1:0] = 0b10
          P4SEL = 0x00;
          P4DIR = 0xBF;
          Device = DRV8804;
        break;
    case (DRV8805): //DRV8805 Indexer Mode PIN3[1:0] = 0b01
          P4SEL = 0x02;
          P4DIR = 0xBF;
          Device = DRV8805;
        break;
    case (DRV8806): //DRV8806 Serial Mode PIN3[1:0] = 0b11
          P4SEL = 0x00;
          P4DIR = 0xBF;
          Device = DRV8806;
        break;

    }
    
while (1)
  {
  _BIS_SR(CPUOFF);                        // Enter LPM0
  switch (TaskPointer)
    {
    case 0:
      Task0();
    break;

    case 1:
      Task1();
    break;

    case 2:
      Task2();
    break;

    case 3:
      Task3();
    break;

    case 4:
      Task4();
    break;

    case 5:
      Task5();
    break;

    case 6:
      Task6();
    break;

    case 7:
      Task7();
      TaskPointer = -1;
    break;
    
    default:
    break;
    }
  TaskPointer += 1;
}

}

#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)
{
    TACCR0 += TASK_TIME;
    _BIC_SR_IRQ(CPUOFF);                   // Clear LPM0
    CommTimeOut += 1;
}
