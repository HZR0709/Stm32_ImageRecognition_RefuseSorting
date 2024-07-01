#include "led.h"
#include "delay.h"
#include "main.h"
#include "debug.h"

/*������ƹܽ�*/
/*
sbit OE=P1^0;	//��Ļ��ʹ��
sbit Add_A=P1^1;  //��ʾ��ַ��λ
sbit Add_B=P1^2; 	//��ʾ��ַ��λ
sbit SHCP_595=P1^3;	 //74HC595ʱ��
sbit STCP_595=P1^4;	 //74HC595����
sbit DS_595=P1^5;	 //��һ·��595�ź�����
sbit DS_595_2=P3^2;	 //�ڶ�·��595�ź�����

Ŀǰ�ܽŲ���
OE--pe0
a--pe1
b--pe3
s--pe5
l--pe6
r--pb4

*/
#define OE GPIO_Pin_0
#define Add_A (GPIO_Pin_1)
#define Add_B (GPIO_Pin_3)
#define SHCP_595 (GPIO_Pin_5)
#define STCP_595 (GPIO_Pin_6)
#define DS_595 (GPIO_Pin_4)//pb�� ������һ��
#define DS_5952 (GPIO_Pin_3)//pb3 �� 
#define DS_5953 (GPIO_Pin_7)//pd7

#define A PEout(1)// PE9
#define B PEout(3)// PE9
#define OEE PEout(0)// PE9

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
/*
led pe9
*/
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOE,GPIO_Pin_9);						 //PB.5 �����

 
}
 
/*����led*/
/*
��ʼ��led���ܽ�
#define OE GPIO_Pin_0
#define Add_A (GPIO_Pin_1)
#define Add_B (GPIO_Pin_3)
#define SHCP_595 (GPIO_Pin_5)
#define STCP_595 (GPIO_Pin_6)
#define DS_595 (GPIO_Pin_4)//pb��
*/
void led_p_init()
{
	u8 i,j;
	//ȫ����ʼ��Ϊ�������
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);		//��ֹJTAG����
	/*GPIO_Pin_9 ��һ��led��*/
 GPIO_InitStructure.GPIO_Pin = OE|Add_A|Add_B|SHCP_595|STCP_595|GPIO_Pin_9;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 
	//��ʼ����������ܽ�
	GPIO_InitStructure.GPIO_Pin = DS_595|DS_5952;				 //LED0-->PB.5 �˿�����
 
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_InitStructure.GPIO_Pin=DS_5953;//pd7
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	OEE=0;//ȥʹ��
	for(i=0;i<16;i++)//led����ʾ����
	{
		for(j=0;j<12;j++)
		{
			led[i][j]=0x00;
		}
	}
	//�ر�Сled��
	led1=0;
}
/*
ɨ���q�� q=0,1,2,3
*/
void led_scan_row(u8 q)//
{
	u8 i,k,dat;
	s8 j;
	for(i=0;i<12;i++)
	{
		//���͵�i*4��
		//dat=led[i*4+q];
		//����һ������ 1�� 12���ֽ�
		for(j=0;j<4;j++)
		{
			dat=led[j*4+q][i];
			//�����ֽ�
			for(k=0;k<8;k++)
			{
				if(dat&0x80)
				{
					GPIO_ResetBits(GPIOB,DS_595);	  //���͵�һ·595����
					GPIO_ResetBits(GPIOB,DS_5952);
					GPIO_ResetBits(GPIOD,DS_5953);
				}				
			else
			{
				GPIO_SetBits(GPIOB,DS_595);
				GPIO_SetBits(GPIOB,DS_5952);
				GPIO_SetBits(GPIOD,DS_5953);
			}
			
//			if(dat1&0x80) DS_595_2=1;  //���͵ڶ�·595����
//			else DS_595_2=0;
			GPIO_ResetBits(GPIOE,SHCP_595);
			
			GPIO_SetBits(GPIOE,SHCP_595);

			dat<<=1;//�ȷ���λ  �ٷ���λ
			//dat1=dat1<<1;   //������λ
			}
		}
	}
	//delay_us(1);
}
/*
����led����ĺ���
0--��
1--��
1С�� 16X32  16��  32��
��Ļ��3X3
һ·���� 16X96=16*12 �ֽ�
stcp--����-- ������
shcp--ʱ��--���ݷ��� ������
stcp shcp ������  ���ݴ��� �� ��һ�ε����ݲ��� �Ҵ���
stcp ������ ���� ����
shcp ������ ����  �� ����
*/
/*
�رյ�i����ʾ
*/
void led_row_close()
{
	OEE=1;
}
/*
ɨ��4��֮1���� i=0 1 2 3
OEE=1 ʹ��
*/
void led_scan_rowlock(u8 i)
{
	/*
	ɨ���i�� ������
	*/
	switch(i)
	{
		case 0:
			GPIO_ResetBits(GPIOE,STCP_595);//���� Ϊ���γ�һ��������

		  A=1;
	    B=1;
		  led_scan_row(0);
		  GPIO_SetBits(GPIOE,STCP_595);//�γ�������  �����������
			OEE=1;//ѡ���0��
			break;
		case 1:
			GPIO_ResetBits(GPIOE,STCP_595);//���� Ϊ���γ�һ��������

		  A=0;
	    B=1;
		  led_scan_row(1);
		  GPIO_SetBits(GPIOE,STCP_595);//�γ�������  �����������
			OEE=1;//ѡ���0��
			break;
		case 2:

	GPIO_ResetBits(GPIOB,DS_595);
		  A=1;
	    B=0;
		  led_scan_row(2);
		  GPIO_SetBits(GPIOE,STCP_595);//�γ�������  �����������
		  OEE=1;//ѡ���0��
			break;
		case 3:

	GPIO_ResetBits(GPIOB,DS_595);
			A=0;
	    B=0;
		  led_scan_row(3);
		  GPIO_SetBits(GPIOE,STCP_595);//�γ�������  �����������
		  OEE=1;//ѡ���0��
			break;
	}
}
/*
�����ڲ���

*/
void test_led()
{
	
	GPIO_ResetBits(GPIOE,STCP_595);
	OEE=1;
	GPIO_ResetBits(GPIOB,DS_595);
	//ѡ�е�һ��
	A=1;
	B=1;
	led_scan_row(0);
	
	GPIO_SetBits(GPIOE,STCP_595);
	OEE=0;
	//delay_us(1);
	
	GPIO_ResetBits(GPIOE,STCP_595);
	OEE=1;
	GPIO_ResetBits(GPIOB,DS_595);
	
	
	A=0;B=1;
	led_scan_row(1);
	
	GPIO_SetBits(GPIOE,STCP_595);
	OEE=0;
	//delay_us(1);
	
	GPIO_ResetBits(GPIOE,STCP_595);
	OEE=1;
	GPIO_ResetBits(GPIOB,DS_595);
	
	
	A=1;B=0;
	led_scan_row(2);
	
	GPIO_SetBits(GPIOE,STCP_595);
	OEE=0;
	//delay_us(1);
	
	GPIO_ResetBits(GPIOE,STCP_595);
	OEE=1;
	GPIO_ResetBits(GPIOB,DS_595);
	
	
	A=0;B=0;
	led_scan_row(3);

	GPIO_SetBits(GPIOE,STCP_595);
	OEE=0;
	//delay_us(1);
}


