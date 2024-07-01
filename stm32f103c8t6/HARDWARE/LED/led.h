#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "../config.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
 
#define BEEP PAout(5)	//  
#define LED0 PCout(13)	// PC13
 

//#define SG90_1 PAout(7)	  //  	
//#define SG90_2 PBout(1)	  //  
//#define SG90_3 PBout(10)	//  	
//#define SG90_4 PBout(11)	//  
 
void LED_Init(void);//初始化

  
#endif
