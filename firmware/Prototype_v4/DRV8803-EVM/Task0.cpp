//******************************************************************************
//  LT Thomas
//  Texas Instruments Inc.
//  May 2011
//  Built with CCS4
//******************************************************************************

#include "Config.h"

char StatusLED;
unsigned int count, FrequencyB, FrequencyA, TmpFrequency, AccelTimeBase, AccelPPS;
char StepperIndex, PhaseDutyCycle, Device, Speed;
bool StepperDirection;

void Task0(void)
{
if (MessageComplete)
    {
      SerialOutBuffer[0] = FIRMWARE_REVISION;
      SerialOutBuffer[1] = 0;
      SerialOutBuffer[2] = 0;
      switch(OPCODE)
      {

// Write GPIO Data [ OPCODE = 0 ] [ GPIO DATA ] [ Not Used ] [ Not Used ] [ Not Used ]
// GPIO DATA = P13/P12/P11/P10/P47/P46/P45/P44
      case (WRITE_GPIO):
        P4OUT = SerialBuffer[1];
        SerialOutBuffer[1] = ~SerialBuffer[1];
      break;

// Pulse Timer Output [ OPCODE = 0x1 ] [Timer Used] [ Pulse Length Hi ] [ Pulse Length Lo ] [ Not Used ]
      case (PULSE_TMR):
        TBCTL &= ~TB_CNTL_08;                //Configure Timer to run up to FFFF (16 bit FRC)
        TBCCTL1 &= 0xFF1F;                                          // Clear OUTMODx bits; 3 MSB on lower byte; Timer configured to output mode
        TBCCTL1 |= TB_OUT_HIGH;                                     //Set the output (pulse start)
        TBCCTL1 |= TB_OUTMOD_RESET;                                 //Configure the timer to reset
        TBCCR1 = TBR + ((SerialBuffer[2] * 256) + SerialBuffer[3]); //Configure the pulse reset time (pulse end)
      break;
      
// Set Timer Frequency [ OPCODE = 0x2 ] [ Frequency Hi  ] [ Frequency Lo ] [ ] [ ] 
      case (FREQ_TMR):
      	if(Device == DRV8805)
      	{
	        TBCTL &= ~TB_CNTL_08;                //Configure Timer to run up to FFFF (16 bit FRC)
	        TBCCTL1 &= 0xFF1F;                                          // Clear OUTMODx bits; 3 MSB on lower byte; Timer configured to output mode
	        TBCCTL1 |= TB_OUTMOD_TOGGLE + CCIE;                         //Configure the timer to reset
	        FrequencyB = 4*((SerialBuffer[1] * 256) + SerialBuffer[2]);   
	        FrequencyB = (2000000/FrequencyB);							//Configure the Frequency Rate	        
	        if( TmpFrequency < FrequencyB)
	        {
	        	Speed = ACCEL;
	        }
	        else
	        {
	        	Speed = DCCEL;
	        }
	        TBCCR1 = TmpFrequency;
	        
	        TBCCTL5 &= 0xFF1F;
	        TBCCR5 = 850;//3277; // 1ms for timebase counter   
	        TBCCTL5 |= CCIE;
	        
      	}
      	break;
      
// Disable Timer Frequency [ OPCODE = 0x3 ] [ Timer # ] [ Not Used ] [ Not Used ] [ Not Used ]
      case (FREQ_TMR_STOP):
        TmpFrequency = 0xFFFF;									  //Set 0 Pulses Per Second
        TBCCTL5 &= ~CCIE; 
        TBCCTL1 &= 0xFF0F;                                        //Disable CC and Interrupt
       break;

// Send Universal SPI [Opcode = 0x4 ] [data]
//[ Data ] [ Data] [ Not Used ] [ Not Used ]
      case (UNIV_SPI):
      
      Send_SPI_Data(SerialBuffer[1]);

      break;

// Configure Device [ OPCODE = 0x5 ] [ Timer # ] [ Not Used ] [ Not Used ] [ Not Used ]
// Used to select Device and clear Interrups
      case (CONFIG_DEVICE):

        TACCTL1 &= ~CCIE;                     //Disable and Clear All Interrupt
        TACCTL2 &= ~CCIE;					
      	TBCCTL0 &= ~CCIE;  	
      	TBCCTL1 &= ~CCIE;
        TBCCTL2 &= ~CCIE;
        TBCCTL3 &= ~CCIE;
        TBCCTL4 &= ~CCIE;
        TBCCTL5 &= ~CCIE;
        TBCCTL1 &= 0xFF1F;
        TBCCTL2 &= 0xFF1F;
        TBCCTL3 &= 0xFF1F;
        TBCCTL4 &= 0xFF1F;
	TBCCTL5 &= 0xFF1F;
		
        TmpFrequency = 0xFFFF;	              //Set 0 Pulses Per Second  
        AccelPPS = 1;			      //Reset Speed
        AccelTimeBase = 1;                                  
        P4OUT = 0;			      //Clear outputs
        P4OUT |= 0x20;			      //Disable Motor

        switch (SerialBuffer[1])
        {
        case 0:     //DRV8803 Parallel Mode
          P4SEL = 0x1E;
          P4DIR = 0xBF;
          Device = DRV8803;
          break;
        case 1:     //DRV8804 Serial Mode
          P4SEL = 0x00;
          P4DIR = 0xBF;
          Device = DRV8804;
          Send_SPI_Data(0);
          break;
        case 2:     //DRV8805 Indexer Mode
          P4SEL = 0x02;
          P4DIR = 0xBF;
          Device = DRV8805;
          break;
        case 3:     //DRV8806 Serial Mode
          P4SEL = 0x00;
          P4DIR = 0xBF;
          Device = DRV8804;
          Send_SPI_Data(0);
          break;          
        }

        break;
// SET Timer Out [ OPCODE = 0x6 ] [ Timer # ] [ Not Used ] [ Not Used ] [ Not Used ]
      case (SET_TMR_OUT):
        switch (SerialBuffer[1])
        {
        case 0:
            TBCCTL0 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
            TBCCTL0 |= BIT2;                  // Set OUT
          break;
        case 1:
            TBCCTL1 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
            TBCCTL1 |= BIT2;                  // Set OUT
          break;
        case 2:
            TBCCTL2 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
            TBCCTL2 |= BIT2;                  // Set OUT
          break;
        case 3:
            TBCCTL3 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
            TBCCTL3 |= BIT2;                  // Set OUT
          break;
        case 4:
            TBCCTL4 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
            TBCCTL4 |= BIT2;                  // Set OUT
          break;
        case 5:
            TBCCTL5 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
            TBCCTL5 |= BIT2;                  // Set OUT
          break;
        case 6:
            TBCCTL6 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
            TBCCTL6 |= BIT2;                  // Set OUT
          break;
        }
        break;

// Enable PWM [ OPCODE = 0x07 ] [ Timer # ] [ Duty Cycle ] [ Not Used ] [ Not Used ]
      case (ENABLE_PWM):
        TBCTL |= TB_CNTL_08;                //Configure Timer to run up to FF (8 bit FRC)
        int tempOut;
        
        switch (SerialBuffer[1])
        {
        case 0:
            TBCCR0 = SerialBuffer[2];
            tempOut = TBCCTL0;
            tempOut &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
            tempOut |= TB_OUTMOD_RESSET;
            TBCCTL0 = tempOut;
        break;
        case 1:
            TBCCR1 = SerialBuffer[2];
            tempOut = TBCCTL1;
            tempOut &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
            tempOut |= TB_OUTMOD_RESSET;
            TBCCTL1 = tempOut;
        break;
        case 2:
            TBCCR2 = SerialBuffer[2];
            tempOut = TBCCTL2;
            tempOut &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
            tempOut |= TB_OUTMOD_RESSET;
            TBCCTL2 = tempOut;
        break;
        case 3:
            TBCCR3 = SerialBuffer[2];
            tempOut = TBCCTL3;
            tempOut &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
            tempOut |= TB_OUTMOD_RESSET;
            TBCCTL3 = tempOut;
        break;
        case 4:
            TBCCR4 = SerialBuffer[2];
            tempOut = TBCCTL4;
            tempOut &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
            tempOut |= TB_OUTMOD_RESSET;
            TBCCTL4 = tempOut;
        break;
        
        }
      break;
// Disable PWM [ OPCODE = 0x08 ] [ Timer # ] [ Not Used ] [ Not Used ] [ Not Used ]
      case (DISABLE_PWM):

        //(TBCCTL0 + SerialBuffer[1]) &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
        switch (SerialBuffer[1])
        {
        case 0:
            TBCCTL0 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
          break;
        case 1:
            TBCCTL1 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
          break;
        case 2:
            TBCCTL2 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
          break;
        case 3:
            TBCCTL3 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
          break;
        case 4:
            TBCCTL4 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
          break;
        case 5:
            TBCCTL5 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
          break;
        case 6:
            TBCCTL6 &= 0xFF1B;                // Clear OUTMODx bits and OUT bit; Configures the PWM output to OUTx value, in this case 0
          break;
        }
      break;
// Enable PWM [ OPCODE = 0x09 ] [ Timer # ] [ Duty Cycle ] [ Not Used ] [ Not Used ]
      case (UPDATE_PWM):
        switch (SerialBuffer[1])
        {
        case 0:
            TBCCR0 = SerialBuffer[2];
        break;
        case 1:
            TBCCR1 = SerialBuffer[2];
        break;
        case 2:
            TBCCR2 = SerialBuffer[2];
        break;
        case 3:
            TBCCR3 = SerialBuffer[2];
        break;
        case 4:
            TBCCR4 = SerialBuffer[2];
        break;
        case 5:
            TBCCR5 = SerialBuffer[2];
        break;
        case 6:
            TBCCR6 = SerialBuffer[2];
        break;
        }
            break;

// Parallel Stepper [ OPCODE = 0xA ] [ FREQ HI ] [ FREQ LO ] [ DutyCycle ] [ Direction ]
      case (PARALLEL_STEPPER):
        TACCTL1 &= 0xFF1F;                     // Clear OUTMODx bits; 3 MSB on lower byte; Timer configured to output mode
        TACCTL1 |= CCIE;                       //Enable Interrupt
        FrequencyA = (SerialBuffer[1] * 256) + SerialBuffer[2];   //Configure the Frequency Rate
        FrequencyA = 2000000/FrequencyA;
        TACCR1 = TmpFrequency;
        
        if( TmpFrequency < FrequencyA)
        {
        	Speed = ACCEL;
        }
        else
        {
        	Speed = DCCEL;
        }
		
		TBCTL |= TB_CNTL_08;              //Configure Timer to run up to FF (8 bit FRC)
		 
        TBCCR1 = SerialBuffer[3];
        TBCCTL1 &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte

        TBCCR2 = SerialBuffer[3];
        TBCCTL2 &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte

        TBCCR3 = SerialBuffer[3];
        TBCCTL3 &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte

        TBCCR4 = SerialBuffer[3];
        TBCCTL4 &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
        
        TBCCR5 = 33;//3277; // 1ms for timebase counter   
        TBCCTL5 &= 0xFF1F;
        TBCCTL5 |= CCIE;

        StepperDirection = SerialBuffer[4];
        StepperIndex = 0;
      break;        

// Serial Stepper [ OPCODE = 0xB ] [ FREQ HI ] [ FREQ LO ] [ DutyCycle ] [ Direction ]      
	  case (SERIAL_STEPPER):
	    TACCTL2 &= 0xFF1F;                     // Clear OUTMODx bits; 3 MSB on lower byte; Timer configured to output mode
        TACCTL2 |= CCIE;                       //Enable Interrupt
        FrequencyA = (SerialBuffer[1] * 256) + SerialBuffer[2];   //Configure the Frequency Rate
        FrequencyA = 2000000/FrequencyA;
        TACCR2 = TmpFrequency;
        
        if( TmpFrequency < FrequencyA)
        {
        	Speed = ACCEL;
        }
        else
        {
        	Speed = DCCEL;
        }
		
		TBCTL |= TB_CNTL_08;                //Configure Timer to run up to FF (8 bit FRC)
		TBCCTL0 |= CCIE;
		
		if(SerialBuffer[3] == 0xFF)			//If 100% Duty Cycle then do not use PWM
			{FrequencyB = 0;}
		else
			{FrequencyB = SerialBuffer[3];}
		
        TBCCR1 = FrequencyB;
        TBCCTL1 &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
		
		
        TBCCR2 = FrequencyB;
        TBCCTL2 &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
		
		
        TBCCR3 = FrequencyB;
        TBCCTL3 &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
		
		
        TBCCR4 = FrequencyB;
        TBCCTL4 &= 0xFF1F;                // Clear OUTMODx bits; 3 MSB on lower byte
		
		TBCCR5 = 33;//3277; // 1ms for timebase counter   
        TBCCTL5 &= 0xFF1F;
        TBCCTL5 |= CCIE;
		
        StepperDirection = SerialBuffer[4];
        StepperIndex = 0;
        break;
        
// Parallel Stepper STOP [ OPCODE = 0xC ] [ FREQ HI ] [ FREQ LO ] [ DutyCycle ] [ Direction ]
      case (PARALLEL_STEPPER_STOP):
        TACCTL1 &= ~CCIE;                       //Disable Interrupt
        TmpFrequency = 0xFFFF;					//Set 0 Pulses Per Second
        TBCCTL5 &= ~CCIE;
      break;       
      
 // Serial Stepper STOP  [ OPCODE = 0xD ] [ FREQ HI ] [ FREQ LO ] [ DutyCycle ] [ Direction ]  
      case (SERIAL_STEPPER_STOP):
        TACCTL2 &= ~CCIE;						//Disable Interrupts 
      	TBCCTL0 &= ~CCIE;  	
      	TBCCTL1 &= ~CCIE;
        TBCCTL2 &= ~CCIE;
        TBCCTL3 &= ~CCIE;
        TBCCTL4 &= ~CCIE;
        TBCCTL5 &= ~CCIE;
        TBCCTL1 &= 0xFF1F;
        TBCCTL2 &= 0xFF1F;
        TBCCTL3 &= 0xFF1F;
        TBCCTL4 &= 0xFF1F;
        Send_SPI_Data(0);
        TmpFrequency = 0xFFFF;					//Set 0 Pulses Per Second
      	break;
 // Speed control  [ OPCODE = 0xE ] [ pps ] [ time base ]  [ ] [ ]     
      case (WRITE_ACCEL):
        AccelPPS = SerialBuffer[1];							//Configure the Pulses Per Second Acceleration Rate
        AccelTimeBase = SerialBuffer[2];					//Configure The Acceleration Time Base
      break;

 // Read Back Diagnostics[ OPCODE = 0x0F ] [SPI Data ] [Not Used ] [Not Used ] [Not Used ]     
      case (READ_DIAG):
        
      int i, SPIData;
      SPIData = SerialBuffer[1];
      P4OUT &= ~SPI_LATCH;

      //************************************************************************
      // Sending Data
      //************************************************************************
      for (i = 0; i<4;i++)
        {
        P4OUT &= ~SPI_CLK;                                // CLK starts low. 
        if (SPIData & 0x0001)                            // Determine if MSBData Bit is High or Low
          {
          P4OUT |= SPI_MOSI;                            // Write a 1
          }
        else
          {
          P4OUT &= ~SPI_MOSI;                           // Write a 0
          }

        P4OUT |= SPI_CLK;                                 //The clock goes HI. Data is latched
        SPIData >>= 1;                                   // Shift Right SPI Data Out Register (MOSI)
        }
            
        P4OUT |= SPI_LATCH;
        //P4OUT &= ~SPI_CLK;                                // CLK starts low. 

      //************************************************************************
      // Receiving Data
      //************************************************************************

      for (i=0; i<45; i++)
      
      SerialOutBuffer[1] = 0;
      P4OUT |= SPI_CLK;                                 //The clock goes HI.
      for (i = 0; i<4;i++)
        {
        SerialOutBuffer[1] <<= 1;
        P4OUT &= ~SPI_CLK;                                //The clock goes LO.         
        if (P4IN & SPI_MISO)
          {
          SerialOutBuffer[1] |= BIT0;                   // Write a 1
          }
        else
          {
          SerialOutBuffer[1] &= ~BIT0;                   // Write a 0
          }

        P4OUT |= SPI_CLK;                                 //The clock goes HI.
        }
        P4OUT &= ~SPI_CLK;                                //The clock goes LO.         
        
      break;            
            
// Read Memory [Opcode = 0xE0 ] [ Address Hi ] [ Address Lo ] [ Not Used ] [ Not Used ]
      case (READ_MEM):
       int * MyPointer;
       int Address;

       Address = (SerialBuffer[1]*256 + SerialBuffer[2]);
       MyPointer = (int *) Address;
       SerialOutBuffer[1] = (*MyPointer & 0xFF00) >> 8;
       SerialOutBuffer[2] = (*MyPointer & 0xFF);
      break;
// Write Memory [Opcode = 0xE1 ] [ Address Hi ] [ Address Lo ] [ Data Hi ] [ Data Lo ]
      case (WRITE_WMEM):
        int Data;

        Address = (SerialBuffer[1]*256 + SerialBuffer[2]);
        Data = (SerialBuffer[3]*256 + SerialBuffer[4]);
        MyPointer = (int *) Address;
        *MyPointer = Data;
        break;

// Write Byte Memory [Opcode = 0xE2 ] [ Address ] [ Address Lo ] [ Data ] [Not Used ]
      case (WRITE_BMEM):
        char BData;
        char * MyPtr;

        Address = (SerialBuffer[1]*256 + SerialBuffer[2]);
        BData = SerialBuffer[3];
        MyPtr = (char *) Address;
        *MyPtr = BData;
        break;

      case (RESET_MCU):
        WDTCTL = 0x00;
        break;
      case (SHOW_CLKS):
        P5SEL |= SerialBuffer[1] & 0xF0;
        break;
      }
    MessageComplete = false;
    SerialOutPointer = 0;
    UCA0TXBUF = SerialOutBuffer[SerialOutPointer];
    IE2 |= UCA0TXIE;
    }
}





