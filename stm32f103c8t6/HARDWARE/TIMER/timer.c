#include "timer.h"
#include "led.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	
// 定义舵机PWM信号关闭状态
u8 SG90_PWM1 = CLOSE;
u8 SG90_PWM2 = CLOSE;
u8 SG90_PWM3 = CLOSE;
u8 SG90_PWM4 = CLOSE;
// 舵机触发打开垃圾桶一次标志
u8 SG90Flag = 0;
extern volatile uint8_t model;
int Maxtime = 30;
extern float tempFlag;
// 外部变量声明
//extern volatile uint8_t sensor_last_state;
//extern uint8_t model;
extern volatile uint64_t time;
extern float temp;

void BackupModel(void) 
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); // 使能PWR和BKP时钟
    PWR_BackupAccessCmd(ENABLE); // 解除备份域的写保护
    BKP_WriteBackupRegister(MODE_BACKUP_REG, model); // 将model变量的值写入备份寄存器
	BKP_WriteBackupRegister(TIME_BACKUP_REG, Maxtime);
	BKP_WriteBackupRegister(TEMP_BACKUP_REG, tempFlag);
}

void RestoreModel(void) 
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); // 使能PWR和BKP时钟
    model = (uint8_t)BKP_ReadBackupRegister(MODE_BACKUP_REG); // 从备份寄存器读取model变量的值
	Maxtime = (uint8_t)BKP_ReadBackupRegister(TIME_BACKUP_REG);
	tempFlag = (uint8_t)BKP_ReadBackupRegister(TEMP_BACKUP_REG);
}


// TIM2定时器初始化
void TIM2_Int_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;			//定义结构体，配置定时器
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体，配置中断优先级
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		//启用TIM3时钟
																// https://blog.zeruns.tech
	TIM_InternalClockConfig(TIM2);								//设置TIM3使用内部时钟
	
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//设置1分频（不分频）
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//设置计数模式为向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;			//设置最大计数值，达到最大值触发更新事件，因为从0开始计数，所以计数10次是10-1,每10微秒触发一次
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;			//设置时钟预分频，72-1就是每 时钟频率(72Mhz)/72=1000000 个时钟周期计数器加1,每1微秒+1
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;		//重复计数器（高级定时器才有，所以设置0）
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);			//初始化TIM3定时器
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);			//清除更新中断标志位
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);		//开启更新中断
		
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//指定中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//中断使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM2_PREEMPTION_PRIORITY;	//设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM2_SUB_PRIORITY;			//设置响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);							//开启定时器
}

// TIM3定时器初始化
// arr：自动重装值
// psc：时钟预分频数
void TIM3_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 时钟使能

    // 定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr; // 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc; // 设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // 根据指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // 使能指定的TIM3中断,允许更新中断

    // 中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQ_INDEX; // TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM3_PREEMPTION_PRIORITY; // 先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM3_SUB_PRIORITY; // 从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure); // 初始化NVIC寄存器

    TIM_Cmd(TIM3, ENABLE); // 使能TIMx
}

void TIM2_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{		
  		time++;
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);    
	}		 	
}

// TIM3定时器中断服务程序
void TIM3_IRQHandler(void)
{
    static u16 count = 0, count2 = 0;
    uint8_t sensor_current_state = 0;

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // 检查TIM3更新中断发生与否
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除TIMx更新中断标志
        LED0 = ~LED0;

        if (SG90Flag == 1 && model == 1)
        {
            count++; // 计数器
            if (count > 2000) // 1秒
            {
                count = 0;
                timecount2++;
            }
            if (timecount2 > 3)
            {
                SG90Flag = 0;
//                BEEP = 0; // 关闭蜂鸣器
                SG90_PWM1 = CLOSE; // 关闭垃圾桶盖
                SG90_PWM2 = CLOSE; // 关闭垃圾桶盖
                SG90_PWM3 = CLOSE; // 关闭垃圾桶盖
                SG90_PWM4 = CLOSE; // 关闭垃圾桶盖
            }
        }
        else
        {
            count = 0;
            timecount2 = 0;
        }

        timecount1++;
        if (timecount1 < SG90_PWM1)
            SG90_1 = 1;		//PAout(7)
        else if (timecount1 < 40)
            SG90_1 = 0;
        else
            timecount1 = 0;

        if (timecount1 < SG90_PWM2)
            SG90_2 = 1;		//PBout(1)
        else if (timecount1 < 40)
            SG90_2 = 0;
        else
            timecount1 = 0;

        if (timecount1 < SG90_PWM3)
            SG90_3 = 1;		//PBout(10)
        else if (timecount1 < 40)
            SG90_3 = 0;
        else
            timecount1 = 0;

        if (timecount1 < SG90_PWM4)
            SG90_4 = 1;		//PBout(11)
        else if (timecount1 < 40)
            SG90_4 = 0;
        else
            timecount1 = 0;

        if (model == 1)
        {	
            sensor_current_state = GPIO_ReadInputDataBit(TRIGGER_PORT, TRIGGER_PIN); 
            if (sensor_current_state == Bit_SET) { // 检测到人
                count2 = 0;
                timecount3 = 0;
            }
            else { // 人离开
                count2++;
                if (count2 > 2000) {//2000为1秒
                    printf("timecount3:%d\n", timecount3);
                    timecount3++;
					timecount4++;
                    count2 = 0;
                }
                if (timecount3 > Maxtime) {
					GPIO_ResetBits(LED_GPIO_PORT, LED_PIN);
					PWR_ClearFlag(PWR_FLAG_WU);
                    PWR_WakeUpPinCmd(ENABLE);
                    Enter_Standby_Mode();//待机模式
                }
            }
        }
	} 
}










