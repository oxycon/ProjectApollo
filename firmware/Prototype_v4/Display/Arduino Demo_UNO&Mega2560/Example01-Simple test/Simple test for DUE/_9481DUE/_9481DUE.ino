//Technical support:goodtft@163.com
// Breakout/Arduino UNO pin usage:
// LCD Data Bit :   7   6   5   4   3   2   1   0
// Uno dig. pin :   7   6   5   4   3   2   9   8
// Uno port/pin : PD7 PD6 PD5 PD4 PD3 PD2 PB1 PB0
// Mega dig. pin:  29  28  27  26  25  24  23  22
//              : PH4  PH3 PE3 PG5 PE5 PE4 PH6 PH5
// Due port/pin : C23  C24 B27 C26  D7 C29 C21 C22
#define LCD_RD   A0
#define LCD_WR   A1     
#define LCD_RS   A2        
#define LCD_CS   A3       
#define LCD_REST A4

#define WR_PORT PIOA				/*pin A1 */
#define CD_PORT PIOA				/*pin A2 */

 #define RD_MASK 0x00010000
 #define WR_MASK 0x01000000
 #define CD_MASK 0x00800000
 #define CS_MASK  0x00400000
 #define RST_MASK 0x00000040 

#define WR_ACTIVE  WR_PORT->PIO_CODR |= WR_MASK
#define WR_IDLE    WR_PORT->PIO_SODR |= WR_MASK
#define CD_COMMAND CD_PORT->PIO_CODR |= CD_MASK
#define CD_DATA    CD_PORT->PIO_SODR |= CD_MASK

#define RST_ACTIVE  PIOA->PIO_CODR |= RST_MASK
#define RST_IDLE    PIOA->PIO_SODR |= RST_MASK

#define RD_ACTIVE  PIOA->PIO_CODR |= RD_MASK
#define RD_IDLE    PIOA->PIO_SODR |= RD_MASK
void Lcd_Writ_Bus(unsigned char d)
{
   PIO_Set(PIOD, (((d) & 0x08)<<(7-3))); 
   PIO_Clear(PIOD, (((~d) & 0x08)<<(7-3))); 
   PIO_Set(PIOC, (((d) & 0x01)<<(22-0)) | (((d) & 0x02)<<(21-1))| (((d) & 0x04)<<(29-2))| (((d) & 0x10)<<(26-4))| (((d) & 0x40)<<(24-6))| (((d) & 0x80)<<(23-7))); 
   PIO_Clear(PIOC, (((~d) & 0x01)<<(22-0)) | (((~d) & 0x02)<<(21-1))| (((~d) & 0x04)<<(29-2))| (((~d) & 0x10)<<(26-4))| (((~d) & 0x40)<<(24-6))| (((~d) & 0x80)<<(23-7))); 
   PIO_Set(PIOB, (((d) & 0x20)<<(27-5))); 
   PIO_Clear(PIOB, (((~d) & 0x20)<<(27-5))); 
  WR_ACTIVE;
  WR_IDLE;
}


void Lcd_Write_Com(unsigned char VH)  
{   
  CD_COMMAND;
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Data(unsigned char VH)
{
  CD_DATA;
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Com_Data(unsigned char com,unsigned char dat)
{
  Lcd_Write_Com(com);
  Lcd_Write_Data(dat);
}

void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
        Lcd_Write_Com(0x2a);
	Lcd_Write_Data(x1>>8);
	Lcd_Write_Data(x1);
	Lcd_Write_Data(x2>>8);
	Lcd_Write_Data(x2);
        Lcd_Write_Com(0x2b);
	Lcd_Write_Data(y1>>8);
	Lcd_Write_Data(y1);
	Lcd_Write_Data(y2>>8);
	Lcd_Write_Data(y2);
	Lcd_Write_Com(0x2c); 							 
}

void Lcd_Init(void)
{

RST_IDLE;delay(500); 
RST_ACTIVE;delay(1500);
RST_IDLE;delay(500); 
RD_IDLE;

/*
  digitalWrite(LCD_CS,HIGH);
  digitalWrite(LCD_WR,HIGH);
  digitalWrite(LCD_CS,LOW);  //CS
  */
  Lcd_Write_Com(0x11);
  delay(200);
  Lcd_Write_Com(0xD0);
  Lcd_Write_Data(0x07);
  Lcd_Write_Data(0x42);
  Lcd_Write_Data(0x18);
  
  Lcd_Write_Com(0xD1);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x07);//07
  Lcd_Write_Data(0x10);
  
  Lcd_Write_Com(0xD2);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0x02);
  
  Lcd_Write_Com(0xC0);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x3B);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x02);
  Lcd_Write_Data(0x11);
  
  Lcd_Write_Com(0xC5);
  Lcd_Write_Data(0x03);
  Lcd_Write_Com(0x36);
  Lcd_Write_Data(0x0A);
  
  Lcd_Write_Com(0x3A);
  Lcd_Write_Data(0x55);
  
  Lcd_Write_Com(0x2A);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0x3F);
  
  Lcd_Write_Com(0x2B);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0xE0);
  delay(1200);
  Lcd_Write_Com(0x29);
  Lcd_Write_Com(0x002c); 
}

