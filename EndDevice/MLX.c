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

void start(void)//ֹͣ������ SCL1ʱ��SDA��1��0 
{   SDA1();  
    delay(4);  
    SCL1(); 
    delay(4);  
    SDA0();  
    delay(4);  
    SCL0();  
    delay(4); 
} 

void stop(void)        //ֹͣ������ SCL1ʱ��SDA��0 ��1 
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
//---------����һ���ֽ�--------- 
void SendByte(uchar number) {  
uchar i,n,dat;  n=Nack_number;       //�����ط�����  
Send_again:  dat=number;  
for(i=0;i<8;i++)       //8λ���η���  
{   
  if(dat&0x80)         //ȡ���λ   
  {    
    bit_out=1;            //��1    
  }   
  else   
  {    
    bit_out=0;         //��0   
  }     
  send_bit();           //����һ��λ   
  dat=dat<<1;         //����һλ   
}   
read_bit();          //����1λӦ���ź�  
if(bit_in==1)              //��Ӧ��ʱ�ط�   
{   
  stop();   
  if(n!=0)   { 
      n--;          //�����ط�Nack_number=10��    
      goto Repeat;     //�ط�    
  }   
  else   
  {    
      goto exit;      //�˳�    
  }    
}  
else  
{   
  goto exit;  
}   
Repeat:  start();        //���¿�ʼ  
goto Send_again;      //�ط�  
exit:asm("nop");          //�˳�  
}

//-----------����һ��λ--------- 
void send_bit(void) 
{  
  if(bit_out==1)  
  {   
    SDA1();          //��1   
  }   
  else  
  {   
    SDA0();          //��0   
  }  
  asm("nop"); 
  SCL1();           //������   
  delay(4);delay(4);  
  SCL0();  
  delay(4);delay(4); 
}

uchar ReadByte(void) 
{  
  uchar i,dat;  
  dat=0;         //��ֵΪ0   
  for(i=0;i<8;i++)  
  {   
    dat=dat<<1;        //����    
    read_bit();       //����һλ    
    if(bit_in==1) 
    {    
      dat=dat+1;      //Ϊ1ʱ��Ӧλ��1    
    }    
  }  
  SDA0();         //����Ӧ���ź�0  
  send_bit();  
  return dat;           //���ؽ�������  
}
  
//----------����һ��λ---------- 
void read_bit(void) 
{  
  P1DIR &= SDADIRI;
  SDA1();         //���ݶ�����1  
  bit_in=1;  
  SCL1();         //������  
  delay(4);delay(4);  
  bit_in=SDA;           //������   
  asm("nop"); 
  SCL0();  
  delay(4);
  delay(4); 
} 

uint readtemp(void) 
{  
  SCL0();  
  start();          //��ʼ����  
  SendByte(0x00);       //���ʹӵ�ַ00  
  SendByte(0x07);       //��������  
  start();         //��ʼ����  
  SendByte(0x01);       //���ӵ�ַ00  
  bit_out=0;  
  tempL=ReadByte();      //�����ݵ��ֽ�  
  bit_out=0;  
  tempH=ReadByte();      //�����ݸ��ֽ�  
  bit_out=1;  
  err=ReadByte();       //��������Ϣ��  
  stop();            //ֹͣ����   
  return(tempH*256+tempL); 
}

uint wreadtemp(void) 
{  
  SCL0(); 
  start();         //��ʼ����  
  SendByte(0x00);       //���ʹӵ�ַ00  
  SendByte(0x06);       //��������  
  start();         //��ʼ����  
  SendByte(0x01);       //���ӵ�ַ00  
  bit_out=0;  
  tempL=ReadByte();      //�����ݵ��ֽ�  
  bit_out=0;  
  tempH=ReadByte();      //�����ݸ��ֽ�  
  bit_out=1;  
  err=ReadByte();       //��������Ϣ��  
  stop();            //ֹͣ����   
  return(tempH*256+tempL); 
} 
 
