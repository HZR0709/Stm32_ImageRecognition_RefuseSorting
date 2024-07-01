#ifndef __USART2_H
#define	__USART2_H
#include "stm32f10x.h"
#include "../config.h"
//extern uint8_t classify;
extern u8 p1,p2,p3,p4,w1,w2,w3,w4; 

void usart2_init(u32 band);//串口2 初始化函数
void usart2_SendStringByLen(unsigned char * data,u8 len);//
void usart2_SendString(unsigned char * data);
u16 usart2_receive(u32 time);
 
void USART2SendByte(unsigned char SendData);
void getResultTemp(void);
	 
	
	
#endif

