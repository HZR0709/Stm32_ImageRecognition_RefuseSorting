/**************************************
 * �ļ���  ��SysTick.c
 * ����    ��SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
 *           ���õ��� 1us 10us 1ms �жϡ�         
 * ʵ��ƽ̨��MINI STM32������ ����STM32F103C8T6
 * Ӳ�����ӣ���
 * ��汾  ��ST3.0.0  

*********************************************************/

#include "SysTick.h"

static __IO u32 TimingDelay;

/*��ʼ��  SysTick*/
void SysTick_Init(void)
{
	/* SystemFrequency / 1000SysTick    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
}


/*us��ʱ����,10usΪһ����λ */
//void Delay_us(__IO u32 nTime)
//{ 
//  TimingDelay = nTime;

//  while(TimingDelay != 0);
//}

/*us��ʱ����,1msΪһ����λ */
void Delay_ms(__IO u32 nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


/* ��ȡ���ĳ����� SysTick �жϺ��� SysTick_Handler()����	 */  
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

