#include <ioCC2530.h>
#include "ALLdef.h"
#include "LCDcmd.h"
#include "MLX.h"
char n_Room[7]={'C','0','0','0','0','N','\0'};
char w_Room4;
char Perror[2]={'0','0'};
unsigned char i_Room[5] = {'0','0','0','0','\0'};
int i_Rdir = 0;
int i;
int cnt=500;
int cnt1=0;
unsigned int alm=0;
unsigned int sNS=0;
unsigned int counter=0;
int wSkip=0;
unsigned int Temp,Temph,Templ;
unsigned int Tem;        //温度变量
unsigned char Temc[6];
unsigned int XL=60;
unsigned char Xlc[4];
unsigned int Pcnt1=0,Pcnt2=0;
unsigned short ReadXL( void ){
  unsigned short reading = 0;
  
  /* Enable channel */
  ADCCFG |= 0x40;
  
  /* writing to this register starts the extra conversion */
  ADCCON3 = 0x84;// AVDD5 引脚  00： 64 抽取率(7 位ENOB)  0100： AIN4
  
  /* Wait for the conversion to be done */
  while (!(ADCCON1 & 0x80));
  
  /* Disable channel after done conversion */
  ADCCFG &= (0x40 ^ 0xFF); //按位异或。如1010^1111=0101（二进制）
  
  /* Read the result */
  reading = ADCL;
  reading |= (short) (ADCH << 8); 
  
  reading >>= 8;
  
  return (reading);
}

void showTem(void)
{
  Tem=readtemp();       //读取温度
  Temp=Tem*2;
  if(Temp>=27315){
    Temp-=27315;
  }
  else{
    Temp=27315-Temp;
  }
  Temph=Temp/100;
  Templ=Temp-Temph*100;
  Temph+=1;
  if(Temph<33){
    LCD_P8x16Str(118,2,"L");
    Pcnt1++;
    if(Pcnt1>=3){
      Perror[0]='1';
      Pcnt1=0;
    }
  }
  else if(Temph>40){
    LCD_P8x16Str(118,2,"H"); 
    Pcnt1++;
    if(Pcnt1>=3){
      Perror[0]='1';
      Pcnt1=0;
    }
  }
  else{
    LCD_P8x16Str(118,2,"N");
    Perror[0]='0';
    Pcnt1=0;
  }
  Temc[0]=str(Temph/10);
  Temc[1]=str(Temph%10);
  Temc[2]='.';
  Temc[3]=str(Templ/10);
  Temc[4]=str(Templ%10);
  Temc[5]='\0';
  LCD_P8x16Str(40,2,Temc);
}

void showXl(void)
{
  cnt++;
  if(cnt%50==0){
    XL += ReadXL();  //读取烟雾传感器引脚上的ad转换值，并没有换算成能表示烟雾浓度的值
    XL = XL >> 1;
  }
  if(cnt>500){
    cnt=0;
    if(XL<60){
      LCD_P8x16Str(118,4,"L");
      Pcnt2++;
      if(Pcnt2>=3){
        Perror[1]='1';
        Pcnt2=0;
      }
    }
    else if(XL>80){
      LCD_P8x16Str(118,4,"H");
      Pcnt2++;
      if(Pcnt2>=3){
        Perror[1]='1';
        Pcnt2=0;
      }
    }
    else{
      LCD_P8x16Str(118,4,"N");
      Perror[1]='0';
      Pcnt2=0;
    }
    
    if(XL>100){
      Xlc[0]=str(XL/100);
      XL=XL%100;
    }
    else{
      Xlc[0]=' ';
    }
    Xlc[1]=str(XL/10);
    Xlc[2]=str(XL%10);
    LCD_P8x16Str(40,4,Xlc);
  }
}

void Perror_rf_send(void)
{
  if((Perror[0]=='1') || (Perror[1]=='1')){
    if(n_Room[5]!='S'){
      n_Room[5]='S';
      rf_send(n_Room,7);
    }
  }
  else{
    if(n_Room[5]!='R'){
      n_Room[5]='R';
      rf_send(n_Room,7);
    }
  }
}

void main(void)
{
  pInit();
  EA = 1;                                     // 使能全局中断
  rf_init();                                  // RF初始化 无帧过滤
  Init_T1();
  LCD_rSho();
  SetRoomNum();
  SCL1();
  SDA1();
  delay(4);
  SCL0();
  delay(1000);
  SCL1();
  LCD_Temp();
  LCD_P16x16Ch(82, 2, 11);
  LCD_Xl();
  LCD_P8x16Str(72,4,"BPM");
  while(1){
    scan_Start();
    scan_Stop();
    NS_Start();
    NS_Stop();
  }
}

