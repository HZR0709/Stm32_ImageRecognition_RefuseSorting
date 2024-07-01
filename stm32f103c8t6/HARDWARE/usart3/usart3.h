
#ifndef __USART3_H
#define	__USART3_H
#include "stm32f10x.h"
#include "../config.h"
 
void usart3_init(u32 band);//串口2 初始化函数
void usart3_SendStringByLen(unsigned char * data,u8 len);//
void usart3_SendString(unsigned char * data);
u16 usart3_receive(u32 time);
void usart3_Send_Data(u8 *buf,u16 len);
void USART3SendByte(unsigned char SendData);
  
#endif



