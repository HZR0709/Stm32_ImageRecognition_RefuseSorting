#ifndef __MAIN_H
#define	__MAIN_H
#include "sys.h"

//#define USART2_REC_LEN 100
//#define USART3_REC_LEN 100
//extern u8 IRQURA3;//����3 ����������ϱ�־ 
//extern u8 IRQURA2;//����2 ����������ϱ�־ 
//extern u8 USART2_RX_BUF[USART2_REC_LEN];//����2�������ݷ���ȥ
//extern u8 USART3_RX_BUF[USART3_REC_LEN];//����3�������ݷ���ȥ
void write_flash(void);
void read_flash(void);
void sendLcdIcNum(u16 num);
 

extern float  CH2OTemp;         //��ȩ����; 
//extern volatile u8 timecount1;//��ʱ����ʱ
//extern volatile u8 timecount2;
//extern volatile u8 timecount3;
//extern volatile u8 timecount4;
//extern volatile u8 timecount5;
extern u8 timeFlag1;
extern u8 timeFlag2;
extern u8 timeFlag3;
extern u8 timeFlag4;
extern u8 timeFlag5;

extern u8 workFlag1;
extern u8 workFlag2;
extern u8 workFlag3;
extern u8 workFlag4;

extern u8 yyFlag;

#endif
