#include <iocc2530.h>
int pos = 500;

void delayus(unsigned int us){
  unsigned int i;
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
  for(i=us;i>0;i--);
}

void pwm_add()
{
    for(pos = 190 ;pos >80 ;pos-=4){
      P1_0 = 1;
      delayus(pos);
      P1_0 = 0;
      delayus(2000-pos);
    }
}
void pwm_dec()
{
    for(pos = 100 ;pos < 210 ;pos+=4){
      P1_0 = 1;
      delayus(pos);
      P1_0 = 0;
      delayus(2000-pos);
    }
}