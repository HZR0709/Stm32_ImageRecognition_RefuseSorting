#include "../bh1750/bh1750.h"

float tempFlag = 20;

void GPIOConfig(void)
{ 
	  GPIO_InitTypeDef GPIO_InitStruct;
	
	   /*����GPIOB������ʱ��*/ 
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 
	
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = sda | scl ;
    GPIO_Init(GPIOB, &GPIO_InitStruct); 
}

void Delay_5us(void)
{
	 uint16_t i=0;
	 uint16_t timer = 5;
	 while(timer--)
	  {
			 i=10;
			 while(i--);
    }
}

void Delay_mms(uint16_t tmp)
{
	  uint16_t i=0;
	  while(tmp--)
		  {
				i=12000;
				while(i--);
      }
}

/***��ʼ�ź�***/
void BH1750_Start()
{
    GPIO_SetBits(GPIOB, sda);                    //����������
    GPIO_SetBits(GPIOB, scl);                   //����ʱ����
    Delay_5us();                 //��ʱ
    GPIO_ResetBits(GPIOB, sda);                    //�����½���
    Delay_5us();                 //��ʱ
    GPIO_ResetBits(GPIOB, scl);                    //����ʱ����
}

/*****ֹͣ�ź�******/
void BH1750_Stop()
{
    GPIO_ResetBits(GPIOB, sda);                   //����������
    GPIO_SetBits(GPIOB, scl);                      //����ʱ����
    Delay_5us();                 //��ʱ
    GPIO_SetBits(GPIOB, sda);                    //����������
    Delay_5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void BH1750_SendACK(int ack)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = sda;
    GPIO_Init(GPIOB, &GPIO_InitStruct);  
	
	
    if(ack == 1)   //дӦ���ź�
			GPIO_SetBits(GPIOB, sda); 
		else if(ack == 0)
			GPIO_ResetBits(GPIOB, sda); 
		else
			return;
			
    GPIO_SetBits(GPIOB, scl);     //����ʱ����
    Delay_5us();                 //��ʱ
    GPIO_ResetBits(GPIOB, scl);      //����ʱ����
    Delay_5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
**************************************/
int BH1750_RecvACK()
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  /*����һ��Ҫ������������������ܶ�������*/
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = sda;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
	
    GPIO_SetBits(GPIOB, scl);            //����ʱ����
    Delay_5us();                 //��ʱ
	
	  if(GPIO_ReadInputDataBit( GPIOB, sda ) ==1 )//��Ӧ���ź�
        mcy = 1 ;  
    else
        mcy = 0 ;			
	
    GPIO_ResetBits(GPIOB, scl);                    //����ʱ����
    Delay_5us();                 //��ʱ
  
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init( GPIOB, &GPIO_InitStruct );
	
    return mcy;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void BH1750_SendByte(uchar dat)
{
    uchar i;

    for (i=0; i<8; i++)         //8λ������
      {
				if( 0X80 & dat )
          GPIO_SetBits( GPIOB, sda);
        else
          GPIO_ResetBits( GPIOB, sda);
			 
       dat <<= 1;
        GPIO_SetBits(GPIOB, scl);               //����ʱ����
        Delay_5us();             //��ʱ
        GPIO_ResetBits(GPIOB, scl);                //����ʱ����
        Delay_5us();             //��ʱ
      }
    BH1750_RecvACK();
}

uchar BH1750_RecvByte()
{
    uchar i;
    uchar dat = 0;
	  uchar bit;
	  
	 GPIO_InitTypeDef GPIO_InitStruct;
	
	 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;   /*����һ��Ҫ������������������ܶ�������*/
   GPIO_InitStruct.GPIO_Pin = sda;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init( GPIOB, &GPIO_InitStruct );
	
    GPIO_SetBits( GPIOB, sda);          //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        GPIO_SetBits( GPIOB, scl);               //����ʱ����
        Delay_5us();             //��ʱ
			
			  if( SET == GPIO_ReadInputDataBit( GPIOB, sda ) )
             bit = 0X01;
       else
             bit = 0x00;  
			
        dat |= bit;             //������    
			
        GPIO_ResetBits( GPIOB, scl);                //����ʱ����
        Delay_5us();             //��ʱ
    }
		
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOB, &GPIO_InitStruct );
    return dat;
}

void Single_Write_BH1750(uchar REG_Address)
{
    BH1750_Start();                  //��ʼ�ź�
    BH1750_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    BH1750_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
  //  BH1750_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    BH1750_Stop();                   //����ֹͣ�ź�
}


//��ʼ��BH1750��������Ҫ��ο�pdf�����޸�****
void Init_BH1750()
{
   Single_Write_BH1750(0x01);  
}

//��������BH1750�ڲ�����
void mread(void)
{   
	  uchar i;	
    BH1750_Start();                          //��ʼ�ź�
    BH1750_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
	
	 for (i=0; i<3; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = BH1750_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 3)
        {
           BH1750_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {		
          BH1750_SendACK(0);                //��ӦACK
        }
   }

    BH1750_Stop();                          //ֹͣ�ź�
    Delay_mms(5);
}

float GY30_ReadLight(void)
{
	float temp = 0;
	Single_Write_BH1750(0x01);   // power on
	Single_Write_BH1750(0x10);   // H- resolution mode

	delay_ms(50);            //��ʱ180ms
			
	mread();       //�����������ݣ��洢��BUF��

	dis_data=BUF[0];
	dis_data=(dis_data<<8)+BUF[1]; //�ϳ����� 

	temp=(float)dis_data/1.2;
			
			USART_SendData(USART1 ,temp);				
	while(  USART_GetFlagStatus(USART1, USART_FLAG_TC)==SET);
	printf( "\r\n ��ǿ���� %f lx\r\n", temp);
	return temp;
}