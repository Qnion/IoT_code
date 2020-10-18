#include "iocc2530.h"
#include <stdio.h>
#include <string.h>
#include "MLX.h"

#define  uint  unsigned int
#define  uchar unsigned char
#define Nack_number 10
#define SCL P1_4 //P1DIR |= 0x10;
#define SDA P1_5 //P1DIR |= 0x20; P1DIR &= 0xDF;
#define SCLDIR 0x10
#define SDADIR 0x20
#define SDADIRI 0xDF
int bit_out=0,bit_in=0; 
uchar tempH,tempL,err; 

void delay(uint n)
{
  uint j;  
  for(j=0;j<n;j++)  
  {   
    asm("nop"); 
  } 
}

void SDA1(void){
  P1DIR |= SDADIR;
  SDA=1;
}

void SDA0(void){
  P1DIR |= SDADIR;
  SDA=0;
}

void SCL1(void){
  P1DIR |= SCLDIR;
  SCL=1;
}

void SCL0(void){
  P1DIR |= SCLDIR;
  SCL=0;
}

void start(void)//停止条件是 SCL1时，SDA由1到0 
{   SDA1();  
    delay(4);  
    SCL1(); 
    delay(4);  
    SDA0();  
    delay(4);  
    SCL0();  
    delay(4); 
} 

void stop(void)        //停止条件是 SCL1时，SDA由0 到1 
{   
    SDA0();  
    delay(4);  
    SCL1();  
    delay(4);  
    SDA1();  
    delay(4);  
    SCL0(); 
    delay(4);
}
//---------发送一个字节--------- 
void SendByte(uchar number) {  
uchar i,n,dat;  n=Nack_number;       //可以重发次数  
Send_again:  dat=number;  
for(i=0;i<8;i++)       //8位依次发送  
{   
  if(dat&0x80)         //取最高位   
  {    
    bit_out=1;            //发1    
  }   
  else   
  {    
    bit_out=0;         //发0   
  }     
  send_bit();           //发送一个位   
  dat=dat<<1;         //左移一位   
}   
read_bit();          //接收1位应答信号  
if(bit_in==1)              //无应答时重发   
{   
  stop();   
  if(n!=0)   { 
      n--;          //可以重发Nack_number=10次    
      goto Repeat;     //重发    
  }   
  else   
  {    
      goto exit;      //退出    
  }    
}  
else  
{   
  goto exit;  
}   
Repeat:  start();        //重新开始  
goto Send_again;      //重发  
exit:asm("nop");          //退出  
}

//-----------发送一个位--------- 
void send_bit(void) 
{  
  if(bit_out==1)  
  {   
    SDA1();          //发1   
  }   
  else  
  {   
    SDA0();          //发0   
  }  
  asm("nop"); 
  SCL1();           //上升沿   
  delay(4);delay(4);  
  SCL0();  
  delay(4);delay(4); 
}

uchar ReadByte(void) 
{  
  uchar i,dat;  
  dat=0;         //初值为0   
  for(i=0;i<8;i++)  
  {   
    dat=dat<<1;        //左移    
    read_bit();       //接收一位    
    if(bit_in==1) 
    {    
      dat=dat+1;      //为1时对应位加1    
    }    
  }  
  SDA0();         //发送应答信号0  
  send_bit();  
  return dat;           //带回接收数据  
}
  
//----------接收一个位---------- 
void read_bit(void) 
{  
  P1DIR &= SDADIRI;
  SDA1();         //数据端先置1  
  bit_in=1;  
  SCL1();         //上升沿  
  delay(4);delay(4);  
  bit_in=SDA;           //读数据   
  asm("nop"); 
  SCL0();  
  delay(4);
  delay(4); 
} 

uint readtemp(void) 
{  
  SCL0();  
  start();          //开始条件  
  SendByte(0x00);       //发送从地址00  
  SendByte(0x07);       //发送命令  
  start();         //开始条件  
  SendByte(0x01);       //读从地址00  
  bit_out=0;  
  tempL=ReadByte();      //读数据低字节  
  bit_out=0;  
  tempH=ReadByte();      //读数据高字节  
  bit_out=1;  
  err=ReadByte();       //读错误信息码  
  stop();            //停止条件   
  return(tempH*256+tempL); 
}

uint wreadtemp(void) 
{  
  SCL0(); 
  start();         //开始条件  
  SendByte(0x00);       //发送从地址00  
  SendByte(0x06);       //发送命令  
  start();         //开始条件  
  SendByte(0x01);       //读从地址00  
  bit_out=0;  
  tempL=ReadByte();      //读数据低字节  
  bit_out=0;  
  tempH=ReadByte();      //读数据高字节  
  bit_out=1;  
  err=ReadByte();       //读错误信息码  
  stop();            //停止条件   
  return(tempH*256+tempL); 
} 
 
