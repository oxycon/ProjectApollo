/***********************************************************************************
*This program can be used to calibrate the touchscreen of the display modules.
*Instructions will be given on the display.

*This demo was made for LCD modules with 8bit or 16bit data port.
*This program requires the the LCDKIWI library.

* File                : TouchScreen_Calibr.ino
* Hardware Environment: Arduino UNO&Mega2560
* Build Environment   : Arduino

*Set the pins to the correct ones for your development shield or breakout board.
*This demo use the BREAKOUT BOARD only and use these 8bit data lines to the LCD,
*pin usage as follow:
*                  LCD_CS  LCD_CD  LCD_WR  LCD_RD  LCD_RST  SD_SS  SD_DI  SD_DO  SD_SCK 
*     Arduino Uno    A3      A2      A1      A0      A4      10     11     12      13                            
*Arduino Mega2560    A3      A2      A1      A0      A4      10     11     12      13                           

*                  LCD_D0  LCD_D1  LCD_D2  LCD_D3  LCD_D4  LCD_D5  LCD_D6  LCD_D7  
*     Arduino Uno    8       9       2       3       4       5       6       7
*Arduino Mega2560    8       9       2       3       4       5       6       7 

*Remember to set the pins to suit your display module!
*
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************/
#include <TouchScreen.h> //touch library
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

#define TOUCH_ORIENTATION  0
#define TITLE "TouchScreen.h Calibration"

//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV my_lcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_KBV my_lcd(320,480,A3,A2,A1,A0,A4);//width,height,cs,cd,wr,rd,reset

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

TouchScreen mytouch(XP, YP, XM, YM, 300);
TSPoint tp;                      //Touchscreen_due branch uses Point

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

void Read_Resistive(void)
{
    tp = mytouch.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
}

bool is_pressed(void)
{
    bool state;
    Read_Resistive();
    state = (tp.z > 20 && tp.z < 1000);
    return state;
}

void showpoint(void)
{
    Serial.print("\r\nx="); Serial.print(tp.x);
    Serial.print(" y="); Serial.print(tp.y);
    Serial.print(" z="); Serial.print(tp.z);
}

uint32_t cx, cy;
uint32_t rx[8], ry[8];
int32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center, swapxy;
uint32_t calx, caly, cals;
char buf[13];

void setup()
{
    Serial.begin(9600);
    Serial.println(TITLE);
    digitalWrite(A0, HIGH);
    pinMode(A0, OUTPUT);
    my_lcd.Init_LCD();
    my_lcd.Set_Rotation(TOUCH_ORIENTATION);
    my_lcd.Fill_Screen(BLACK);
    dispx = my_lcd.Get_Display_Width();
    dispy = my_lcd.Get_Display_Height();
    text_y_center = (dispy / 2) - 6;
}

void drawCrossHair(int x, int y, unsigned int color)
{
    my_lcd.Set_Draw_color(color);
    my_lcd.Draw_Rectangle(x - 10, y - 10, x + 10, y + 10);
    my_lcd.Draw_Fast_VLine(x, y-5, 11);
    my_lcd.Draw_Fast_HLine(x-5, y, 11);
}

void readCoordinates()
{
    int iter = 5000;
    int failcount = 0;
    int cnt = 0;
    uint32_t tx = 0;
    uint32_t ty = 0;
    boolean OK = false;

    while (OK == false)
    {
        my_lcd.Set_Draw_color(0);
        my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-33, text_y_center, my_lcd.Get_Display_Width()/2+33, text_y_center+8);
        show_string("*  PRESS  *",CENTER,text_y_center,1,GREEN, BLACK,1);
        while (is_pressed() == false) {}
        my_lcd.Set_Draw_color(0);
        my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-33, text_y_center, my_lcd.Get_Display_Width()/2+33, text_y_center+8);
        show_string("*  HOLD!  *",CENTER,text_y_center,1,RED, BLACK,1);
        cnt = 0;
        iter = 400;
        do
        {
            Read_Resistive();
            // showpoint(tp);
            if (tp.z > 20 && tp.z < 1000)
            {
                tx += tp.x;
                ty += tp.y;
                cnt++;
            }
            else
                failcount++;
        } while ((cnt < iter) && (failcount < 10000));
        if (cnt >= iter)
        {
            OK = true;
        }
        else
        {
            tx = 0;
            ty = 0;
            cnt = 0;
        }
        if (failcount >= 10000)
            fail();
    }

    cx = tx / iter;
    cy = ty / iter;
}

void calibrate(int x, int y, int i)
{
    drawCrossHair(x, y,WHITE);
    readCoordinates();
    my_lcd.Set_Draw_color(0);
    my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()/2-33, text_y_center, my_lcd.Get_Display_Width()/2+33, text_y_center+8);
    show_string("* RELEASE *",CENTER,text_y_center,1,BLUE, BLACK,1);
    drawCrossHair(x, y,0x528A);
    rx[i] = cx;
    ry[i] = cy;
    //Serial.print("\r\ncx="); Serial.print(cx);
    //Serial.print(" cy="); Serial.print(cy);
    while (is_pressed() == true) {}
}

