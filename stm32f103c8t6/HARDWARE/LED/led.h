#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "../config.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
 
#define BEEP PAout(5)	//  
#define LED0 PCout(13)	// PC13
 

//#define SG90_1 PAout(7)	  //  	
//#define SG90_2 PBout(1)	  //  
//#define SG90_3 PBout(10)	//  	
//#define SG90_4 PBout(11)	//  
 
void LED_Init(void);//��ʼ��

  
#endif
