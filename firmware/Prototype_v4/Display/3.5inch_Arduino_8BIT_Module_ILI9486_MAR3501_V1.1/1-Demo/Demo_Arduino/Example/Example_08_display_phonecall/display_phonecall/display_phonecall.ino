/***********************************************************************************
*This program is a demo of how to use the touch function in a phone GUI
*This demo was made for LCD modules with 8bit or 16bit data port.
*This program requires the the LCDKIWI library.

* File                : display_phonecall.ino
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

//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV my_lcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_KBV my_lcd(320,480,A3,A2,A1,A0,A4);//width,height,cs,cd,wr,rd,reset

                             /*  r     g    b */
#define BLACK        0x0000  /*   0,   0,   0 */
#define BLUE         0x001F  /*   0,   0, 255 */
#define RED          0xF800  /* 255,   0,   0 */
#define GREEN        0x07E0  /*   0, 255,   0 */
#define CYAN         0x07FF  /*   0, 255, 255 */
#define MAGENTA      0xF81F  /* 255,   0, 255 */
#define YELLOW       0xFFE0  /* 255, 255,   0 */
#define WHITE        0xFFFF  /* 255, 255, 255 */
#define NAVY         0x000F  /*   0,   0, 128 */
#define DARKGREEN    0x03E0  /*   0, 128,   0 */
#define DARKCYAN     0x03EF  /*   0, 128, 128 */
#define MAROON       0x7800  /* 128,   0,   0 */
#define PURPLE       0x780F  /* 128,   0, 128 */
#define OLIVE        0x7BE0  /* 128, 128,   0 */
#define LIGHTGREY    0xC618  /* 192, 192, 192 */
#define DARKGREY     0x7BEF  /* 128, 128, 128 */
#define ORANGE       0xFD20  /* 255, 165,   0 */
#define GREENYELLOW  0xAFE5  /* 173, 255,  47 */
#define PINK         0xF81F  /* 255,   0, 255 */

/******************* UI details */
#define BUTTON_R 35 //the radius of button 
#define BUTTON_SPACING_X 35 //the horizontal distance between button
#define BUTTON_SPACING_Y 10  //the vertical distance between button
#define EDG_Y 10 //lower edge distance 
#define EDG_X 20 //left and right distance

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//touch sensitivity for X
#define TS_MINX 906
#define TS_MAXX 116

//touch sensitivity for Y
#define TS_MINY 92
#define TS_MAXY 952

// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65

//touch sensitivity for press
#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

typedef struct _button_info
{
     uint8_t button_name[10];
     uint8_t button_name_size;
     uint16_t button_name_colour;
     uint16_t button_colour;
     uint16_t button_x;
     uint16_t button_y;     
 }button_info;

//the definition of buttons
button_info phone_button[15] = 
{
  "1",4,BLACK,CYAN,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-4*BUTTON_SPACING_Y-9*BUTTON_R-1,
  "2",4,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-4*BUTTON_SPACING_Y-9*BUTTON_R-1,
  "3",4,BLACK,CYAN,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-4*BUTTON_SPACING_Y-9*BUTTON_R-1,
  "4",4,BLACK,CYAN,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-3*BUTTON_SPACING_Y-7*BUTTON_R-1, 
  "5",4,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-3*BUTTON_SPACING_Y-7*BUTTON_R-1,
  "6",4,BLACK,CYAN,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-3*BUTTON_SPACING_Y-7*BUTTON_R-1,
  "7",4,BLACK,CYAN,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-2*BUTTON_SPACING_Y-5*BUTTON_R-1,
  "8",4,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-2*BUTTON_SPACING_Y-5*BUTTON_R-1,
  "9",4,BLACK,CYAN,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-2*BUTTON_SPACING_Y-5*BUTTON_R-1,
  "*",4,BLACK,PINK,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1,
  "0",4,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1,
  "#",4,BLACK,PINK,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1,
  "end",3,BLACK,RED,EDG_X+BUTTON_R-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_R-1,
  "call",3,BLACK,GREEN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_R-1,
  "dele",3,BLACK,LIGHTGREY,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,my_lcd.Get_Display_Height()-EDG_Y-BUTTON_R-1,
};

