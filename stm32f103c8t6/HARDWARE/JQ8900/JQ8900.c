

#include "JQ8900.h"
#include "delay.h"



 


void JQ8900_Init(void)
{	
 
	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTA时钟使能 
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*使能SWD 禁用JTAG*/ 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  //  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; //输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA
 
 	
}







/***********************发送命令***控制语音输出*********************/
void SendJQ8900Data (u8 addr)// 
{
    u8 i;
    VSDA = 1;/*拉高 */
    delay_us ( 1000 );
    VSDA = 0;/*拉低*/
    delay_us ( 3200 );/*>2ms*/
    for ( i = 0; i < 8; i++ ) /*发送一字节*/
    {
        VSDA = 1;
        if ( addr & 0x01 ) /* */
        {
            delay_us ( 1200 );
            VSDA = 0;
            delay_us ( 400 );
        }
        else              /* */
        {
            delay_us ( 400 );
            VSDA = 0;
            delay_us ( 1200 );
        }
        addr >>= 1;
    }
    VSDA = 1; 
}

// 播放第 num 段语音
void JQ8900Speak(u8 num)
{
	  //while(VBUSY);
	  SendJQ8900Data(0x0a);   delay_ms(300);
		SendJQ8900Data(num/10); delay_ms(300);
		SendJQ8900Data(num%10); delay_ms(300);
		SendJQ8900Data(0x0b);   delay_ms(300);
		//while(!VBUSY);
}




















