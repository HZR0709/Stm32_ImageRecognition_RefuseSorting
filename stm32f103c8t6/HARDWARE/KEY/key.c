#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  

/* 
   ����ȷ����ʱ����
   ����:
     nCount: ѭ��������������ʱ
*/
void Delay(__IO u32 nCount)
{
    for (; nCount != 0; nCount--);
}

/* 
   ���ð����õ��� I/O ��
*/
void Key_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // ���������˿� PA �� PB ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // �˿�����Ϊ��������
    GPIO_InitStructure.GPIO_Pin = ConfirmKey_PIN | SelectKey_PIN | EscKey_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // ��ʼ���˿�
}

/* 
   ����Ƿ��а�������
   ����:
     GPIOx: GPIO �˿�
     GPIO_Pin: GPIO ����
   ����ֵ:
     KEY_ON: ��������
     KEY_OFF: û�а�������
*/
u8 Key_Scan(GPIO_TypeDef *GPIOx, u16 GPIO_Pin)
{
    // ����Ƿ��а�������
    if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON)
    {
        // ��ʱ����
        Delay(50000);
        if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON)
        {
            // �ȴ������ͷ�
            while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON);
            return KEY_ON;
        }
        else
            return KEY_OFF;
    }
    else
        return KEY_OFF;
}

