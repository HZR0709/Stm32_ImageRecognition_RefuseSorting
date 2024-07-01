#include "../exti/exti.h"

extern int Maxtime;
extern float tempFlag;

void EXTI0_Config(void) {
	
	GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // 使能 GPIOA 和 AFIO 时钟

    // 配置 PA0 为输入浮空模式
    GPIO_InitStructure.GPIO_Pin = TRIGGER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(TRIGGER_PORT, &GPIO_InitStructure);

    // 连接 EXTI Line 到 PA0 引脚
    GPIO_EXTILineConfig(TRIGGER_PORT_SOURCE, TRIGGER_PIN_SOURCE);

    // 配置 EXTI Line0
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 设置为上升沿和下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 配置 NVIC
    NVIC_InitStructure.NVIC_IRQChannel = EXTI_IRQ_INDEX;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_PREEMPTION_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_SUB_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void Enter_Standby_Mode(void) 
{
	BackupModel(); // 备份model变量

    // 关闭所有不需要的时钟，减少功耗

	// 关闭GPIO端口的时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, DISABLE);

//	// 关闭ADC1的时钟和GPIOA时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , DISABLE);

//	// 关闭TIM3的时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2, DISABLE);

//	// 关闭USART1的时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
//	
//	// 关闭USART2的时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
//	
//	// ...以此类推，根据你的需要关闭其他外设时钟


    // 进入待机模式
    PWR_EnterSTANDBYMode();
}