void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)                   
{	
  unsigned int i,j;
  Lcd_Write_Com(0x02c); //write_memory_start
  digitalWrite(LCD_RS,HIGH);
  digitalWrite(LCD_CS,LOW);
  l=l+x;
  Address_set(x,y,l,y);
  j=l*2;
  for(i=1;i<=j;i++)
  {
    Lcd_Write_Data(c);
  }
  digitalWrite(LCD_CS,HIGH);   
}

void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)                   
{	
  unsigned int i,j;
  Lcd_Write_Com(0x02c); //write_memory_start
  digitalWrite(LCD_RS,HIGH);
  digitalWrite(LCD_CS,LOW);
  l=l+y;
  Address_set(x,y,x,l);
  j=l*2;
  for(i=1;i<=j;i++)
  { 
    Lcd_Write_Data(c);
  }
  digitalWrite(LCD_CS,HIGH);   
}

void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}

void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  unsigned int i;
  for(i=0;i<h;i++)
  {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}
int RGB(int r,int g,int b)
{return r << 16 | g << 8 | b;
}
void LCD_Clear(unsigned int j)                   
{	
  unsigned int i,m;
 Address_set(0,0,320,480);
  //Lcd_Write_Com(0x02c); //write_memory_start
  //digitalWrite(LCD_RS,HIGH);
  digitalWrite(LCD_CS,LOW);


  for(i=0;i<320;i++)
    for(m=0;m<480;m++)
    {
      Lcd_Write_Data(j>>8);
      Lcd_Write_Data(j);

    }
  digitalWrite(LCD_CS,HIGH);   
}

void setup()
{/*
  for(int p=0;p<10;p++)
  {
    pinMode(p,OUTPUT);
  }*/
 PIOD->PIO_MDDR |=  0x00000080; /*PIOD->PIO_SODR =  0x00000080;*/ PIOD->PIO_OER |=  0x00000080; PIOD->PIO_PER |=  0x00000080; 
   PIOC->PIO_MDDR |=  0x25E00000; /*PIOC->PIO_SODR =  0x25E00000;*/ PIOC->PIO_OER |=  0x25E00000; PIOC->PIO_PER |=  0x25E00000; 
   PIOB->PIO_MDDR |=  0x08000000; /*PIOB->PIO_SODR =  0x08000000;*/ PIOB->PIO_OER |=  0x08000000; PIOB->PIO_PER |=  0x08000000;

PIOA->PIO_MDDR |=  CS_MASK; PIOA->PIO_OER |=  CS_MASK; PIOA->PIO_PER |=  CS_MASK;
PIOA->PIO_MDDR |=  CD_MASK; PIOA->PIO_OER |=  CD_MASK; PIOA->PIO_PER |=  CD_MASK;
PIOA->PIO_MDDR |=  WR_MASK; PIOA->PIO_OER |=  WR_MASK; PIOA->PIO_PER |=  WR_MASK;
PIOA->PIO_MDDR |=  RD_MASK; PIOA->PIO_OER |=  RD_MASK; PIOA->PIO_PER |=  RD_MASK;
PIOA->PIO_MDDR |=  RST_MASK; PIOA->PIO_OER |=  RST_MASK; PIOA->PIO_PER |=  RST_MASK;

   PIOA->PIO_CODR  |=  CS_MASK; // Set all control bits to HIGH (idle)
    PIOA->PIO_SODR  |=  CD_MASK; // Signals are ACTIVE LOW
    PIOA->PIO_SODR  |=  WR_MASK;
    PIOA->PIO_SODR  |=  RD_MASK;

  Lcd_Init();
 //LCD_Clear(0xf800);
}

void loop()
{  
   LCD_Clear(0xf800);
   LCD_Clear(0x07E0);
   LCD_Clear(0x001F);
  /*   
  for(int i=0;i<1000;i++)
  {
    Rect(random(300),random(300),random(300),random(300),random(65535)); // rectangle at x, y, with, hight, color
  }*/
  
//  LCD_Clear(0xf800);
}
