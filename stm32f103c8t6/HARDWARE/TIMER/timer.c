#include "timer.h"
#include "led.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	
// ������PWM�źŹر�״̬
u8 SG90_PWM1 = CLOSE;
u8 SG90_PWM2 = CLOSE;
u8 SG90_PWM3 = CLOSE;
u8 SG90_PWM4 = CLOSE;
// �������������Ͱһ�α�־
u8 SG90Flag = 0;
extern volatile uint8_t model;
int Maxtime = 30;
extern float tempFlag;
// �ⲿ��������
//extern volatile uint8_t sensor_last_state;
//extern uint8_t model;
extern volatile uint64_t time;
extern float temp;

void BackupModel(void) 
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); // ʹ��PWR��BKPʱ��
    PWR_BackupAccessCmd(ENABLE); // ����������д����
    BKP_WriteBackupRegister(MODE_BACKUP_REG, model); // ��model������ֵд�뱸�ݼĴ���
	BKP_WriteBackupRegister(TIME_BACKUP_REG, Maxtime);
	BKP_WriteBackupRegister(TEMP_BACKUP_REG, tempFlag);
}

void RestoreModel(void) 
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); // ʹ��PWR��BKPʱ��
    model = (uint8_t)BKP_ReadBackupRegister(MODE_BACKUP_REG); // �ӱ��ݼĴ�����ȡmodel������ֵ
	Maxtime = (uint8_t)BKP_ReadBackupRegister(TIME_BACKUP_REG);
	tempFlag = (uint8_t)BKP_ReadBackupRegister(TEMP_BACKUP_REG);
}


// TIM2��ʱ����ʼ��
void TIM2_Int_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;			//����ṹ�壬���ö�ʱ��
	NVIC_InitTypeDef NVIC_InitStructure;						//����ṹ�壬�����ж����ȼ�
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		//����TIM3ʱ��
																// https://blog.zeruns.tech
	TIM_InternalClockConfig(TIM2);								//����TIM3ʹ���ڲ�ʱ��
	
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//����1��Ƶ������Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ü���ģʽΪ���ϼ���
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;			//����������ֵ���ﵽ���ֵ���������¼�����Ϊ��0��ʼ���������Լ���10����10-1,ÿ10΢�봥��һ��
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;			//����ʱ��Ԥ��Ƶ��72-1����ÿ ʱ��Ƶ��(72Mhz)/72=1000000 ��ʱ�����ڼ�������1,ÿ1΢��+1
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;		//�ظ����������߼���ʱ�����У���������0��
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);			//��ʼ��TIM3��ʱ��
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);			//��������жϱ�־λ
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);		//���������ж�
		
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//ָ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//�ж�ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM2_PREEMPTION_PRIORITY;	//������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM2_SUB_PRIORITY;			//������Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);							//������ʱ��
}

// TIM3��ʱ����ʼ��
// arr���Զ���װֵ
// psc��ʱ��Ԥ��Ƶ��
void TIM3_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // ʱ��ʹ��

    // ��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr; // ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc; // ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // ����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // ����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // ʹ��ָ����TIM3�ж�,��������ж�

    // �ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQ_INDEX; // TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM3_PREEMPTION_PRIORITY; // ��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM3_SUB_PRIORITY; // �����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure); // ��ʼ��NVIC�Ĵ���

    TIM_Cmd(TIM3, ENABLE); // ʹ��TIMx
}

void TIM2_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{		
  		time++;
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);    
	}		 	
}

// TIM3��ʱ���жϷ������
void TIM3_IRQHandler(void)
{
    static u16 count = 0, count2 = 0;
    uint8_t sensor_current_state = 0;

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // ���TIM3�����жϷ������
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ���TIMx�����жϱ�־
        LED0 = ~LED0;

        if (SG90Flag == 1 && model == 1)
        {
            count++; // ������
            if (count > 2000) // 1��
            {
                count = 0;
                timecount2++;
            }
            if (timecount2 > 3)
            {
                SG90Flag = 0;
//                BEEP = 0; // �رշ�����
                SG90_PWM1 = CLOSE; // �ر�����Ͱ��
                SG90_PWM2 = CLOSE; // �ر�����Ͱ��
                SG90_PWM3 = CLOSE; // �ر�����Ͱ��
                SG90_PWM4 = CLOSE; // �ر�����Ͱ��
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
            if (sensor_current_state == Bit_SET) { // ��⵽��
                count2 = 0;
                timecount3 = 0;
            }
            else { // ���뿪
                count2++;
                if (count2 > 2000) {//2000Ϊ1��
                    printf("timecount3:%d\n", timecount3);
                    timecount3++;
					timecount4++;
                    count2 = 0;
                }
                if (timecount3 > Maxtime) {
					GPIO_ResetBits(LED_GPIO_PORT, LED_PIN);
					PWR_ClearFlag(PWR_FLAG_WU);
                    PWR_WakeUpPinCmd(ENABLE);
                    Enter_Standby_Mode();//����ģʽ
                }
            }
        }
	} 
}










