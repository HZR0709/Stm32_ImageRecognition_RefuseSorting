#include "led.h"
#include "delay.h"
#include "main.h"
#include "debug.h"

/*定义控制管脚*/
/*
sbit OE=P1^0;	//屏幕总使能
sbit Add_A=P1^1;  //显示地址高位
sbit Add_B=P1^2; 	//显示地址高位
sbit SHCP_595=P1^3;	 //74HC595时钟
sbit STCP_595=P1^4;	 //74HC595锁存
sbit DS_595=P1^5;	 //第一路屏595信号数据
sbit DS_595_2=P3^2;	 //第二路屏595信号数据

目前管脚布置
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
#define DS_595 (GPIO_Pin_4)//pb口 最下面一组
#define DS_5952 (GPIO_Pin_3)//pb3 口 
#define DS_5953 (GPIO_Pin_7)//pd7

#define A PEout(1)// PE9
#define B PEout(3)// PE9
#define OEE PEout(0)// PE9

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
/*
led pe9
*/
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOE,GPIO_Pin_9);						 //PB.5 输出高

 
}
 
/*点亮led*/
/*
初始化led屏管脚
#define OE GPIO_Pin_0
#define Add_A (GPIO_Pin_1)
#define Add_B (GPIO_Pin_3)
#define SHCP_595 (GPIO_Pin_5)
#define STCP_595 (GPIO_Pin_6)
#define DS_595 (GPIO_Pin_4)//pb口
*/
void led_p_init()
{
	u8 i,j;
	//全部初始化为推挽输出
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PB,PE端口时钟
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);		//禁止JTAG功能
	/*GPIO_Pin_9 是一个led灯*/
 GPIO_InitStructure.GPIO_Pin = OE|Add_A|Add_B|SHCP_595|STCP_595|GPIO_Pin_9;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 
	//初始化数据输出管脚
	GPIO_InitStructure.GPIO_Pin = DS_595|DS_5952;				 //LED0-->PB.5 端口配置
 
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_InitStructure.GPIO_Pin=DS_5953;//pd7
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	OEE=0;//去使能
	for(i=0;i<16;i++)//led不显示内容
	{
		for(j=0;j<12;j++)
		{
			led[i][j]=0x00;
		}
	}
	//关闭小led等
	led1=0;
}
/*
扫描第q行 q=0,1,2,3
*/
void led_scan_row(u8 q)//
{
	u8 i,k,dat;
	s8 j;
	for(i=0;i<12;i++)
	{
		//发送第i*4行
		//dat=led[i*4+q];
		//发送一行数据 1行 12个字节
		for(j=0;j<4;j++)
		{
			dat=led[j*4+q][i];
			//发送字节
			for(k=0;k<8;k++)
			{
				if(dat&0x80)
				{
					GPIO_ResetBits(GPIOB,DS_595);	  //发送第一路595数据
					GPIO_ResetBits(GPIOB,DS_5952);
					GPIO_ResetBits(GPIOD,DS_5953);
				}				
			else
			{
				GPIO_SetBits(GPIOB,DS_595);
				GPIO_SetBits(GPIOB,DS_5952);
				GPIO_SetBits(GPIOD,DS_5953);
			}
			
//			if(dat1&0x80) DS_595_2=1;  //发送第二路595数据
//			else DS_595_2=0;
			GPIO_ResetBits(GPIOE,SHCP_595);
			
			GPIO_SetBits(GPIOE,SHCP_595);

			dat<<=1;//先发高位  再发低位
			//dat1=dat1<<1;   //数据移位
			}
		}
	}
	//delay_us(1);
}
/*
测试led点阵的函数
0--亮
1--灭
1小块 16X32  16行  32列
屏幕是3X3
一路屏是 16X96=16*12 字节
stcp--锁存-- 上升沿
shcp--时钟--数据发送 上升沿
stcp shcp 上升沿  数据传入 且 上一次的数据并出 且串出
stcp 上升沿 锁定 并出
shcp 上升沿 串入  且 串出
*/
/*
关闭第i行显示
*/
void led_row_close()
{
	OEE=1;
}
/*
扫描4分之1函数 i=0 1 2 3
OEE=1 使能
*/
void led_scan_rowlock(u8 i)
{
	/*
	扫描第i行 并锁存
	*/
	switch(i)
	{
		case 0:
			GPIO_ResetBits(GPIOE,STCP_595);//拉低 为了形成一个上升沿

		  A=1;
	    B=1;
		  led_scan_row(0);
		  GPIO_SetBits(GPIOE,STCP_595);//形成上升沿  保持数据输出
			OEE=1;//选择第0行
			break;
		case 1:
			GPIO_ResetBits(GPIOE,STCP_595);//拉低 为了形成一个上升沿

		  A=0;
	    B=1;
		  led_scan_row(1);
		  GPIO_SetBits(GPIOE,STCP_595);//形成上升沿  保持数据输出
			OEE=1;//选择第0行
			break;
		case 2:

	GPIO_ResetBits(GPIOB,DS_595);
		  A=1;
	    B=0;
		  led_scan_row(2);
		  GPIO_SetBits(GPIOE,STCP_595);//形成上升沿  保持数据输出
		  OEE=1;//选择第0行
			break;
		case 3:

	GPIO_ResetBits(GPIOB,DS_595);
			A=0;
	    B=0;
		  led_scan_row(3);
		  GPIO_SetBits(GPIOE,STCP_595);//形成上升沿  保持数据输出
		  OEE=1;//选择第0行
			break;
	}
}
/*
仅用于测试

*/
void test_led()
{
	
	GPIO_ResetBits(GPIOE,STCP_595);
	OEE=1;
	GPIO_ResetBits(GPIOB,DS_595);
	//选中第一行
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
变换矩阵 适应led
*/
void led_tran()
{
	u8 temp[16][12]={0};
	//初始化矩阵
	u8 row,col;
	
	for(row=0;row<16;row++)
	{
		for(col=0;col<12;col++)
		{
			//赋值
			temp[15-row][col]=led[row][col];
			led[row][col]=0;
		}
	}
	//再次变换
	/*
	0--》3
	1--》0
	2--》1
	3--》2
	
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
	//清空显示
	for(row=0;row<16;row++)
	{
		for(col=0;col<12;col++)
		{
			led[row][col]=0x00;
		}
	}
}
/*
将1个字体写入矩阵
dat--字体自摸
width--宽度所占字节
rownum-高度所占行数
iserasure--是不是擦除其余部分
left--距离左上角位数
right--距离右上角位数

*/
void test1(u8*dat,u8 width,u8 rownum,u8 iserasure,u8 left,u8 height)//
{
	
	//初始化矩阵
	u8 len,row,col,i=0;
	u8 loffbyte,loffbit;
	 len=width*rownum;
	loffbyte=left/8;
	loffbit=left%8;//余数
	
	//循环替换
	for(row=0;row<rownum;row++)
	{
		//计算从left开始的位数
		for(col=0;col<width;col++)//width 所占字节数
		{
			led[height+row][loffbyte+col]=dat[i];
			i++;
			//循环移位
			
			if(i==len) break;//布置完了
		}
		if(i==len)break;//布置完了
		
	}
	//变换矩阵
	
	
}

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}
//定时器3中断服务程序
/*
用于扫描 led 4ms进一次中断
1次扫描4分之1
*/
void TIM3_IRQHandler(void)   //TIM3中断
{
	//u8 temp[10]={0};
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
			
//		 if(led_lock==0)
//		 {
//			 //扫描4分之1
//			led_scan_rowlock(led_scan_rownum);

//			//记录下次要扫描的行
//			led_scan_rownum++;
//			if(led_scan_rownum>3) led_scan_rownum=0;
//		  led_scan_start_count_flag=1;//开始计时
//			led_scan_counter=0;
//		 }
     
			
			
			test_led();
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
		}
}


