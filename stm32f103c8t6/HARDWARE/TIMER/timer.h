#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "stm32f10x_tim.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  

#define  OPEN   4   //ʹ������ڴ����������״̬
#define  CLOSE  2   //ʹ������ڴ���������ر�״̬

#define START_TIME  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);TIM_Cmd(TIM3, ENABLE)
#define STOP_TIME  TIM_Cmd(TIM3, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE)

extern u8 SG90_PWM1;
extern u8 SG90_PWM2;
extern u8 SG90_PWM3;
extern u8 SG90_PWM4;
extern u8 SG90Flag;

void BackupModel(void);
void RestoreModel(void);

void Delay_10s(void);
void TIM2_Int_Init(void);
void TIM3_Int_Init(u16 arr,u16 psc);
 
#endif
