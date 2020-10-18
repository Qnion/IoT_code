extern void delay(unsigned int);
extern void start();        //MLX90614发起始位子程序 
extern void stop();         //MLX90614发结束位子程序 
extern unsigned char ReadByte(void);       //MLX90614接收字节子程序 
extern void send_bit(void);       //MLX90614发送位子程序 
extern void SendByte(unsigned char number);     //MLX90614接收字节子程序 
extern void read_bit(void);       //MLX90614接收位子程序 
extern void delay(unsigned int N);       //延时程序 
extern unsigned int readtemp(void);       //读温度数据 
extern unsigned int wreadtemp(void);       //读温度数据
extern void SDA1(void);
extern void SDA0(void);
extern void SCL1(void);
extern void SCL0(void);