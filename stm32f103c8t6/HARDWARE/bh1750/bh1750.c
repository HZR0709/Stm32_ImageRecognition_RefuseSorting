#include "../bh1750/bh1750.h"

float tempFlag = 20;

void GPIOConfig(void)
{ 
	  GPIO_InitTypeDef GPIO_InitStruct;
	
	   /*开启GPIOB的外设时钟*/ 
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

/***开始信号***/
void BH1750_Start()
{
    GPIO_SetBits(GPIOB, sda);                    //拉高数据线
    GPIO_SetBits(GPIOB, scl);                   //拉高时钟线
    Delay_5us();                 //延时
    GPIO_ResetBits(GPIOB, sda);                    //产生下降沿
    Delay_5us();                 //延时
    GPIO_ResetBits(GPIOB, scl);                    //拉低时钟线
}

/*****停止信号******/
void BH1750_Stop()
{
    GPIO_ResetBits(GPIOB, sda);                   //拉低数据线
    GPIO_SetBits(GPIOB, scl);                      //拉高时钟线
    Delay_5us();                 //延时
    GPIO_SetBits(GPIOB, sda);                    //产生上升沿
    Delay_5us();                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void BH1750_SendACK(int ack)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = sda;
    GPIO_Init(GPIOB, &GPIO_InitStruct);  
	
	
    if(ack == 1)   //写应答信号
			GPIO_SetBits(GPIOB, sda); 
		else if(ack == 0)
			GPIO_ResetBits(GPIOB, sda); 
		else
			return;
			
    GPIO_SetBits(GPIOB, scl);     //拉高时钟线
    Delay_5us();                 //延时
    GPIO_ResetBits(GPIOB, scl);      //拉低时钟线
    Delay_5us();                 //延时
}

/**************************************
接收应答信号
**************************************/
int BH1750_RecvACK()
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  /*这里一定要设成输入上拉，否则不能读出数据*/
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = sda;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
	
    GPIO_SetBits(GPIOB, scl);            //拉高时钟线
    Delay_5us();                 //延时
	
	  if(GPIO_ReadInputDataBit( GPIOB, sda ) ==1 )//读应答信号
        mcy = 1 ;  
    else
        mcy = 0 ;			
	
    GPIO_ResetBits(GPIOB, scl);                    //拉低时钟线
    Delay_5us();                 //延时
  
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_Init( GPIOB, &GPIO_InitStruct );
	
    return mcy;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void BH1750_SendByte(uchar dat)
{
    uchar i;

    for (i=0; i<8; i++)         //8位计数器
      {
				if( 0X80 & dat )
          GPIO_SetBits( GPIOB, sda);
        else
          GPIO_ResetBits( GPIOB, sda);
			 
       dat <<= 1;
        GPIO_SetBits(GPIOB, scl);               //拉高时钟线
        Delay_5us();             //延时
        GPIO_ResetBits(GPIOB, scl);                //拉低时钟线
        Delay_5us();             //延时
      }
    BH1750_RecvACK();
}

uchar BH1750_RecvByte()
{
    uchar i;
    uchar dat = 0;
	  uchar bit;
	  
	 GPIO_InitTypeDef GPIO_InitStruct;
	
	 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;   /*这里一定要设成输入上拉，否则不能读出数据*/
   GPIO_InitStruct.GPIO_Pin = sda;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init( GPIOB, &GPIO_InitStruct );
	
    GPIO_SetBits( GPIOB, sda);          //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        GPIO_SetBits( GPIOB, scl);               //拉高时钟线
        Delay_5us();             //延时
			
			  if( SET == GPIO_ReadInputDataBit( GPIOB, sda ) )
             bit = 0X01;
       else
             bit = 0x00;  
			
        dat |= bit;             //读数据    
			
        GPIO_ResetBits( GPIOB, scl);                //拉低时钟线
        Delay_5us();             //延时
    }
		
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOB, &GPIO_InitStruct );
    return dat;
}

void Single_Write_BH1750(uchar REG_Address)
{
    BH1750_Start();                  //起始信号
    BH1750_SendByte(SlaveAddress);   //发送设备地址+写信号
    BH1750_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
  //  BH1750_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    BH1750_Stop();                   //发送停止信号
}


//初始化BH1750，根据需要请参考pdf进行修改****
void Init_BH1750()
{
   Single_Write_BH1750(0x01);  
}

//连续读出BH1750内部数据
void mread(void)
{   
	  uchar i;	
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
	
	 for (i=0; i<3; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = BH1750_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 3)
        {
           BH1750_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {		
          BH1750_SendACK(0);                //回应ACK
        }
   }

    BH1750_Stop();                          //停止信号
    Delay_mms(5);
}

float GY30_ReadLight(void)
{
	float temp = 0;
	Single_Write_BH1750(0x01);   // power on
	Single_Write_BH1750(0x10);   // H- resolution mode

	delay_ms(50);            //延时180ms
			
	mread();       //连续读出数据，存储在BUF中

	dis_data=BUF[0];
	dis_data=(dis_data<<8)+BUF[1]; //合成数据 

	temp=(float)dis_data/1.2;
			
			USART_SendData(USART1 ,temp);				
	while(  USART_GetFlagStatus(USART1, USART_FLAG_TC)==SET);
	printf( "\r\n 光强度是 %f lx\r\n", temp);
	return temp;
}