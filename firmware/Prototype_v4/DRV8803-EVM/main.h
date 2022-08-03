//******************************************************************************
//  LT Thomas
//  Texas Instruments Inc.
//  May 2011
//  Built with CCS4
//******************************************************************************

void AppInit(void);
void Task0(void);
void Task1(void);
void Task2(void);
void Task3(void);
void Task4(void);
void Task5(void);
void Task6(void);
void Task7(void);

void StepExecute(void);

#define     TACCR1_CCIFG_SET    (0x02)
#define     TACCR2_CCIFG_SET    (0x04)
#define     TAIFG_SET           (0x0A)

#define     TBCCR1_CCIFG_SET    (0x02)
#define     TBCCR2_CCIFG_SET    (0x04)
#define     TBCCR3_CCIFG_SET    (0x06)
#define     TBCCR4_CCIFG_SET    (0x08)
#define     TBCCR5_CCIFG_SET    (0x0A)
#define     TBCCR6_CCIFG_SET    (0x0C)
#define     TBIFG_SET           (0x0E)
