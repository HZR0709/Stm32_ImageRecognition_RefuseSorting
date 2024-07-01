#include "../exti/exti.h"

extern int Maxtime;
extern float tempFlag;

void EXTI0_Config(void) {
	
	GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // ʹ�� GPIOA �� AFIO ʱ��

    // ���� PA0 Ϊ���븡��ģʽ
    GPIO_InitStructure.GPIO_Pin = TRIGGER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(TRIGGER_PORT, &GPIO_InitStructure);

    // ���� EXTI Line �� PA0 ����
    GPIO_EXTILineConfig(TRIGGER_PORT_SOURCE, TRIGGER_PIN_SOURCE);

    // ���� EXTI Line0
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // ����Ϊ�����غ��½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // ���� NVIC
    NVIC_InitStructure.NVIC_IRQChannel = EXTI_IRQ_INDEX;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI_PREEMPTION_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXTI_SUB_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void Enter_Standby_Mode(void) 
{
	BackupModel(); // ����model����

    // �ر����в���Ҫ��ʱ�ӣ����ٹ���

	// �ر�GPIO�˿ڵ�ʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, DISABLE);

//	// �ر�ADC1��ʱ�Ӻ�GPIOAʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , DISABLE);

//	// �ر�TIM3��ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2, DISABLE);

//	// �ر�USART1��ʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
//	
//	// �ر�USART2��ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
//	
//	// ...�Դ����ƣ����������Ҫ�ر���������ʱ��


    // �������ģʽ
    PWR_EnterSTANDBYMode();
}
