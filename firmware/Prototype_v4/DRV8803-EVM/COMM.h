//******************************************************************************
//  Jose I Quinones
//  Texas Instruments Inc.
//  May 2011
//  Built with IAR Embedded Workbench Version: 5.1
//******************************************************************************
//***********************************************************************************
// Communication Engine Globals                                                     *
//***********************************************************************************
#define SERIAL_BUFFER_LENGTH    5
#define SERIAL_OUT_LENGTH       3
extern char SerialPointer;
extern char SerialBuffer[SERIAL_BUFFER_LENGTH];
extern char SerialOutPointer;
extern char SerialOutBuffer[SERIAL_OUT_LENGTH];
extern bool MessageComplete;
extern int CommTimeOut;
