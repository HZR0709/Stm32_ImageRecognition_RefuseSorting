/***************************************
 * �ļ���  ��usart1.c
 * ����    ������USART1         
 * ʵ��ƽ̨��MINI STM32������ ����STM32F103C8T6
 * Ӳ�����ӣ�------------------------
 *          | PA9  - USART1(Tx)      |
 *          | PA10 - USART1(Rx)      |
 *           ------------------------
 * ��汾  ��ST3.0.0  

**********************************************************************************/
#include "sys.h"
#include "usart1.h"
#include <stdarg.h>

// ���ջ���,���USART_REC_LEN���ֽ�.
u8 USART_RX_BUF[USART_REC_LEN];
// ����״̬
// bit15��	������ɱ�־   
// bit14��	���յ�0x0d
// bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0; // ����״̬���

// UART1��ʼ��
void uart1_init(u32 bound)
{
    // GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); // ʹ��USART1��GPIOAʱ��

    // USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);         // ��ʼ��GPIOA.9

    // USART1_RX	  GPIOA.10��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                   // PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;        // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                       // ��ʼ��GPIOA.10

    // USART1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

    // USART ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;                        // ���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;         // �ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;             // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    // �շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); // ��ʼ������1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // �������ڽ����ж�
    USART_Cmd(USART1, ENABLE);                      // ʹ�ܴ���1
}

// ����1�жϷ������
void USART1_IRQHandler(void)
{
    u8 Res;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        Res = USART_ReceiveData(USART1);    // ��ȡ���յ�������

        if ((USART_RX_STA & 0x8000) == 0) // ����δ���
        {
            if (USART_RX_STA & 0x4000) // ���յ���0x0d
            {
                if (Res != 0x0a)
                    USART_RX_STA = 0;   // ���մ���,���¿�ʼ
                else
                    USART_RX_STA |= 0x8000; // ��������� 
            }
            else // ��û�յ�0X0D
            {
                if (Res == 0x0d)
                    USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))
                        USART_RX_STA = 0; // �������ݴ���,���¿�ʼ����	  
                }
            }
        }
    }
}

// ���ݳ����򴮿�3 ��������
void usart1_SendStringByLen(unsigned char *data, u8 len)
{
    u16 i = 0;
    for (i = 0; i < len; i++)
    {
        USART_SendData(USART1, data[i]); // �򴮿�1��������
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET); // �ȴ����ͽ���
    }
}

// �ض���c�⺯��printf��USART2
int fputc(int ch, FILE *f)
{
    // ��Printf���ݷ�������
    USART_SendData(USART1, (unsigned char)ch);
    while (!(USART1->SR & USART_FLAG_TXE));

    return (ch);
}

// ����һ���ֽ�����
void UART1SendByte(unsigned char SendData)
{
    USART_SendData(USART1, SendData);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

// ����һ���ֽ�����
unsigned char UART1GetByte(unsigned char *GetData)
{
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
    {
        return 0; // û���յ����� 
    }
    *GetData = USART_ReceiveData(USART1);
    return 1; // �յ�����
}

// ����һ�����ݣ����Ϸ��ؽ��յ����������
void UART1Test(void)
{
    unsigned char i = 0;

    while (1)
    {
        while (UART1GetByte(&i))
        {
            USART_SendData(USART1, i);
        }
    }
}

