#ifndef __MAIN_H
#define	__MAIN_H
#include "sys.h"

//#define USART2_REC_LEN 100
//#define USART3_REC_LEN 100
//extern u8 IRQURA3;//串口3 接受数据完毕标志 
//extern u8 IRQURA2;//串口2 接受数据完毕标志 
//extern u8 USART2_RX_BUF[USART2_REC_LEN];//串口2接受数据反冲去
//extern u8 USART3_RX_BUF[USART3_REC_LEN];//串口3接受数据反冲去
void write_flash(void);
void read_flash(void);
void sendLcdIcNum(u16 num);
 

extern float  CH2OTemp;         //甲醛含量; 
//extern volatile u8 timecount1;//定时器延时
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
