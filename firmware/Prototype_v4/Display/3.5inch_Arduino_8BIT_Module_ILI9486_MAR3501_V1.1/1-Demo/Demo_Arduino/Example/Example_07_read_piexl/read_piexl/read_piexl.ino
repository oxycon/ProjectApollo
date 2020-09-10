/***********************************************************************************
*This program is a demo of how to read color data from speciallied point
*This demo was made for LCD modules with 8bit or 16bit data port.
*This program requires the the LCDKIWI library.

* File                : read_piexl.ino
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

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

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

//get the color data from speciallied point
void color_dump(uint16_t x,uint16_t y)
{
    uint8_t buf[30] = {0},pbuf[10] = {0};
    uint8_t wd = (my_lcd.Get_Display_Width() - 9 * 6)/ (5 * 6);
    uint8_t hi = (my_lcd.Get_Display_Height() / 8) - 1;
    uint16_t pixel = 0;
    //set white
    my_lcd.Set_Text_colour(WHITE);
    //set text size 1
    my_lcd.Set_Text_Size(1);
    for(int j = 0;j< hi;j++)
    {
      sprintf(buf,"%3d,%3d:",x,y+j);
      my_lcd.Print_String(buf, 0, 8*(j+1)*my_lcd.Get_Text_Size());
      for(int i=0;i<wd;i++)
      {          
//          my_lcd.Print_String(buf, 0, line+8);
        //read pixel
          pixel = my_lcd.Read_Pixel(x+i,y+j);

        // if white set green
          if(WHITE == pixel)
          {
             my_lcd.Set_Text_colour(GREEN); 
           }
           sprintf(pbuf,"%04X ",pixel); 
           my_lcd.Print_String(pbuf,(strlen(buf)+strlen(pbuf)*i)*6*my_lcd.Get_Text_Size(),8*(j+1)*my_lcd.Get_Text_Size());
        //set white
           my_lcd.Set_Text_colour(WHITE);
        } 
     }
}   

uint8_t aspect;
char *aspectname[] = {"PORTRAIT", "LANDSCAPE", "PORTRAIT_REV", "LANDSCAPE_REV"};
uint16_t colors[] = {BLACK, BLUE};

void setup()
{
  Serial.begin(9600);
  my_lcd.Init_LCD();
  Serial.println(my_lcd.Read_ID(), HEX);
//my_lcd.Set_Text_Back_colour(BLACK);
}

void loop() 
{
  uint16_t iter, color;
    char buf[80];
    aspect = (aspect + 1) & 3;
    my_lcd.Set_Rotation(aspect);
 //color_dump(36,0);
 
    for (iter = 0; iter < sizeof(colors) / sizeof(uint16_t); iter++) 
    {
        color = colors[iter];
        my_lcd.Fill_Screen(color);
        my_lcd.Set_Text_Back_colour(color);
        my_lcd.Set_Text_colour(WHITE);
        my_lcd.Set_Text_Size(1);
     sprintf(buf, " ID=0x%04X Background=%04X %s", my_lcd.Read_ID(), color, aspectname[aspect]);
      my_lcd.Print_String(buf,0,0);
        color_dump(36,0);
        delay(3000);
       
    } 

}
