//******************************************************************************
//  LT Thomas
//  Texas Instruments Inc.
//  May 2011
//  Built with CCS4
//******************************************************************************

#include "Config.h"

int DebugVar;

/************************************************************
* Interrupt Vectors (offset from 0xFFC0)
*************************************************************
#define DAC12_VECTOR        (14 * 2u) 0xFFDC DAC12 
#define DMA_VECTOR          (15 * 2u) 0xFFDE DMA
#define USCIAB1TX_VECTOR    (16 * 2u) 0xFFE0 USCI A1/B1 Transmit
#define USCIAB1RX_VECTOR    (17 * 2u) 0xFFE2 USCI A1/B1 Receive
#define PORT1_VECTOR        (18 * 2u) 0xFFE4 Port 1
#define PORT2_VECTOR        (19 * 2u) 0xFFE6 Port 2
#define ADC12_VECTOR        (21 * 2u) 0xFFEA ADC
#define USCIAB0TX_VECTOR    (22 * 2u) 0xFFEC USCI A0/B0 Transmit
#define USCIAB0RX_VECTOR    (23 * 2u) 0xFFEE USCI A0/B0 Receive
#define TIMERA1_VECTOR      (24 * 2u) 0xFFF0 Timer A CC1-2, TA
#define TIMERA0_VECTOR      (25 * 2u) 0xFFF2 Timer A CC0
#define WDT_VECTOR          (26 * 2u) 0xFFF4 Watchdog Timer
#define COMPARATORA_VECTOR  (27 * 2u) 0xFFF6 Comparator A
#define TIMERB1_VECTOR      (28 * 2u) 0xFFF8 Timer B CC1-6, TB
#define TIMERB0_VECTOR      (29 * 2u) 0xFFFA Timer B CC0
#define NMI_VECTOR          (30 * 2u) 0xFFFC Non-maskable
#define RESET_VECTOR        (31 * 2u) 0xFFFE Reset [Highest Priority]*/

#pragma vector=DAC12_VECTOR
__interrupt void DigitaltoAnalogConverter(void)
{

}

#pragma vector=DMA_VECTOR
__interrupt void DigitalToAnalog_DMA(void)
{

}

#pragma vector=USCIAB1TX_VECTOR
__interrupt void USCIAB1_Transmit(void)
{

}

#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCIAB1_Receive(void)
{

}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_Change(void)
{

}

#pragma vector=PORT2_VECTOR
__interrupt void Port2_Change(void)
{

}

#pragma vector=ADC12_VECTOR
__interrupt void AnalogToDigitalConverter(void)
{

}

