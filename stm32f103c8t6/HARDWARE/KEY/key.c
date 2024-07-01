#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  

/* 
   不精确的延时函数
   参数:
     nCount: 循环次数，用于延时
*/
void Delay(__IO u32 nCount)
{
    for (; nCount != 0; nCount--);
}

/* 
   配置按键用到的 I/O 口
*/
void Key_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启按键端口 PA 和 PB 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // 端口配置为上拉输入
    GPIO_InitStructure.GPIO_Pin = ConfirmKey_PIN | SelectKey_PIN | EscKey_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化端口
}

/* 
   检测是否有按键按下
   参数:
     GPIOx: GPIO 端口
     GPIO_Pin: GPIO 引脚
   返回值:
     KEY_ON: 按键按下
     KEY_OFF: 没有按键按下
*/
u8 Key_Scan(GPIO_TypeDef *GPIOx, u16 GPIO_Pin)
{
    // 检测是否有按键按下
    if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON)
    {
        // 延时消抖
        Delay(50000);
        if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON)
        {
            // 等待按键释放
            while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON);
            return KEY_ON;
        }
        else
            return KEY_OFF;
    }
    else
        return KEY_OFF;
}