void Delay(unsigned int msec)
{ 
    unsigned int i,j;
    for (i=0; i<msec; i++)
        for (j=0; j<1270; j++);
}
void pInit(void)
{
    P0DIR |= 0x61; 
    P1DIR |= 0x8D; 
    P2DIR &= 0xFE;
    LCD_Init();
    sPower=1;
    LCD_CLS();
    sButton = 0;
}
void Init_T1(void)
{
    CLKCONCMD &= ~0x7f;
    while(CLKCONSTA & 0x40);
    T1IE = 1;
    T1CTL = 0x05;
    T1CTL &= ~0x03;
}
/*开始监控*/
void scan_Start(void){
    sPower = 0;
    while(sIn != 0);
    sPower = 1;
    while(alm<2){
      if(sIn == 0){
        Delay(1000);
        alm++;
      }
      if(S2 == 0){
        Delay(1000);
        alm=6;
        wSkip=1;
      }
      showTem();
      showXl();
      Perror_rf_send();
    }
    alm=0;
    sPower = 0;
    while(sIn != 0);
    sPower = 1;
}
/*停止监控，夹住输液管*/
void scan_Stop(void){
  if(wSkip==0)
  {
    pwm_add();
    Delay(200);
    w_Room4=n_Room[5];
    n_Room[5]='N';
    rf_send(n_Room,7);
    n_Room[5]=w_Room4;
    LCD_Next();
    while(S1 != 0){
      showTem();
      showXl();
      Perror_rf_send();
    }
    pwm_dec();
    Delay(200);
  }
  LCD_P8x16Str(0,6,"                ");
  wSkip=0;
}
/*开始盐水冲洗*/
void NS_Start(void){
  sNS=0;
  counter=0;
  T1_Start();
  while(sNS==0){
    Time_show();
    if(S2==0){
      Delay(1000);
      wSkip=1;
      counter=18000;
    }
    showTem();
    showXl();
    Perror_rf_send();
  }
  T1_Stop();
}
/*停止盐水冲洗*/
void NS_Stop(void){
  if(wSkip==0)
  {
  pwm_add();
  Delay(200);
  w_Room4=n_Room[5];
  n_Room[5]='N';
  rf_send(n_Room,7);
  n_Room[5]=w_Room4;
  LCD_Next();
  while(S1 != 0){
    showTem();
    showXl();
    Perror_rf_send();
  }
  pwm_dec();
  Delay(200);
  }
  LCD_P8x16Str(0,6,"                ");
  wSkip=0;
}

/*设置房间号*/
void SetRoomNum(void){
  LCD_P8x16Str(58, 0,i_Room);
  T1_Start();
  i_Rdir = 0;
  while(sButton == 1){
      if (S2==0){
      T1_Stop();
      if(i_Rdir == 0){
          Delay(200);
          if(i_Room[3]<'9'){
            i_Room[3]++;}
          else{
            i_Room[3]='0';}
        LCD_P8x16Str(58, 0,i_Room);
      }
      if(i_Rdir == 1){
          Delay(200);
          if(i_Room[2]<'9'){
            i_Room[2]++;}
          else{
            i_Room[2]='0';}
        LCD_P8x16Str(58, 0,i_Room);
      }
      if(i_Rdir == 2){
          Delay(200);
          if(i_Room[1]<'9'){
            i_Room[1]++;}
          else{
            i_Room[1]='0';}
        LCD_P8x16Str(58, 0,i_Room);
      }
      if(i_Rdir == 3){
          Delay(200);
          if(i_Room[0]<'9'){
            i_Room[0]++;}
          else{
            i_Room[0]='0';
          }
        LCD_P8x16Str(58, 0,i_Room);
      }
      T1_Start();
    }
    
    if(S1==0){
      T1_Stop();
      Delay(200);
      if (i_Rdir == 3)
      {i_Rdir = 0;}
      else
        i_Rdir++;
      T1_Start();
    }
    if(cnt1<20)
      {LCD_P8x16Str(50+8*(4-i_Rdir), 0, "_");}
    else
      {LCD_P8x16Str(58, 0,i_Room);}
  }
  T1_Stop();
  cnt1=1001;
  LCD_P8x16Str(58, 0,i_Room);
  for(i=0;i<4;i++)
    n_Room[i+1]=(char)i_Room[i];
}
void Time_show(void){
  unsigned int mm,ss;
  ss=((18000-counter)/60)%60;
  mm=((18000-counter)/60-ss)/60;
  LCD_P8x16Int(44,6,mm/10);
  LCD_P8x16Int(52,6,mm%10);
  LCD_P8x16Str(60,6,":");
  LCD_P8x16Int(68,6,ss/10);
  LCD_P8x16Int(76,6,ss%10);
}

char str(unsigned int dat){
  switch(dat){
  case 0:return '0';
  case 1:return '1';
  case 2:return '2';
  case 3:return '3';
  case 4:return '4';
  case 5:return '5';
  case 6:return '6';
  case 7:return '7';
  case 8:return '8';
  case 9:return '9';
  default:return ' ';
  }
}

#pragma vector = T1_VECTOR
__interrupt void T1_ISR(void)
{
  IRCON = 0;
    if(counter<18000)
      counter++;
     else
      {
        counter=0;
        sNS=1;
        T1_Stop();
      }
     if(cnt1<1000){
        if(cnt1<50)
          cnt1++;
         else
         {
          cnt1=0;
         }
     }
}