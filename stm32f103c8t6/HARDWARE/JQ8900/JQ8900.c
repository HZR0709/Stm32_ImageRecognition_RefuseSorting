

#include "JQ8900.h"
#include "delay.h"



 


void JQ8900_Init(void)
{	
 
	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTAʱ��ʹ�� 
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);      /*ʹ��SWD ����JTAG*/ 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;  //  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA
 
 	
}







/***********************��������***�����������*********************/
void SendJQ8900Data (u8 addr)// 
{
    u8 i;
    VSDA = 1;/*���� */
    delay_us ( 1000 );
    VSDA = 0;/*����*/
    delay_us ( 3200 );/*>2ms*/
    for ( i = 0; i < 8; i++ ) /*����һ�ֽ�*/
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

// ���ŵ� num ������
void JQ8900Speak(u8 num)
{
	  //while(VBUSY);
	  SendJQ8900Data(0x0a);   delay_ms(300);
		SendJQ8900Data(num/10); delay_ms(300);
		SendJQ8900Data(num%10); delay_ms(300);
		SendJQ8900Data(0x0b);   delay_ms(300);
		//while(!VBUSY);
}




















