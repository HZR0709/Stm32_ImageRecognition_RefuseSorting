#include "sys.h"
#include "main.h"
#include "usart3.h" 
#include "delay.h"
 

u8 uart3flag=0; 
 
 

/*
初始化串口 

*/
void usart3_init(u32 band)
{
	//GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//  
	
 
	//USART3_TX    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化 
   
  //USART3_RX	   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//P  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化   

  //Usart NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3 原3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3     原2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = band;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口3
}
void usart3_Send_Data(u8 *buf,u16 len)
{
	u16 t;
	 
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
	 
}
 /*发送一个字节数据*/
 void USART3SendByte(unsigned char SendData)
{	 
   
        USART_SendData(USART3,SendData);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	    
} 

/*
根据长度向串口3 发送数据
*/
void usart3_SendStringByLen(unsigned char * data,u8 len)
{
	u16 i=0;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART3, data[i]);         //向串口1发送数据
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
		//i++;
	}
	//i++;
}
/*串口3 发送数据到上位机 data 以\0结尾*/
void usart3_SendString(unsigned char * data)
{
	u16 i=0;
	while(data[i]!='\0')
	{
		USART_SendData(USART3, data[i]);         //向串口1发送数据
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
		i++;
	}
	i++;
}
/*
串口3 查询接受模式

*/
u16 usart3_receive(u32 time)  
{
	  u16 i=0;
    u32 time0=0;
    i = USART_ReceiveData(USART3);
    while(USART_GetFlagStatus(USART3,USART_FLAG_RXNE) == RESET)
    { 
        delay_us(1);
        time0++;
        if( time0 > time )
        {
           // printf("\r\ntime=%d\r\n",time);
            return 0;
        } 
    }
    USART3_RX_BUF[0] = USART_ReceiveData(USART3); 
    i = 1;
    time0 = 0;
    while(1)
    { 
        if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE) != RESET)
        {	 
            time0 = 0;
            USART3_RX_BUF[i] = USART_ReceiveData(USART3);
            i++;
//            if( (i == len) && (len != 0) )  
//                break; 
        }
        else 
        {
            if( time0 > 50000 ) 
                break;
            else 
                time0++;
        }
    }
    USART3_RX_BUF[i]=0;
    return i;

}

/*
串口3终端函数
*/
void USART3_IRQHandler(void)                	//串口3中断服务程序
	{
 
		 static u8 i;
	  
 
 	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{	
    //旧版本甲醛传感器
			if(uart3flag==0)
		{
		  USART3_RX_BUF[0] = USART_ReceiveData(USART3);	
				if( USART2_RX_BUF[0]==0x42) 
				 {
					 uart3flag=1; i = 1; 
				 }
				 else
				 {
					USART3_RX_BUF[0]=0;
				 }
		}
	else	
	  {
			 
				USART3_RX_BUF[i] = USART_ReceiveData(USART3);
				  i++; 
			if(i == 10) 
			{
				  i=0;
					IRQURA2 = 1;  
					uart3flag=0; 
					USART3_RX_BUF[12] = '\0'; //对最后一个字符的下个字节置0，使其成为字符串格式
			}
		 
	}	
//		if(uart3flag==0) //新版本传感器
//		{
//		  USART3_RX_BUF[0] = USART_ReceiveData(USART3);	
//				if( USART2_RX_BUF[0]==0xaa) 
//				 {
//					 uart3flag=1; i = 1; 
//				 }
//				 else
//				 {
//					USART3_RX_BUF[0]=0;
//				 }
//		}
//	else	
//	  {
//			 
//				USART3_RX_BUF[i] = USART_ReceiveData(USART3);
//				  i++; 
//			if(i == 12) 
//			{
//				  i=0;
//					IRQURA2 = 1;  
//					uart3flag=0; 
//					USART3_RX_BUF[12] = '\0'; //对最后一个字符的下个字节置0，使其成为字符串格式
//			}
//		 
//	}	
 
}   
 }


