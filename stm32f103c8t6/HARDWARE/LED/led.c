#include "led.h"
#include "delay.h"
#include "main.h"
#include "codetab.h" 
 
//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
/*
led pe9
*/
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PB�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11;	// 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.12

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	// 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 // 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� �� 
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 // ������ led
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50
 GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� ��  
 
}
