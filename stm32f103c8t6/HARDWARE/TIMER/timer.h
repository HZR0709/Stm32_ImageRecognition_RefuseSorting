#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "stm32f10x_tim.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  

#define  OPEN   4   //使舵机处于代表垃圾箱打开状态
#define  CLOSE  2   //使舵机处于代表垃圾箱关闭状态

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
