#include "charCode.h"

#define LCD_SCL P1_2       //SCLK  时钟 D0（SCLK）
#define LCD_SDA P1_3       //SDA   D1（MOSI） 数据
#define LCD_RST P1_7       //_RES  hardware reset   复位 
#define LCD_DC  P0_0       //A0  H/L 命令数据选通端，H：数据，L:命令

#define XLevelL        0x00
#define XLevelH        0x10
#define XLevel         ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column     128
#define Max_Row        64
#define Brightness     0xCF 
#define X_WIDTH        128
#define Y_WIDTH        64

/*********************LCD 延时1ms************************************/
void LCD_DLY_ms(unsigned int ms)
{                         
    unsigned int a;
    while(ms)
    {
        a=1800;
        while(a--);
        ms--;
    }
    return;
}
/*********************LCD写数据************************************/ 
void LCD_WrDat(unsigned char dat)     
{
    unsigned char i=8, temp=0;
    LCD_DC=1;  
    for(i=0;i<8;i++) //发送一个八位数据 
    {
        LCD_SCL=0;  
        
        temp = dat&0x80;
        if (temp == 0)
        {
            LCD_SDA = 0;
        }
        else
        {
            LCD_SDA = 1;
        }
        LCD_SCL=1;             
        dat<<=1;    
    }
}
/*********************LCD写命令************************************/                                        
void LCD_WrCmd(unsigned char cmd)
{
    unsigned char i=8, temp=0;
    LCD_DC=0;
    for(i=0;i<8;i++) //发送一个八位数据 
    { 
        LCD_SCL=0; 
       
        temp = cmd&0x80;
        if (temp == 0)
        {
            LCD_SDA = 0;
        }
        else
        {
            LCD_SDA = 1;
        }
        LCD_SCL=1;
        cmd<<=1;;        
    }     
}
/*********************LCD 设置坐标************************************/
void LCD_Set_Pos(unsigned char x, unsigned char y) 
{ 
    LCD_WrCmd(0xb0+y);
    LCD_WrCmd(((x&0xf0)>>4)|0x10);
    LCD_WrCmd((x&0x0f)|0x01); 
} 
/*********************LCD全屏************************************/
void LCD_Fill(unsigned char bmp_dat) 
{
    unsigned char y,x;
    for(y=0;y<8;y++)
    {
        LCD_WrCmd(0xb0+y);
        LCD_WrCmd(0x01);
        LCD_WrCmd(0x10);
        for(x=0;x<X_WIDTH;x++)
            LCD_WrDat(bmp_dat);
    }
}
/*********************LCD复位************************************/
void LCD_CLS(void)
{
    unsigned char y,x;    
    for(y=0;y<8;y++)
    {
        LCD_WrCmd(0xb0+y);
        LCD_WrCmd(0x01);
        LCD_WrCmd(0x10); 
        for(x=0;x<X_WIDTH;x++)
            LCD_WrDat(0);
    }
}
/*********************LCD初始化************************************/
void LCD_Init(void)     
{  
    LCD_SCL=1;
    LCD_RST=0;
    LCD_DLY_ms(50);
    LCD_RST=1;      //从上电到下面开始初始化要有足够的时间，即等待RC复位完毕   
    LCD_WrCmd(0xae);//--turn off oled panel
    LCD_WrCmd(0x00);//---set low column address
    LCD_WrCmd(0x10);//---set high column address
    LCD_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    LCD_WrCmd(0x81);//--set contrast control register
    LCD_WrCmd(0xcf); // Set SEG Output Current Brightness
    LCD_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    LCD_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    LCD_WrCmd(0xa6);//--set normal display
    LCD_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
    LCD_WrCmd(0x3f);//--1/64 duty
    LCD_WrCmd(0xd3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    LCD_WrCmd(0x00);//-not offset
    LCD_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
    LCD_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    LCD_WrCmd(0xd9);//--set pre-charge period
    LCD_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    LCD_WrCmd(0xda);//--set com pins hardware configuration
    LCD_WrCmd(0x12);
    LCD_WrCmd(0xdb);//--set vcomh
    LCD_WrCmd(0x40);//Set VCOM Deselect Level
    LCD_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    LCD_WrCmd(0x02);//
    LCD_WrCmd(0x8d);//--set Charge Pump enable/disable
    LCD_WrCmd(0x14);//--set(0x10) disable
    LCD_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    LCD_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
    LCD_WrCmd(0xaf);//--turn on oled panel
    LCD_Fill(0xff);  //初始清屏
    LCD_Set_Pos(0,0);     
} 

/*******************功能描述：显示8*16一组标准ASCII字符串     显示的坐标（x,y），y为页范围0～7****************/
void LCD_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[])
{
    unsigned char c=0,i=0,j=0;
    while (ch[j]!='\0')
    {    
        c =ch[j]-32;
        if(x>120){x=0;y++;}
        LCD_Set_Pos(x,y);    
        for(i=0;i<8;i++)     
            LCD_WrDat(F8X16[c*16+i]);
        LCD_Set_Pos(x,y+1);    
        for(i=0;i<8;i++)     
            LCD_WrDat(F8X16[c*16+i+8]);  
        x+=8;
        j++;
    }
}
void LCD_P8x16Int(unsigned char x, unsigned char y,unsigned int rs)
{
switch (rs){
      case 0:{LCD_P8x16Str(x, y, "0");break;}
      case 1:{LCD_P8x16Str(x, y, "1");break;}
      case 2:{LCD_P8x16Str(x, y, "2");break;}
      case 3:{LCD_P8x16Str(x, y, "3");break;}
      case 4:{LCD_P8x16Str(x, y, "4");break;}
      case 5:{LCD_P8x16Str(x, y, "5");break;}
      case 6:{LCD_P8x16Str(x, y, "6");break;}
      case 7:{LCD_P8x16Str(x, y, "7");break;}
      case 8:{LCD_P8x16Str(x, y, "8");break;}
      case 9:{LCD_P8x16Str(x, y, "9");break;}
      default:{LCD_P8x16Str(x, y, "e");break;}
      }
}
/*****************功能描述：显示16*16点阵  显示的坐标（x,y），y为页范围0～7****************************/
void LCD_P16x16Ch(unsigned char x, unsigned char y, unsigned char N)
{
    unsigned char wm=0;
    unsigned int adder=32*N;  //      
    LCD_Set_Pos(x , y);
    for(wm = 0;wm < 16;wm++)  //             
    {
        LCD_WrDat(F16x16[adder]);    
        adder += 1;
    }      
    LCD_Set_Pos(x,y + 1); 
    for(wm = 0;wm < 16;wm++) //         
    {
        LCD_WrDat(F16x16[adder]);
        adder += 1;
    }           
}