//display string
void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

//Check whether to press or not
boolean is_pressed(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t px,int16_t py)
{
    if((px > x1 && px < x2) && (py > y1 && py < y2))
    {
        return true;  
    } 
    else
    {
        return false;  
    }
 }

//display the main menu
void show_menu(void)
{
    uint16_t i;
   for(i = 0;i < sizeof(phone_button)/sizeof(button_info);i++)
   {
      my_lcd.Set_Draw_color(phone_button[i].button_colour);
      my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
      show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+phone_button[i].button_name_size/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+phone_button[i].button_name_size/2+1,phone_button[i].button_name_size,phone_button[i].button_name_colour,BLACK,1);
   }
   my_lcd.Set_Draw_color(BLACK);
   my_lcd.Fill_Rectangle(1, 1, my_lcd.Get_Display_Width()-2, 3);
   my_lcd.Fill_Rectangle(1, 45, my_lcd.Get_Display_Width()-2, 47);
   my_lcd.Fill_Rectangle(1, 1, 3, 47);
   my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()-4, 1, my_lcd.Get_Display_Width()-2, 47);
}
                            
void setup(void) 
{
  Serial.begin(9600);
   my_lcd.Init_LCD();
   Serial.println(my_lcd.Read_ID(), HEX);
   my_lcd.Fill_Screen(BLUE); 
   show_menu();
}

uint16_t text_x=7,text_y=10,text_x_add = 6*phone_button[0].button_name_size,text_y_add = 8*phone_button[0].button_name_size;
uint16_t n=0;

void loop(void)
{
  uint16_t i;
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    //p.x = my_lcd.Get_Display_Width()-map(p.x, TS_MINX, TS_MAXX, my_lcd.Get_Display_Width(), 0);
    //p.y = my_lcd.Get_Display_Height()-map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Height(), 0);
    p.x = map(p.x, TS_MINX, TS_MAXX, my_lcd.Get_Display_Width(),0);
    p.y = map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Height(),0);
    
    for(i=0;i<sizeof(phone_button)/sizeof(button_info);i++)
    {
         //press the button
         if(is_pressed(phone_button[i].button_x-BUTTON_R,phone_button[i].button_y-BUTTON_R,phone_button[i].button_x+BUTTON_R,phone_button[i].button_y+BUTTON_R,p.x,p.y))
         {
              my_lcd.Set_Draw_color(DARKGREY);
              my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
              show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+phone_button[i].button_name_size/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+phone_button[i].button_name_size/2+1,phone_button[i].button_name_size,WHITE,BLACK,1);
              delay(100);
              my_lcd.Set_Draw_color(phone_button[i].button_colour);
              my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
              show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+phone_button[i].button_name_size/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+phone_button[i].button_name_size/2+1,phone_button[i].button_name_size,phone_button[i].button_name_colour,BLACK,1);  
              if(i < 12)
              {
                  if(n < 13)
                  {
                    show_string(phone_button[i].button_name,text_x,text_y,phone_button[i].button_name_size,GREENYELLOW, BLACK,1);
                    text_x += text_x_add-1;
                    n++;
                  }
              }
              else if(12 == i) //show calling ended
              {
                  my_lcd.Set_Draw_color(BLUE);
                  my_lcd.Fill_Rectangle(0, 48, my_lcd.Get_Display_Width()-1, 60);
                  show_string("Calling ended",CENTER,52,1,RED, BLACK,1);  
              } 
              else if(13 == i) //show calling
              {
                  my_lcd.Set_Draw_color(BLUE);
                  my_lcd.Fill_Rectangle(0, 48, my_lcd.Get_Display_Width()-1, 60);
                  show_string("Calling...",CENTER,52,1,GREEN, BLACK,1);  
              }
              else if(14 == i) //delete button
              {
                  if(n > 0)
                  {
                      my_lcd.Set_Draw_color(BLUE);
                      text_x -= (text_x_add-1);  
                      my_lcd.Fill_Rectangle(text_x, text_y, text_x+text_x_add-1, text_y+text_y_add-2);
                      n--; 
                  }
              }
         }      
     }
  }
}