#pragma vector=TIMERA1_VECTOR
__interrupt void TimerA1(void)
{

	switch (TAIV)
		{
	case TACCR1_CCIFG_SET:					//DRV8803 main interrupt
			switch(Speed)
			{
				case ACCEL:
					if(TmpFrequency < FrequencyA)
					{
					TACCR1 += TmpFrequency;
					}
					else
					{
			    	TACCR1 += FrequencyA;
					}
				break;
				case DCCEL:
					if(TmpFrequency > FrequencyA)
					{
					TACCR1 += TmpFrequency;
					}
					else
					{
			    	TACCR1 += FrequencyA;
					}
				break;
			}
	    switch (StepperIndex)
	    {
	    
	    case (0):
	      TBCCTL1 |= TB_OUTMOD_RESSET;
	      TBCCTL2 &= 0xFF1B;
	      TBCCTL3 &= 0xFF1B;
	      TBCCTL4 &= 0xFF1B;      
	      break;
	    case (1):
	      TBCCTL1 &= 0xFF1B;
	      TBCCTL2 |= TB_OUTMOD_RESSET;
	      TBCCTL3 &= 0xFF1B;
	      TBCCTL4 &= 0xFF1B;      
	      break;
	    case (2):
	      TBCCTL1 &= 0xFF1B;
	      TBCCTL2 &= 0xFF1B;
	      TBCCTL3 |= TB_OUTMOD_RESSET;
	      TBCCTL4 &= 0xFF1B;      
	      break;
	    case (3):
	      TBCCTL1 &= 0xFF1B;
	      TBCCTL2 &= 0xFF1B;
	      TBCCTL3 &= 0xFF1B;      
	      TBCCTL4 |= TB_OUTMOD_RESSET;
	      break;
	    }
	  if (StepperDirection)
	    {
	      StepperIndex += 1;
	    }
	  else
	    {
	      StepperIndex -= 1;
	    }
	  StepperIndex &= 0x03; //2 MSB bits used only
	    break;
	    
	case TACCR2_CCIFG_SET:					//DRV8804 main interrupt
	
		switch(Speed)
			{
				case ACCEL:
					if(TmpFrequency < FrequencyA)
					{
					TACCR2 += TmpFrequency;
					}
					else
					{
			    	TACCR2 += FrequencyA;
					}
				break;
				case DCCEL:
					if(TmpFrequency > FrequencyA)
					{
					TACCR2 += TmpFrequency;
					}
					else
					{
			    	TACCR2 += FrequencyA;
					}
				break;
			}
	    switch (StepperIndex)
	    {
	    case (0):
	      TBCCTL1 |= CCIE;
	      TBCCTL2 &= ~CCIE;
	      TBCCTL3 &= ~CCIE;
	      TBCCTL4 &= ~CCIE;     
	      break;
	    case (1):
	      TBCCTL1 &= ~CCIE;
	      TBCCTL2 |= CCIE;
	      TBCCTL3 &= ~CCIE;
	      TBCCTL4 &= ~CCIE;  	        
	      break;
	    case (2):
	      TBCCTL1 &= ~CCIE;
	      TBCCTL2 &= ~CCIE;
	      TBCCTL3 |= CCIE;
	      TBCCTL4 &= ~CCIE;  
	      break;
	    case (3):
	      TBCCTL1 &= ~CCIE;
	      TBCCTL2 &= ~CCIE;
	      TBCCTL3 &= ~CCIE;      
	      TBCCTL4 |= CCIE;
	      break;
	      }
	  if (StepperDirection)
	    {
	      StepperIndex += 1;
	    }
	  else
	    {
	      StepperIndex -= 1;
	    }
	  StepperIndex &= 0x03; 			//2 MSB bits used only
	  
	  break;
	  
	case TAIFG_SET:
	  break;
	}

}

#pragma vector=WDT_VECTOR
__interrupt void Watchdog_Timer(void)
{

}

#pragma vector=COMPARATORA_VECTOR
__interrupt void ComparatorA(void)
{

}

