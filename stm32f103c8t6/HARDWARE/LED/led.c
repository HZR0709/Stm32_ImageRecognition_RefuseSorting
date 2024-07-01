#include "led.h"
#include "delay.h"
#include "main.h"
#include "codetab.h" 
 
//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
/*
led pe9
*/
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PB端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11;	// 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.12

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	// 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 // 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ， 
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 // 蜂鸣器 led
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，  
 
}
