extern void delay(unsigned int);
extern void start();        //MLX90614����ʼλ�ӳ��� 
extern void stop();         //MLX90614������λ�ӳ��� 
extern unsigned char ReadByte(void);       //MLX90614�����ֽ��ӳ��� 
extern void send_bit(void);       //MLX90614����λ�ӳ��� 
extern void SendByte(unsigned char number);     //MLX90614�����ֽ��ӳ��� 
extern void read_bit(void);       //MLX90614����λ�ӳ��� 
extern void delay(unsigned int N);       //��ʱ���� 
extern unsigned int readtemp(void);       //���¶����� 
extern unsigned int wreadtemp(void);       //���¶�����
extern void SDA1(void);
extern void SDA0(void);
extern void SCL1(void);
extern void SCL0(void);