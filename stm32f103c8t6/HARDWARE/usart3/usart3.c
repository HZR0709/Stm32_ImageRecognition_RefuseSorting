#include "sys.h"
#include "main.h"
#include "usart3.h" 
#include "delay.h"
 

u8 uart3flag=0; 
 
 

/*
��ʼ������ 

*/
void usart3_init(u32 band)
{
	//GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//  
	
 
	//USART3_TX    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ�� 
   
  //USART3_RX	   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//P  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��   

  //Usart NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3 ԭ3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3     ԭ2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = band;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3
}
void usart3_Send_Data(u8 *buf,u16 len)
{
	u16 t;
	 
  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
	 
}
 /*����һ���ֽ�����*/
 void USART3SendByte(unsigned char SendData)
{	 
   
        USART_SendData(USART3,SendData);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	    
} 

/*
���ݳ����򴮿�3 ��������
*/
void usart3_SendStringByLen(unsigned char * data,u8 len)
{
	u16 i=0;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART3, data[i]);         //�򴮿�1��������
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		//i++;
	}
	//i++;
}
/*����3 �������ݵ���λ�� data ��\0��β*/
void usart3_SendString(unsigned char * data)
{
	u16 i=0;
	while(data[i]!='\0')
	{
		USART_SendData(USART3, data[i]);         //�򴮿�1��������
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		i++;
	}
	i++;
}
/*
����3 ��ѯ����ģʽ

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
����3�ն˺���
*/
void USART3_IRQHandler(void)                	//����3�жϷ������
	{
 
		 static u8 i;
	  
 
 	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{	
    //�ɰ汾��ȩ������
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
					USART3_RX_BUF[12] = '\0'; //�����һ���ַ����¸��ֽ���0��ʹ���Ϊ�ַ�����ʽ
			}
		 
	}	
//		if(uart3flag==0) //�°汾������
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
//					USART3_RX_BUF[12] = '\0'; //�����һ���ַ����¸��ֽ���0��ʹ���Ϊ�ַ�����ʽ
//			}
//		 
//	}	
 
}   
 }