void waitForTouch()
{
    while (is_pressed() == true) {}
    while (is_pressed() == false) {}
    while (is_pressed() == true) {}
}

void toHex(uint32_t num)
{
    buf[0] = '0';
    buf[1] = 'x';
    buf[10] = 'U';
    buf[11] = 'L';
    buf[12] = 0;
    for (int zz = 9; zz > 1; zz--)
    {
        if ((num & 0xF) > 9)
            buf[zz] = (num & 0xF) + 55;
        else
            buf[zz] = (num & 0xF) + 48;
        num = num >> 4;
    }
}

void startup()
{
    my_lcd.Set_Draw_color(255, 0, 0);
    my_lcd.Fill_Rectangle(0, 0, dispx - 1, 13);
    my_lcd.Set_Draw_color(255, 255, 255);
    my_lcd.Draw_Line(0, 14, dispx - 1, 14);
    show_string(TITLE,CENTER,1,1,WHITE, BLACK,1);

    my_lcd.Print_String("#define NUMSAMPLES 3 in Library", LEFT, 18);
    my_lcd.Print_String("Use a stylus or something", LEFT, 30);
    my_lcd.Print_String("similar to touch as close", LEFT, 42);
    my_lcd.Print_String("to the center of the", LEFT, 54);
    my_lcd.Print_String("highlighted crosshair as", LEFT, 66);
    my_lcd.Print_String("possible. Keep as still as", LEFT, 78);
    my_lcd.Print_String("possible and keep holding", LEFT, 90);
    my_lcd.Print_String("until the highlight is", LEFT, 102);
    my_lcd.Print_String("removed. Repeat for all", LEFT, 114);
    my_lcd.Print_String("crosshairs in sequence.", LEFT, 126);
    my_lcd.Print_String("Touch screen to continue", CENTER, 162);

    waitForTouch();
    my_lcd.Fill_Screen(BLACK);
}

void showNumI(char *msg, uint32_t val, int x, int y)
{
    show_string(msg,x,y,1,MAGENTA, BLACK,1);
    my_lcd.Set_Text_colour(GREEN);
    my_lcd.Print_Number_Int(val, x+50, y, 0, ' ',10);
}

void done()
{
    uint16_t TS_LEFT, TS_RT, TS_TOP, TS_BOT, TS_WID, TS_HT, TS_SWAP;
    int16_t tmp;
    my_lcd.Fill_Screen(BLACK);
    my_lcd.Set_Draw_color(255, 0, 0);
    my_lcd.Fill_Rectangle(0, 0, dispx - 1, 13);
    my_lcd.Set_Draw_color(255, 255, 255);
    my_lcd.Draw_Line(0, 14, dispx - 1, 14);
    show_string(TITLE,CENTER,1,1,WHITE, BLACK,1);
    my_lcd.Print_String("To use the new calibration", LEFT, 30);
    my_lcd.Print_String("settings you must map the values", LEFT, 42);
    my_lcd.Print_String("from Point p = ts.getPoint() e.g. ", LEFT, 54);
    my_lcd.Print_String("x = map(p.x, LEFT, RT, 0, lcd.width());", LEFT, 66);
    my_lcd.Print_String("y = map(p.y, TOP, BOT, 0, lcd.height());", LEFT, 78);
    my_lcd.Print_String("swap p.x and p.y if diff ORIENTATION", LEFT, 90);

    //.kbv show human values
    TS_LEFT = (calx >> 14) & 0x3FFF;
    TS_RT   = (calx >>  0) & 0x3FFF;
    TS_TOP  = (caly >> 14) & 0x3FFF;
    TS_BOT  = (caly >>  0) & 0x3FFF;
    TS_WID  = ((cals >> 12) & 0x0FFF) + 1;
    TS_HT   = ((cals >>  0) & 0x0FFF) + 1;
    TS_SWAP = (cals >> 31);
    if (TOUCH_ORIENTATION != 0) {
        my_lcd.Print_String("Sketch is LANDSCAPE", 0, 126);
        my_lcd.Print_Number_Int(TS_WID, 150, 126, 0, ' ',10);
        my_lcd.Print_String("x", 174, 126);
        my_lcd.Print_Number_Int(TS_HT, 186, 126, 0, ' ',10);
        showNumI("LEFT ", TS_LEFT, 0, 138);
        showNumI("RT   ", TS_RT, 100, 138);
        showNumI("TOP  ", TS_TOP, 0, 150);
        showNumI("BOT  ", TS_BOT, 100, 150);
        switch (TOUCH_ORIENTATION) {
            case 1:
                tmp = TS_LEFT, TS_LEFT = TS_TOP, TS_TOP = TS_RT, TS_RT = TS_BOT, TS_BOT = tmp;
                tmp = TS_WID, TS_WID = TS_HT, TS_HT = tmp;
                break;
        }
    }
    my_lcd.Print_String("PORTRAIT CALIBRATION", 0, 174);
    my_lcd.Print_Number_Int(TS_WID, 150, 174, 0, ' ',10);
    my_lcd.Print_String("x", 174, 174);
    my_lcd.Print_Number_Int(TS_HT, 186, 174, 0, ' ',10);
    showNumI("LEFT ", TS_LEFT, 0, 186);
    showNumI("RT   ", TS_RT, 100, 186);
    my_lcd.Print_Number_Float(((float)TS_RT - TS_LEFT) / TS_WID, 2, 200, 186, '.', 0, ' ');  
    showNumI("TOP  ", TS_TOP, 0, 198);
    showNumI("BOT  ", TS_BOT, 100, 198);
    my_lcd.Print_Number_Float(((float)TS_BOT - TS_TOP) / TS_HT, 2, 200, 198, '.', 0, ' ');  
    my_lcd.Print_String("Touch Pin Wiring is ", 0, 222);
    my_lcd.Print_String((cals >> 31) ? "SWAPXY" : "PORTRAIT", 170, 222);

}

