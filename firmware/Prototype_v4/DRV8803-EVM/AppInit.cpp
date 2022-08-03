//******************************************************************************
//  LT Thomas
//  Texas Instruments Inc.
//  May 2011
//  Built with CCS4
//******************************************************************************
#include "Config.h"

void AppInit(void)
{
	count = 0;
	TmpFrequency = 0xFFFF;				//Set 0 Pulses Per Second
	AccelPPS = 1;						//Configure the Pulses Per Second Acceleration Rate
    AccelTimeBase = 1;
    P4OUT |= 0x20;						//Disable Motor
}
