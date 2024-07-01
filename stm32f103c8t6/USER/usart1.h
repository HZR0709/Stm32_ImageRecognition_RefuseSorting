#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

#define USART_REC_LEN  			200  	//定义最大接收字节数 200

void uart1_init(u32 bound);

void USART1_Config(void);
void UART1Test(void);
int fputc(int ch, FILE *f);
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
void UART1SendByte(unsigned char SendData);
void usart1_SendStringByLen(unsigned char * data,u8 len);
void Delayus(int Time) ;
void Delayms(int time);
#endif /* __USART1_H */