void LCD_title()
{
    int i=0;
    for(i=0; i<7; i++)
    {
      LCD_P16x16Ch(8+16*i, 0, i);
    }
}

void LCD_rSho(){
    int i=0;
    for(i=0; i<4; i++)
    {
      LCD_P16x16Ch(0+16*i, 0, i+7);
    }
}

void LCD_Next(){
    int i=0;
    for(i=0; i<6; i++)
    {
      LCD_P16x16Ch(0+16*i, 6, i+12);
    }
}

void LCD_Temp(){
    int i=0;
    for(i=0; i<3; i++)
    {
      LCD_P16x16Ch(0+16*i, 2, i+18);
    }
}
void LCD_Xl(){
    int i=0;
    for(i=0; i<3; i++)
    {
      LCD_P16x16Ch(0+16*i, 4, i+21);
    }
}
void LCD_RoomNum(unsigned char h,unsigned int rNum){
    if (rNum>9999) rNum = 0;
    int ri,rs;
    for (ri=0;ri<4;ri++){
      rs=rNum%10;
      rNum=(rNum-rNum%10)/10;
      switch (rs){
      case 0:{LCD_P8x16Str(50+8*(4-ri), h, "0");break;}
      case 1:{LCD_P8x16Str(50+8*(4-ri), h, "1");break;}
      case 2:{LCD_P8x16Str(50+8*(4-ri), h, "2");break;}
      case 3:{LCD_P8x16Str(50+8*(4-ri), h, "3");break;}
      case 4:{LCD_P8x16Str(50+8*(4-ri), h, "4");break;}
      case 5:{LCD_P8x16Str(50+8*(4-ri), h, "5");break;}
      case 6:{LCD_P8x16Str(50+8*(4-ri), h, "6");break;}
      case 7:{LCD_P8x16Str(50+8*(4-ri), h, "7");break;}
      case 8:{LCD_P8x16Str(50+8*(4-ri), h, "8");break;}
      case 9:{LCD_P8x16Str(50+8*(4-ri), h, "9");break;}
      default:{LCD_P8x16Str(50+8*(4-ri), h, "e");break;}
      }
    }
}
