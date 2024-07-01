#include "sys.h"
#include "main.h"
#include "usart2.h" 
#include "usart3.h" 
#include "delay.h"
#include "led.h"
#include "timer.h"
 
u8 uart2flag = 0;

/*
初始化串口2
*/
void usart2_init(u32 band)
{
    // GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // 使能USART2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //  

    //重新定义管脚
    //GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
    //USART2_TX    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);         // 初始化 

    //USART2_RX	   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // 初始化   

    //Usart NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQ_INDEX;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_PREEMPTION_PRIORITY; // 抢占优先级3 原3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_SUB_PRIORITY;             // 子优先级3     原2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                  // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                                                  // 根据指定的参数初始化VIC寄存器

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = band;                        // 串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;         // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;             // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    // 收发模式

    USART_Init(USART2, &USART_InitStructure); // 初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启串口接受中断
    USART_Cmd(USART2, ENABLE);                    // 使能串口2
}

void RS485_Send_Data(u8 *buf, u16 len)
{
    u16 t;

    for (t = 0; t < len; t++) // 循环发送数据
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        USART_SendData(USART2, buf[t]);
    }

    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

/*发送一个字节数据*/
void USART2SendByte(unsigned char SendData)
{
    USART_SendData(USART2, SendData);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/*
根据长度向串口2 发送数据
*/
void usart2_SendStringByLen(unsigned char *data, u8 len)
{
    u16 i = 0;
    for (i = 0; i < len; i++)
    {
        USART_SendData(USART2, data[i]); //向串口2发送数据
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET); //等待发送结束
    }
}

/*串口2 发送数据到上位机 data 以\0结尾*/
void usart2_SendString(unsigned char *data)
{
    u16 i = 0;
    while (data[i] != '\0')
    {
        USART_SendData(USART2, data[i]); //向串口2发送数据
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET); //等待发送结束
        i++;
    }
}

/*
串口2 查询接受模式
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

//['无', '可回收', '有害', '厨余', '其他']   0-1-2-3-4 
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
                SG90_PWM1 = OPEN;  //打开垃圾桶盖可回收垃圾 
                printf("可回收垃圾\r\n");
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
                SG90_PWM2 = OPEN;  //打开垃圾桶盖有害垃圾
                printf("有害垃圾\r\n");
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
                SG90_PWM3 = OPEN;   //打开垃圾桶盖厨余垃圾 
                printf("厨余垃圾\r\n");
            }
            memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
        }
        else if (strstr((char *)USART2_RX_BUF, "result:4") != NULL) //
        {
            cnt4++;
			printf("其它垃圾%d\n",cnt4);
            if (cnt4 > 3)
            {
                cnt4 = 0;
                SG90Flag = 1;
                classify = 4;
                SG90_PWM4 = OPEN;   //打开垃圾桶盖其他垃圾 
                printf("其他垃圾\r\n");
            }
            memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
        }
        else if (strstr((char *)USART2_RX_BUF, "result:0") != NULL) //
        {
            cnt1 = 0;
            cnt2 = 0;
            cnt3 = 0;
            cnt4 = 0;
            printf("无垃圾\r\n"); //无垃圾 
            classify = 0;
            memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
        }
    }
}

/*
串口2中断服务程序
*/
void USART2_IRQHandler(void)
{
    static u8 i;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART2_RX_BUF[i] = USART_ReceiveData(USART2);
        i++;
        if (i > 10) //接收 
        {
            i = 0;
        }
    }
}


