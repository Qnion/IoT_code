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
    TXPOWER   = 0xD5;                 // ���书��Ϊ1dBm
    CCACTRL0  = 0xF8;                 // �Ƽ�ֵ smartRF�������

    FRMFILT0  = 0x0C;                 // ��ֹ���չ��ˣ��������������ݰ�

    FSCAL1 =    0x00;                 // �Ƽ�ֵ smartRF�������
    TXFILTCFG = 0x09;
    AGCCTRL1 =  0x15;
    AGCCTRL2 =  0xFE;

    TXFILTCFG = 0x09;                 // �Ƽ�ֵ smartRF�������

    FREQCTRL = 0x0B;                  // ѡ��ͨ��11

    RFIRQM0 |= (1<<6);                // ʹ��RF���ݰ������ж�
    IEN2 |= (1<<0);                   // ʹ��RF�ж�

    RFST = 0xED;                      // ���RF���ջ����� ISFLUSHRX
    RFST = 0xE3;                      // RF����ʹ�� ISRXON
}

void rf_send( char *pbuf , int len)
{
    RFST = 0xE3;                      // RF����ʹ�� ISRXON
    // �ȴ�����״̬����Ծ ���� û�н��յ�SFD
    while( FSMSTAT1 & (( 1<<1 ) | ( 1<<5 )));

    RFIRQM0 &= ~(1<<6);               // ��ֹ�������ݰ��ж�
    IEN2 &= ~(1<<0);                  // ���RFȫ���ж�

    RFST = 0xEE;                      // ������ͻ����� ISFLUSHTX
    RFIRQF1 = ~(1<<1);                // ���������ɱ�־

    // ��仺���� ��������Ҫ����2�ֽڣ�CRCУ���Զ����
    RFD = len + 2;
    for (int i = 0; i < len; i++)
    {
        RFD = *pbuf++;
    }

    RFST = 0xE9;                      // �������ݰ� ISTXON
    while (!(RFIRQF1 & (1<<1)));      // �ȴ��������
    RFIRQF1 = ~(1<<1);                // ���������ɱ�־λ

    RFIRQM0 |= (1<<6);                // RX�����ж�
    IEN2 |= (1<<0);
}

#pragma vector=RF_VECTOR
__interrupt void rf_isr(void)
{
    EA = 0;

    // ���յ�һ�����������ݰ�
    if (RFIRQF0 & ( 1<<6 ))
    {
        RFST = 0xED;                                // ������ջ�����

        S1CON = 0;                                  // ���RF�жϱ�־
        RFIRQF0 &= ~(1<<6);                         // ���RF����������ݰ��ж�
    }
    EA = 1;
}