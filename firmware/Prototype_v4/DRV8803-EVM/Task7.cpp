//******************************************************************************
//  LT Thomas
//  Texas Instruments Inc.
//  May 2011
//  Built with CCS4
//******************************************************************************
#include "Config.h"

void Task7(void)
{
StatusLED += 1;
if (!StatusLED)
    {
    P6OUT ^= StatusLEDPin;
    }
if (CommTimeOut > 200)
    {
    CommTimeOut = 201;
    SerialPointer = 0;
    }
}