#pragma vector=TIMERB1_VECTOR
__interrupt void TimerB1(void)
{
switch (TBIV)
{
case TBCCR1_CCIFG_SET:					//DRV8805 main interrupt
	switch(Device)
	{
		case DRV8805:
			switch(Speed)
			{
				case ACCEL:
					if(TmpFrequency < FrequencyB)
					{
					TBCCR1 += TmpFrequency;
					}
					else
					{
			    	TBCCR1 += FrequencyB;
					}
				break;
				case DCCEL:
					if(TmpFrequency > FrequencyB)
					{
					TBCCR1 += TmpFrequency;
					}
					else
					{
			    	TBCCR1 += FrequencyB;
					}
				break;
			}
		break;
		case DRV8804:
			Send_SPI_Data(0);
			TBCCTL1 &= ~CCIE;
			break;
	}
  break;
  
case TBCCR2_CCIFG_SET:
	Send_SPI_Data(0);
	TBCCTL2 &= ~CCIE;

  break;
case TBCCR3_CCIFG_SET:
	Send_SPI_Data(0);
	TBCCTL3 &= ~CCIE;

  break;
case TBCCR4_CCIFG_SET:
    Send_SPI_Data(0);
    TBCCTL4 &= ~CCIE;
  
  break;

case TBCCR5_CCIFG_SET:					//Speed Control
	switch(Device)
	{
		case(DRV8803):
			count++;
			if(count > (AccelTimeBase*10) )
			{
				count=0;
				switch(Speed)
				{
					case DCCEL:
						if(TmpFrequency > FrequencyA)
						{
							TmpFrequency = 2000000/((2000000/TmpFrequency)+AccelPPS);
						}
					break;
					case ACCEL:
						if(TmpFrequency < FrequencyA)
						{
							TmpFrequency = 2000000/((2000000/TmpFrequency)-AccelPPS);
						}
					break;
				}
			}
			break;
			case(DRV8804):
			count++;
			if(count > (AccelTimeBase*10) )
			{
				count=0;
				switch(Speed)
				{
					case DCCEL:
						if(TmpFrequency > FrequencyA)
						{
							TmpFrequency = 2000000/((2000000/TmpFrequency)+AccelPPS);
						}
					break;
					case ACCEL:
						if(TmpFrequency < FrequencyA)
						{
							TmpFrequency = 2000000/((2000000/TmpFrequency)-AccelPPS);
						}
					break;
				}
			}
			break;
			case(DRV8805):
				TBCCR5 += 850;
				count++;
				if(count>AccelTimeBase)
				{
					count=0;
					switch(Speed)
					{
						case DCCEL:
							if(TmpFrequency > FrequencyB)
							{
								TmpFrequency = 2000000/((2000000/TmpFrequency)+AccelPPS);
							}
						break;
						case ACCEL:
							if(TmpFrequency < FrequencyB)
							{
								TmpFrequency = 2000000/((2000000/TmpFrequency)-AccelPPS);
							}
						break;
					}
				}
				break;
	}
  break;
  
case TBCCR6_CCIFG_SET:					   // Task Timer
	TBCCR6 += TASK_TIME;
    _BIC_SR_IRQ(CPUOFF);                   // Clear LPM0
    CommTimeOut += 1;
  break;
case TBIFG_SET:
  break;
}
}

#pragma vector=TIMERB0_VECTOR
__interrupt void TimerB0(void)
{
	switch (StepperIndex)
	    {
	    case (0):
	      Send_SPI_Data(1); 
	      TBCCTL1 &= ~CCIFG;
	      TBCCTL1 |= CCIE;   
	      break;
	    case (1):
	      Send_SPI_Data(2);	
	      TBCCTL2 &= ~CCIFG;
	      TBCCTL2 |= CCIE;         
	      break;
	    case (2):
	      Send_SPI_Data(4);
	      TBCCTL3 &= ~CCIFG;
	      TBCCTL3 |= CCIE; 
	      break;
	    case (3):
	      Send_SPI_Data(8);
	      TBCCTL4 &= ~CCIFG;
	      TBCCTL4 |= CCIE; 
	      break;
	      }
	

  
}

#pragma vector=NMI_VECTOR
__interrupt void NonMaskableInterrupt(void)
{

}

void Send_SPI_Data( int SPI_Data )
{
  int i;

  P4OUT &= ~SPI_LATCH;

  for (i = 0; i<4;i++)
    {
    P4OUT &= ~SPI_CLK;                                // CLK starts low. 
    if (SPI_Data & 0x0001)                            // Determine if MSBData Bit is High or Low
      P4OUT |= SPI_MOSI;                            // Write a 1
    else
      P4OUT &= ~SPI_MOSI;                           // Write a 0

    P4OUT |= SPI_CLK;                                 //The clock goes HI. Data is latched
    SPI_Data >>= 1;                                   // Shift Right SPI Data Out Register (MOSI)

    }
            
  P4OUT |= SPI_LATCH;
  P4OUT &= ~SPI_CLK;                                // CLK starts low. 
}

