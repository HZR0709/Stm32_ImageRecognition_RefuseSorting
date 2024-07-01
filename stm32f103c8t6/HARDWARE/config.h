#ifndef __CONFIG_H
#define __CONFIG_H


#include "../gpio/gpio.h"
#include "../exti/exti.h"
#include "adc.h"
#include "timer.h"
#include "led.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h" 
#include "usart3.h"
#include "SysTick.h"
#include "sys.h"
#include "../bh1750/bh1750.h"
#include "../oled/OLED.h"
#include "../KEY/key.h"
#include "../hcsr04/HCSR04.h"
//#include "../ultrasonic/ultrasonic.h"

/************************* 定义常量 *************************/
//#define Maxtime 60              // 计时变量/秒    60秒无人环境进入待机模式
#define USART2_REC_LEN 100      // 串口2接收缓冲区长度
#define USART3_REC_LEN 100      // 串口3接收缓冲区长度

#define MODE_BACKUP_REG BKP_DR1  // 使用备份寄存器1来存储model变量
#define TIME_BACKUP_REG BKP_DR2
#define TEMP_BACKUP_REG BKP_DR3

/************************* 外部变量声明 *************************/
extern u8 IRQURA3;                         // 串口3 接收数据完毕标志 
extern u8 IRQURA2;                         // 串口2 接收数据完毕标志 
extern u8 USART2_RX_BUF[USART2_REC_LEN];   // 串口2接收数据缓冲区
extern u8 USART3_RX_BUF[USART3_REC_LEN];   // 串口3接收数据缓冲区
extern volatile u8 timecount1;             // 定时器1延时
extern volatile u8 timecount2;             // 舵机恢复计时
extern volatile u8 timecount3;             // 开机计时
extern volatile u8 timecount4;
extern volatile u8 timecount5;

extern volatile uint8_t classify;
extern volatile uint8_t classFlag;

extern volatile uint8_t sensor_last_state;
extern volatile uint8_t model;
extern volatile uint64_t time;

/****************** 中断优先级设置 ******************/
#define USART_PREEMPTION_PRIORITY   3   // 串口中断抢占优先级
#define USART_SUB_PRIORITY          3   // 串口中断子优先级

#define EXTI_PREEMPTION_PRIORITY    1   // 外部中断抢占优先级
#define EXTI_SUB_PRIORITY           1   // 外部中断子优先级

#define TIM2_PREEMPTION_PRIORITY    2   // TIM2中断抢占优先级
#define TIM2_SUB_PRIORITY           2   // TIM2中断子优先级

#define TIM3_PREEMPTION_PRIORITY    4   // TIM3中断抢占优先级
#define TIM3_SUB_PRIORITY           4   // TIM3中断子优先级

/****************** 定义中断向量表索引 ******************/

#define USART2_IRQ_INDEX    USART2_IRQn

#define EXTI_IRQ_INDEX      EXTI0_IRQn

#define TIM2_IRQ_INDEX      TIM2_IRQn

#define TIM3_IRQ_INDEX      TIM3_IRQn

/****************** 超声波模块定义 ******************/

#define Echo1 GPIO_Pin_4		//HC-SR04模块 1 的Echo脚接GPIOB4
#define Trig1 GPIO_Pin_5		//HC-SR04模块 1 的Trig脚接GPIOB5
#define Trig1_GPIO_PORT GPIOB

#define Echo2 GPIO_Pin_8		//HC-SR04模块 2 
#define Trig2 GPIO_Pin_9
#define Trig2_GPIO_PORT GPIOB

#define Echo3 GPIO_Pin_14		//HC-SR04模块 3 
#define Trig3 GPIO_Pin_15
#define Trig3_GPIO_PORT GPIOB

#define Echo4 GPIO_Pin_11		//HC-SR04模块 4 
#define Trig4 GPIO_Pin_12
#define Trig4_GPIO_PORT GPIOA

/****************** 按键定义 ******************/

#define ConfirmKey_PIN      GPIO_Pin_4 // 确认
#define ConfirmKey_GPIO_PORT GPIOA

#define SelectKey_PIN       GPIO_Pin_1 // 选择
#define SelectKey_GPIO_PORT GPIOA

#define EscKey_PIN          GPIO_Pin_6 // 退出
#define EscKey_GPIO_PORT    GPIOA

/****************** LED定义 ******************/

#define LED_PIN             GPIO_Pin_5  // 5V LED灯
#define LED_GPIO_PORT       GPIOA

/****************** 外部中断触发定义 ******************/

#define TRIGGER_PIN         GPIO_Pin_0  			// 外部中断触发引脚
#define TRIGGER_PORT        GPIOA       			// GPIO端口为GPIOA
#define TRIGGER_PORT_SOURCE GPIO_PortSourceGPIOA 	// GPIO端口源为GPIOA
#define TRIGGER_PIN_SOURCE  GPIO_PinSource0     	// GPIO引脚源为PA0

/****************** SG90 舵机定义 ******************/

#define SG90_1_PIN          GPIO_Pin_7
#define SG90_1_GPIO_PORT    GPIOA
#define SG90_2_PIN          GPIO_Pin_1
#define SG90_2_GPIO_PORT    GPIOB
#define SG90_3_PIN          GPIO_Pin_10
#define SG90_3_GPIO_PORT    GPIOB
#define SG90_4_PIN          GPIO_Pin_11
#define SG90_4_GPIO_PORT    GPIOB

#define SG90_1              PAout(7)
#define SG90_2              PBout(1)
#define SG90_3              PBout(10)
#define SG90_4              PBout(11)


#endif
