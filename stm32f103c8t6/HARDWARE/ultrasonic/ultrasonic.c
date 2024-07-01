#include "../ultrasonic/ultrasonic.h"

volatile uint32_t usTicks;  // Œ¢√Îº∂º∆ ±

void Delay_us(uint32_t us)
{
    usTicks = 0;
    while (usTicks < us);
}

//void Ultrasonic_Init(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;

//    // TRIG pin as output
//    GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    // ECHO pin as input
//    GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Input pull-down
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    // Timer configuration for microsecond precision
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
//    TIM_TimeBaseStructure.TIM_Period = 65535;
//    TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 1000000) - 1;  // 1 MHz, 1 tick = 1 us
//    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
//    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

//    // Enable the TIM4 global Interrupt
//    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}

uint32_t Ultrasonic_GetDistance(void)
{
	uint32_t duration = 0;
    // Send 10us pulse to TRIG pin
    GPIO_SetBits(GPIOA, TRIG_PIN);
	
    Delay_us(10);  // Delay for 10 microseconds using your provided SysTick function
	
    GPIO_ResetBits(GPIOA, TRIG_PIN);
	
    TIM_Cmd(TIM4, ENABLE);  // Start the timer
    while (!GPIO_ReadInputDataBit(GPIOA, ECHO_PIN)) {
        if (usTicks > 1000) {  // Timeout after 1 ms if no echo
            TIM_Cmd(TIM4, DISABLE);
            return 0;
        }
    }
	;
    TIM_SetCounter(TIM4, 0);  // Reset counter when echo starts

    while (GPIO_ReadInputDataBit(GPIOA, ECHO_PIN));  // Wait for echo to end

    duration = TIM_GetCounter(TIM4);
    TIM_Cmd(TIM4, DISABLE);  // Stop the timer
	
    // Calculate distance (speed of sound is approximately 340 m/s)
    return duration * 0.034 / 2;
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        usTicks++;  // Increment microsecond ticks
    }
}