/*
�任���� ��Ӧled
*/
void led_tran()
{
	u8 temp[16][12]={0};
	//��ʼ������
	u8 row,col;
	
	for(row=0;row<16;row++)
	{
		for(col=0;col<12;col++)
		{
			//��ֵ
			temp[15-row][col]=led[row][col];
			led[row][col]=0;
		}
	}
	//�ٴα任
	/*
	0--��3
	1--��0
	2--��1
	3--��2
	
	*/
	
		for(col=0;col<12;col++)
		{
			led[3][col]=temp[0][col];
		}
		for(col=0;col<12;col++)
		{
			led[0][col]=temp[1][col];
		}
		for(col=0;col<12;col++)
		{
			led[1][col]=temp[2][col];
		}
		for(col=0;col<12;col++)
		{
			led[2][col]=temp[3][col];
		}
		
		for(col=0;col<12;col++)
		{
			led[7][col]=temp[4][col];
		}
		for(col=0;col<12;col++)
		{
			led[4][col]=temp[5][col];
		}
		for(col=0;col<12;col++)
		{
			led[5][col]=temp[6][col];
		}
		for(col=0;col<12;col++)
		{
			led[6][col]=temp[7][col];
		}
		
		for(col=0;col<12;col++)
		{
			led[11][col]=temp[8][col];
		}
		for(col=0;col<12;col++)
		{
			led[8][col]=temp[9][col];
		}
		for(col=0;col<12;col++)
		{
			led[9][col]=temp[10][col];
		}
		for(col=0;col<12;col++)
		{
			led[10][col]=temp[11][col];
		}
		
		for(col=0;col<12;col++)
		{
			led[15][col]=temp[12][col];
		}
		for(col=0;col<12;col++)
		{
			led[12][col]=temp[13][col];
		}
		for(col=0;col<12;col++)
		{
			led[13][col]=temp[14][col];
		}
		for(col=0;col<12;col++)
		{
			led[14][col]=temp[15][col];
		}
}

void led_clear()
{
	u8 row,col;
	//�����ʾ
	for(row=0;row<16;row++)
	{
		for(col=0;col<12;col++)
		{
			led[row][col]=0x00;
		}
	}
}
/*
��1������д�����
dat--��������
width--������ռ�ֽ�
rownum-�߶���ռ����
iserasure--�ǲ��ǲ������ಿ��
left--�������Ͻ�λ��
right--�������Ͻ�λ��

*/
void test1(u8*dat,u8 width,u8 rownum,u8 iserasure,u8 left,u8 height)//
{
	
	//��ʼ������
	u8 len,row,col,i=0;
	u8 loffbyte,loffbit;
	 len=width*rownum;
	loffbyte=left/8;
	loffbit=left%8;//����
	
	//ѭ���滻
	for(row=0;row<rownum;row++)
	{
		//�����left��ʼ��λ��
		for(col=0;col<width;col++)//width ��ռ�ֽ���
		{
			led[height+row][loffbyte+col]=dat[i];
			i++;
			//ѭ����λ
			
			if(i==len) break;//��������
		}
		if(i==len)break;//��������
		
	}
	//�任����
	
	
}

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,���������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx					 
}
//��ʱ��3�жϷ������
/*
����ɨ�� led 4ms��һ���ж�
1��ɨ��4��֮1
*/
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	//u8 temp[10]={0};
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
			
//		 if(led_lock==0)
//		 {
//			 //ɨ��4��֮1
//			led_scan_rowlock(led_scan_rownum);

//			//��¼�´�Ҫɨ�����
//			led_scan_rownum++;
//			if(led_scan_rownum>3) led_scan_rownum=0;
//		  led_scan_start_count_flag=1;//��ʼ��ʱ
//			led_scan_counter=0;
//		 }
     
			
			
			test_led();
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		}
}

