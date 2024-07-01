#include "sys.h"
#include "main.h"
#include "usart2.h" 
#include "usart3.h" 
#include "delay.h"
#include "led.h"
#include "timer.h"
 
u8 uart2flag = 0;

/*
��ʼ������2
*/
void usart2_init(u32 band)
{
    // GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // ʹ��USART2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //  

    //���¶���ܽ�
    //GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
    //USART2_TX    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);         // ��ʼ�� 

    //USART2_RX	   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // ��ʼ��   

    //Usart NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQ_INDEX;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_PREEMPTION_PRIORITY; // ��ռ���ȼ�3 ԭ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_SUB_PRIORITY;             // �����ȼ�3     ԭ2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                  // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                                                  // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

    //USART ��ʼ������
    USART_InitStructure.USART_BaudRate = band;                        // ���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;         // �ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;             // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    // �շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); // ��ʼ������2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // �������ڽ����ж�
    USART_Cmd(USART2, ENABLE);                    // ʹ�ܴ���2
}

void RS485_Send_Data(u8 *buf, u16 len)
{
    u16 t;

    for (t = 0; t < len; t++) // ѭ����������
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        USART_SendData(USART2, buf[t]);
    }

    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

/*����һ���ֽ�����*/
void USART2SendByte(unsigned char SendData)
{
    USART_SendData(USART2, SendData);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/*
���ݳ����򴮿�2 ��������
*/
void usart2_SendStringByLen(unsigned char *data, u8 len)
{
    u16 i = 0;
    for (i = 0; i < len; i++)
    {
        USART_SendData(USART2, data[i]); //�򴮿�2��������
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET); //�ȴ����ͽ���
    }
}

/*����2 �������ݵ���λ�� data ��\0��β*/
void usart2_SendString(unsigned char *data)
{
    u16 i = 0;
    while (data[i] != '\0')
    {
        USART_SendData(USART2, data[i]); //�򴮿�2��������
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET); //�ȴ����ͽ���
        i++;
    }
}

/*
����2 ��ѯ����ģʽ
*/
u16 usart2_receive(u32 time)
{
    u16 i = 0;
    u32 time0 = 0;
    i = USART_ReceiveData(USART2);
    while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
        delay_us(1);
        time0++;
        if (time0 > time)
        {
            return 0;
        }
    }
    USART2_RX_BUF[0] = USART_ReceiveData(USART2);
    i = 1;
    time0 = 0;
    while (1)
    {
        if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
        {
            time0 = 0;
            USART2_RX_BUF[i] = USART_ReceiveData(USART2);
            i++;
        }
        else
        {
            if (time0 > 50000)
                break;
            else
                time0++;
        }
    }
    USART2_RX_BUF[i] = 0;
    return i;
}

//['��', '�ɻ���', '�к�', '����', '����']   0-1-2-3-4 
void getResultTemp()
{
    static unsigned char cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;
    if (SG90Flag == 0)
    {
        if (strstr((char *)USART2_RX_BUF, "result:1") != NULL) //  
        {
            cnt1++;
            if (cnt1 > 3)
            {
                cnt1 = 0;
                SG90Flag = 1;
                classify = 1;
                SG90_PWM1 = OPEN;  //������Ͱ�ǿɻ������� 
                printf("�ɻ�������\r\n");
            }
            memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
        }
        else if (strstr((char *)USART2_RX_BUF, "result:2") != NULL) //  
        {
            cnt2++;
            if (cnt2 > 3)
            {
                cnt2 = 0;
                SG90Flag = 1;
                classify = 2;
                SG90_PWM2 = OPEN;  //������Ͱ���к�����
                printf("�к�����\r\n");
            }
            memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
        }
        else if (strstr((char *)USART2_RX_BUF, "result:3") != NULL) //  
        {
            cnt3++;
            if (cnt3 > 3)
            {
                cnt3 = 0;
                SG90Flag = 1;
                classify = 3;
                SG90_PWM3 = OPEN;   //������Ͱ�ǳ������� 
                printf("��������\r\n");
            }
            memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
        }
        else if (strstr((char *)USART2_RX_BUF, "result:4") != NULL) //
        {
            cnt4++;
			printf("��������%d\n",cnt4);
            if (cnt4 > 3)
            {
                cnt4 = 0;
                SG90Flag = 1;
                classify = 4;
                SG90_PWM4 = OPEN;   //������Ͱ���������� 
                printf("��������\r\n");
            }
            memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
        }
        else if (strstr((char *)USART2_RX_BUF, "result:0") != NULL) //
        {
            cnt1 = 0;
            cnt2 = 0;
            cnt3 = 0;
            cnt4 = 0;
            printf("������\r\n"); //������ 
            classify = 0;
            memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
        }
    }
}

/*
����2�жϷ������
*/
void USART2_IRQHandler(void)
{
    static u8 i;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART2_RX_BUF[i] = USART_ReceiveData(USART2);
        i++;
        if (i > 10) //���� 
        {
            i = 0;
        }
    }
}


