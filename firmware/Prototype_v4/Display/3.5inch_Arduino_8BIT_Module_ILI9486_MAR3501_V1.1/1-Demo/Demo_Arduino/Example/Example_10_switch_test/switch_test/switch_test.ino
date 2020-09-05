/***********************************************************************************
*This program is a demo of how to use the touch function in a switch
*This demo was made for LCD modules with 8bit or 16bit data port.
*This program requires the the LCDKIWI library.

* File                : switch_test.ino
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
#include "switch_font.c"

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

#define TS_MINX 906
#define TS_MAXX 116

#define TS_MINY 92
#define TS_MAXY 952
// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65


boolean switch_flag_1 = true,switch_flag_2 = true,switch_flag_3 = true,switch_flag_4 = true,switch_flag_5 = true,switch_flag_6 = true;  
int16_t menu_flag = 1,old_menu_flag;     

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//display string
void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

//diaplay a picture
void show_picture(const uint8_t *color_buf,int16_t buf_size,int16_t x1,int16_t y1,int16_t x2,int16_t y2)
{
    my_lcd.Set_Addr_Window(x1, y1, x2, y2); 
    my_lcd.Push_Any_Color(color_buf, buf_size, 1, 1);
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

//show the system menu
void show_system_menu(void)
{    
    show_string("time setting",10,150,2,BLACK, BLACK,1);
    show_string("date setting",10,170,2,BLACK, BLACK,1);
    show_string("alarm setting",10,190,2,BLACK, BLACK,1);
    show_string("talk setting",10,210,2,BLACK, BLACK,1);
    show_string("sound setting",10,230,2,BLACK, BLACK,1);
    switch(menu_flag)
    {
      case 1:
      {
          my_lcd.Set_Draw_color(BLUE);
          my_lcd.Fill_Rectangle(0, 147, my_lcd.Get_Display_Width()-1, 166);
          show_string("time setting",10,150,2,WHITE, BLACK,1);
          break;
      }
      case 2:
      {
          my_lcd.Set_Draw_color(BLUE);
          my_lcd.Fill_Rectangle(0, 167, my_lcd.Get_Display_Width()-1, 186);
          show_string("date setting",10,170,2,WHITE, BLACK,1);
          break;
      }
      case 3:
      {
          my_lcd.Set_Draw_color(BLUE);
          my_lcd.Fill_Rectangle(0, 187, my_lcd.Get_Display_Width()-1, 206);
          show_string("alarm setting",10,190,2,WHITE, BLACK,1);
          break;
      }
      case 4:
      {
          my_lcd.Set_Draw_color(BLUE);
          my_lcd.Fill_Rectangle(0, 207, my_lcd.Get_Display_Width()-1, 226);
          show_string("talk setting",10,210,2,WHITE, BLACK,1);
          break;
      }
      case 5:
      {
          my_lcd.Set_Draw_color(BLUE);
          my_lcd.Fill_Rectangle(0, 227, my_lcd.Get_Display_Width()-1, 246);
          show_string("sound setting",10,230,2,WHITE, BLACK,1);
          break;
      }
      default:
        break;
    }
}
                    
void setup(void) 
{    
 Serial.begin(9600);
 my_lcd.Init_LCD();
 Serial.println(my_lcd.Read_ID(), HEX);
 my_lcd.Fill_Screen(WHITE); 

 //show the switch picture
 my_lcd.Set_Draw_color(192, 192, 192);
 my_lcd.Draw_Fast_HLine(0, 3, my_lcd.Get_Display_Width());
 show_picture(switch_on_2,sizeof(switch_on_2)/2,5,5,34,34);
 show_string("switch is on ",60,11,2,GREEN, BLACK,1);

 my_lcd.Draw_Fast_HLine(0, 37, my_lcd.Get_Display_Width());
 
 show_string("wifi setting",5,40,2,BLACK, BLACK,1);
 show_picture(switch_on_3,sizeof(switch_on_3)/2,195,40,234,54);

  my_lcd.Draw_Fast_HLine(0, 57, my_lcd.Get_Display_Width());
 
 show_string("bt setting",5,60,2,BLACK, BLACK,1);
 show_picture(switch_on_3,sizeof(switch_on_3)/2,195,60,234,74);

 my_lcd.Draw_Fast_HLine(0, 77, my_lcd.Get_Display_Width());
 
 show_string("auto time",5,80,2,BLACK, BLACK,1);
 show_picture(switch_on_1,sizeof(switch_on_1)/2,204,80,218,94);

 my_lcd.Draw_Fast_HLine(0, 97, my_lcd.Get_Display_Width());
 
 show_string("enable lock",5,100,2,BLACK, BLACK,1);
 show_picture(switch_on_1,sizeof(switch_on_1)/2,204,100,218,114);

 my_lcd.Draw_Fast_HLine(0, 116, my_lcd.Get_Display_Width());
 
 show_string("system setting   >",5,119,2,BLUE, BLACK,1);
 my_lcd.Draw_Fast_HLine(0, 138, my_lcd.Get_Display_Width());
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000
void loop(void)
{
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
   
    if(is_pressed(5,5,34,34,p.x,p.y))
    {
        if(switch_flag_1)
        {   //display off 
            show_picture(switch_off_2,sizeof(switch_off_2)/2,5,5,34,34);
            my_lcd.Set_Draw_color(WHITE);
            my_lcd.Fill_Rectangle(60, 11,  216, 27);
            show_string("switch is off ",60,11,2,RED, BLACK,1); 
            switch_flag_1 = false;
         }
         else
         {  //display on
           show_picture(switch_on_2,sizeof(switch_on_2)/2,5,5,34,34);
           my_lcd.Set_Draw_color(WHITE);
           my_lcd.Fill_Rectangle(60, 11,  216, 27);
           show_string("switch is on ",60,11,2,GREEN, BLACK,1);
           switch_flag_1 = true;
         }
         delay(100);
     }
     if(is_pressed(195,40,234,54,p.x,p.y))
     {
        if(switch_flag_2)
        {
            show_picture(switch_off_3,sizeof(switch_off_3)/2,195,40,234,54);
            switch_flag_2 = false;
        }
        else
        {
           show_picture(switch_on_3,sizeof(switch_on_3)/2,195,40,234,54);
           switch_flag_2 = true;
        }
        delay(100);
      }
      if(is_pressed(195,60,234,74,p.x,p.y))
      {
         if(switch_flag_3)
        {
            show_picture(switch_off_3,sizeof(switch_off_3)/2,195,60,234,74);
            switch_flag_3 = false;
        }
        else
        {
           show_picture(switch_on_3,sizeof(switch_on_3)/2,195,60,234,74);
           switch_flag_3 = true;
        }
        delay(100);
       }
       if(is_pressed(205,81,217,93,p.x,p.y))
       {
         if(switch_flag_4)
         {
           show_picture(switch_off_1,sizeof(switch_off_1)/2,204,80,218,94);
            switch_flag_4 = false;
         }
         else
         {
           show_picture(switch_on_1,sizeof(switch_on_1)/2,204,80,218,94);
            switch_flag_4 = true;
         }
        delay(100);
       }
       if(is_pressed(205,101,217,113,p.x,p.y))
       {
         if(switch_flag_5)
         {
          show_picture(switch_off_1,sizeof(switch_off_1)/2,204,100,218,114);
          switch_flag_5 = false;
         }
         else
         {
           show_picture(switch_on_1,sizeof(switch_on_1)/2,204,100,218,114);
           switch_flag_5 = true;
          }
         delay(100);
       }
       if(is_pressed(5,119,my_lcd.Get_Display_Width()-1,137,p.x,p.y))
       {     
            my_lcd.Set_Draw_color(MAGENTA);
            my_lcd.Fill_Rectangle(0, 117, my_lcd.Get_Display_Width()-1, 137);
            delay(100);
            my_lcd.Set_Draw_color(WHITE);
            my_lcd.Fill_Rectangle(0, 117, my_lcd.Get_Display_Width()-1, 137);
            if(switch_flag_6)
            {
                show_string("system setting   <",5,119,2,BLUE, BLACK,1);
                show_system_menu();
                switch_flag_6 = false;
            }
            else
            {
                 show_string("system setting   >",5,119,2,BLUE, BLACK,1);
                 my_lcd.Set_Draw_color(WHITE);
                 my_lcd.Fill_Rectangle(0, 147, my_lcd.Get_Display_Width()-1, 250);
                 switch_flag_6 = true;
            }
       }
       if(is_pressed(0,147,my_lcd.Get_Display_Width()-1,247,p.x,p.y)&&(!switch_flag_6))
       {
          old_menu_flag = menu_flag;     
          if(is_pressed(0,147,my_lcd.Get_Display_Width()-1,166,p.x,p.y))
          {
              my_lcd.Set_Draw_color(BLUE);
              my_lcd.Fill_Rectangle(0, 147, my_lcd.Get_Display_Width()-1, 166);
              show_string("time setting",10,150,2,WHITE, BLACK,1);
              menu_flag = 1;
           }
           if(is_pressed(0,167,my_lcd.Get_Display_Width()-1,186,p.x,p.y))
           {
              my_lcd.Set_Draw_color(BLUE);
              my_lcd.Fill_Rectangle(0, 167, my_lcd.Get_Display_Width()-1, 186);
              show_string("date setting",10,170,2,WHITE, BLACK,1);
              menu_flag = 2;
           }
           if(is_pressed(0,187,my_lcd.Get_Display_Width()-1,206,p.x,p.y))
           {
              my_lcd.Set_Draw_color(BLUE);
              my_lcd.Fill_Rectangle(0, 187, my_lcd.Get_Display_Width()-1, 206);
              show_string("alarm setting",10,190,2,WHITE, BLACK,1);
              menu_flag = 3;
           }
           if(is_pressed(0,207,my_lcd.Get_Display_Width()-1,226,p.x,p.y))
           {
               my_lcd.Set_Draw_color(BLUE);
               my_lcd.Fill_Rectangle(0, 207, my_lcd.Get_Display_Width()-1, 226);
              show_string("talk setting",10,210,2,WHITE, BLACK,1);
              menu_flag = 4;
           }
           if(is_pressed(0,227,my_lcd.Get_Display_Width()-1,246,p.x,p.y))
           {
              my_lcd.Set_Draw_color(BLUE);
              my_lcd.Fill_Rectangle(0, 227, my_lcd.Get_Display_Width()-1, 246);
              show_string("sound setting",10,230,2,WHITE, BLACK,1);
              menu_flag = 5;
           }  
           if(old_menu_flag != menu_flag)
           {
              switch(old_menu_flag)
              {
                case 1:
                {
                    my_lcd.Set_Draw_color(WHITE);
                    my_lcd.Fill_Rectangle(0, 147, my_lcd.Get_Display_Width()-1, 166);
                    show_string("time setting",10,150,2,BLACK, BLACK,1);
                    break;
                }
                case 2:
                {
                    my_lcd.Set_Draw_color(WHITE);
                    my_lcd.Fill_Rectangle(0, 167, my_lcd.Get_Display_Width()-1, 186);
                    show_string("date setting",10,170,2,BLACK, BLACK,1);
                    break;
                }
                case 3:
                {
                    my_lcd.Set_Draw_color(WHITE);
                    my_lcd.Fill_Rectangle(0, 187, my_lcd.Get_Display_Width()-1, 206);
                    show_string("alarm setting",10,190,2,BLACK, BLACK,1);
                    break;
                }
                case 4:
                {
                    my_lcd.Set_Draw_color(WHITE);
                    my_lcd.Fill_Rectangle(0, 207, my_lcd.Get_Display_Width()-1, 226);
                    show_string("talk setting",10,210,2,BLACK, BLACK,1);
                    break;
                }
                case 5:
                {
                    my_lcd.Set_Draw_color(WHITE);
                    my_lcd.Fill_Rectangle(0, 227, my_lcd.Get_Display_Width()-1, 246);
                    show_string("sound setting",10,230,2,BLACK, BLACK,1);
                    break;
                }
                default:
                  break;                  
             }       
         }
         delay(100);
     }
  }
}
