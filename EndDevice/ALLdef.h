
#define LED2 P1_1
#define LED3 P0_4

#define S1  P0_1
#define S2  P2_0
#define S4  P1_4
#define sButton P0_7
#define sPower  P0_5
#define sIn     P0_6
#define Servo   P1_0

#define T1_Start() T1CTL |= 0x01
#define T1_Stop() T1CTL &= ~0x03
#define T1_Clear() T1STAT=0
#define T1_Over() (T!STAT&1)

extern void Delay(unsigned int msec);
extern void pInit(void);
extern void Init_T1(void);
/*从flash中读取房间号*/
extern unsigned int ReadFlash(void);
/*将房间号保存到flash中*/
extern void SaveToFlash(unsigned int fData);
/*设置房间号，保存进flash并显示*/
extern void SetRoomNum(void);

/*开始监控*/
extern void scan_Start(void);
/*停止监控，夹住输液管*/
extern void scan_Stop(void);
/*开始盐水冲洗*/
extern void NS_Start(void);
/*停止盐水冲洗*/
extern void NS_Stop(void);

extern void Init_T1(void);

extern void delayus(unsigned int us);
extern void pwm_add();
extern void pwm_dec();

extern void Time_show();

extern void rf_init();

extern void rf_send( char *pbuf , int len);

extern char str(unsigned int dat);

extern void showXl(void);