void fail()
{
    my_lcd.Fill_Screen(BLACK);
    my_lcd.Set_Draw_color(255, 0, 0);
    my_lcd.Fill_Rectangle(0, 0, dispx - 1, 13);
    my_lcd.Set_Draw_color(255, 255, 255);
    my_lcd.Draw_Line(0, 14, dispx - 1, 14);
    show_string("Touch Calibration FAILED",CENTER,1,1,WHITE, BLACK,1);

    my_lcd.Print_String("Unable to read the position", LEFT, 30);
    my_lcd.Print_String("of the press. This is a", LEFT, 42);
    my_lcd.Print_String("hardware issue and can", LEFT, 54);
    my_lcd.Print_String("not be corrected in", LEFT, 66);
    my_lcd.Print_String("software.", LEFT, 78);
    my_lcd.Print_String("check XP, XM pins with a multimeter", LEFT, 102);
    my_lcd.Print_String("check YP, YM pins with a multimeter", LEFT, 114);
    my_lcd.Print_String("should be about 300 ohms", LEFT, 126);

    while (true) {};
}

void loop()
{
    startup();


    drawCrossHair(dispx - 11, 10,0x528A);
    drawCrossHair(dispx / 2, 10,0x528A);
    drawCrossHair(10, 10,0x528A);
    drawCrossHair(dispx - 11, dispy / 2,0x528A);
    drawCrossHair(10, dispy / 2,0x528A);
    drawCrossHair(dispx - 11, dispy - 11,0x528A);
    drawCrossHair(dispx / 2, dispy - 11,0x528A);
    drawCrossHair(10, dispy - 11,0x528A);

    show_string("***********",CENTER,text_y_center - 12,1,WHITE, BLACK,1);
    show_string("***********",CENTER,text_y_center + 12,1,WHITE, BLACK,1);
    
    calibrate(10, 10, 0);
    calibrate(10, dispy / 2, 1);
    calibrate(10, dispy - 11, 2);
    calibrate(dispx / 2, 10, 3);
    calibrate(dispx / 2, dispy - 11, 4);
    calibrate(dispx - 11, 10, 5);
    calibrate(dispx - 11, dispy / 2, 6);
    calibrate(dispx - 11, dispy - 11, 7);

    cals = (long(dispx - 1) << 12) + (dispy - 1);
    if (TOUCH_ORIENTATION == 0) swapxy = rx[2] - rx[0];
    else swapxy = ry[2] - ry[0];
    swapxy = (swapxy < -500 || swapxy > 500);
    if ((TOUCH_ORIENTATION == 0) ^ (swapxy != 0)) {
        clx = (rx[0] + rx[1] + rx[2]) / 3;
        crx = (rx[5] + rx[6] + rx[7]) / 3;
        cty = (ry[0] + ry[3] + ry[5]) / 3;
        cby = (ry[2] + ry[4] + ry[7]) / 3;
    } else {
        clx = (ry[0] + ry[1] + ry[2]) / 3;
        crx = (ry[5] + ry[6] + ry[7]) / 3;
        cty = (rx[0] + rx[3] + rx[5]) / 3;
        cby = (rx[2] + rx[4] + rx[7]) / 3;
    }
    px = float(crx - clx) / (dispx - 20);
    py = float(cby - cty) / (dispy - 20);
    //  px = 0;
    clx -= px * 10;
    crx += px * 10;
    cty -= py * 10;
    cby += py * 10;

    calx = (long(clx) << 14) + long(crx);
    caly = (long(cty) << 14) + long(cby);
    if (swapxy)
        cals |= (1L << 31);

    done();
    while (true) {}
}
