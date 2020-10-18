#include "ioCC2530.h"
#include <stdio.h>
#include <string.h>

#define LED1  P1_0
#define LED2  P1_1
#define S1    P0_1
#define S2    P2_0
char rf_rx_buf[128];

void rf_init()
{
    TXPOWER   = 0xD5;                 // 发射功率为1dBm
    CCACTRL0  = 0xF8;                 // 推荐值 smartRF软件生成

    FRMFILT0  = 0x0C;                 // 静止接收过滤，即接收所有数据包

    FSCAL1 =    0x00;                 // 推荐值 smartRF软件生成
    TXFILTCFG = 0x09;
    AGCCTRL1 =  0x15;
    AGCCTRL2 =  0xFE;

    TXFILTCFG = 0x09;                 // 推荐值 smartRF软件生成

    FREQCTRL = 0x0B;                  // 选择通道11

    RFIRQM0 |= (1<<6);                // 使能RF数据包接收中断
    IEN2 |= (1<<0);                   // 使能RF中断

    RFST = 0xED;                      // 清除RF接收缓冲区 ISFLUSHRX
    RFST = 0xE3;                      // RF接收使能 ISRXON
}

void rf_send( char *pbuf , int len)
{
    RFST = 0xE3;                      // RF接收使能 ISRXON
    // 等待发送状态不活跃 并且 没有接收到SFD
    while( FSMSTAT1 & (( 1<<1 ) | ( 1<<5 )));

    RFIRQM0 &= ~(1<<6);               // 禁止接收数据包中断
    IEN2 &= ~(1<<0);                  // 清除RF全局中断

    RFST = 0xEE;                      // 清除发送缓冲区 ISFLUSHTX
    RFIRQF1 = ~(1<<1);                // 清除发送完成标志

    // 填充缓冲区 填充过程需要增加2字节，CRC校验自动填充
    RFD = len + 2;
    for (int i = 0; i < len; i++)
    {
        RFD = *pbuf++;
    }

    RFST = 0xE9;                      // 发送数据包 ISTXON
    while (!(RFIRQF1 & (1<<1)));      // 等待发送完成
    RFIRQF1 = ~(1<<1);                // 清除发送完成标志位

    RFIRQM0 |= (1<<6);                // RX接收中断
    IEN2 |= (1<<0);
}

#pragma vector=RF_VECTOR
__interrupt void rf_isr(void)
{
    EA = 0;

    // 接收到一个完整的数据包
    if (RFIRQF0 & ( 1<<6 ))
    {
        RFST = 0xED;                                // 清除接收缓冲区

        S1CON = 0;                                  // 清除RF中断标志
        RFIRQF0 &= ~(1<<6);                         // 清除RF接收完成数据包中断
    }
    EA = 1;
}