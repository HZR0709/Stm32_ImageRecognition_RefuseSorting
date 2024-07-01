#include "stm32f10x.h"                  // Device header
#include "delay.h"

uint16_t RCValue;
uint16_t FCValue;

void SRF105_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;	
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	TIM_InternalClockConfig(TIM2);
	

	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_Period = 65535;
	TIM_InitStruct.TIM_Prescaler = 71;
	TIM_InitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_InitStruct);
	
	
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICFilter = 0x00;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_BothEdge ;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM2,&TIM_ICInitStruct);
	
	TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2,DISABLE);
}

void SRF105_Trig()
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	delay_us(12);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	TIM_Cmd(TIM2,ENABLE);
	delay_ms(66);
}

void TIM2_IRQHandler()
{
	static uint8_t CCState = 0;
	if(TIM_GetITStatus(TIM2,TIM_IT_CC2) == SET)
	{
		if(CCState == 0)
		{
			RCValue = TIM_GetCapture2(TIM2);
			CCState = 1;
			TIM_ClearITPendingBit(TIM2,TIM_IT_CC2);
		}else if(CCState == 1)
		{
			FCValue = TIM_GetCapture2(TIM2);
			CCState = 0;
			TIM_SetCounter(TIM2,0);
			TIM_Cmd(TIM2,DISABLE);
			TIM_ClearITPendingBit(TIM2,TIM_IT_CC2);
		}
	}
}

float SRF105_EchoDistance()
{
	uint16_t time,SumTime;
	uint8_t Count = 0;
	float Distance;
	while(Count != 5)
	{
		if(FCValue > RCValue)
		time = FCValue - RCValue;
		else time = RCValue - FCValue;
		SumTime += time;
		Count += 1;
	}
	Distance = (float)(SumTime/5)/29.4/2;
	return Distance;
}