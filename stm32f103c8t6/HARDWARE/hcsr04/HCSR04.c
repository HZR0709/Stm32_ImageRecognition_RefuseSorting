#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "stdbool.h"

volatile uint64_t time=0;			//声明变量，用来计时
volatile uint64_t time_end=0;		//声明变量，存储回波信号时间

#define NUM_SENSORS 4  // 假设有4个超声波传感器
bool sensorConnected[NUM_SENSORS] = {false, false, false, false};  // 连接状态数组

//uint64_t numlen(uint64_t num)//计算数字的长度
//{
//    uint64_t len = 1;        // 初始长度为1
//    for(; num > 9; ++len)    // 判断num是否大于9，否则长度+1
//        num /= 10;	         // 使用除法进行运算，直到num小于1
//    return len;              // 返回长度的值
//}

void HC_SR04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Pin = Trig1;				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(Trig1_GPIO_PORT,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	
	GPIO_InitStructure.GPIO_Pin = Echo1;					
	GPIO_Init(Trig1_GPIO_PORT,&GPIO_InitStructure);		

/*************************************************************************************/
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Pin = Trig2;				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(Trig2_GPIO_PORT,&GPIO_InitStructure);		
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		
	GPIO_InitStructure.GPIO_Pin = Echo2;			
	GPIO_Init(Trig2_GPIO_PORT,&GPIO_InitStructure);		

/*************************************************************************************/

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		
	GPIO_InitStructure.GPIO_Pin = Trig3;						
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(Trig3_GPIO_PORT,&GPIO_InitStructure);			
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		
	GPIO_InitStructure.GPIO_Pin = Echo3;						
	GPIO_Init(Trig3_GPIO_PORT,&GPIO_InitStructure);					

/*************************************************************************************/

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		
	GPIO_InitStructure.GPIO_Pin = Trig4;					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(Trig4_GPIO_PORT,&GPIO_InitStructure);			
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;			
	GPIO_InitStructure.GPIO_Pin = Echo4;					
	GPIO_Init(Trig4_GPIO_PORT,&GPIO_InitStructure);			

/*************************************************************************************/

    // 动态检测每个模块的连接状态
    checkSensorConnection(Trig1_GPIO_PORT, Trig1, Trig1_GPIO_PORT, Echo1, 0);
	if(sensorConnected[0]) printf("Hc-sr04已连接01\n");
	checkSensorConnection(Trig2_GPIO_PORT, Trig2, Trig2_GPIO_PORT, Echo2, 1);
	if(sensorConnected[1]) printf("Hc-sr04已连接02\n");
	checkSensorConnection(Trig3_GPIO_PORT, Trig3, Trig3_GPIO_PORT, Echo3, 2);
	if(sensorConnected[2]) printf("Hc-sr04已连接03\n");
	checkSensorConnection(Trig4_GPIO_PORT, Trig4, Trig4_GPIO_PORT, Echo4, 3);
	if(sensorConnected[3]) printf("Hc-sr04已连接04\n");

	GPIO_WriteBit(Trig1_GPIO_PORT,Trig1,0);						//输出低电平
	GPIO_WriteBit(Trig1_GPIO_PORT,Trig2,0);
	GPIO_WriteBit(Trig1_GPIO_PORT,Trig3,0);
	GPIO_WriteBit(Trig1_GPIO_PORT,Trig4,0);
	delay_us(15);											//延时15微秒
}

int16_t sonar_mm(void)									//测距并返回单位为毫米的距离结果
{
	uint32_t Distance,Distance_mm = 0;
	GPIO_WriteBit(GPIOB,Trig1,1);						//输出高电平
	delay_us(15);										//延时15微秒
	GPIO_WriteBit(GPIOB,Trig1,0);						//输出低电平
	while(GPIO_ReadInputDataBit(GPIOB,Echo1)==0);		//等待低电平结束
	time=0;												//计时清零
	while(GPIO_ReadInputDataBit(GPIOB,Echo1)==1);		//等待高电平结束
	time_end=time;										//记录结束时的时间
	if(time_end/100<38)									//判断是否小于38毫秒，大于38毫秒的就是超时，直接调到下面返回0
	{
		Distance=(time_end*346)/2;						//计算距离，25°C空气中的音速为346m/s
		Distance_mm=Distance/100;						//因为上面的time_end的单位是10微秒，所以要得出单位为毫米的距离结果，还得除以100
	}
	return Distance_mm;									//返回测距结果
}

float sonar(void)										//测距并返回单位为米的距离结果
{
	uint32_t Distance,Distance_mm = 0;
	float Distance_m=0;
	GPIO_WriteBit(GPIOB,Trig1,1);					//输出高电平
	delay_us(15);
	GPIO_WriteBit(GPIOB,Trig1,0);					//输出低电平
	while(GPIO_ReadInputDataBit(GPIOB,Echo1)==0);
	time=0;
	while(GPIO_ReadInputDataBit(GPIOB,Echo1)==1);
	time_end=time;
	if(time_end/100<38)
	{
		Distance=(time_end*346)/2;
		Distance_mm=Distance/100;
		Distance_m=Distance_mm/1000;
	}
	return Distance_m;
}

// 函数用于动态检测模块连接状态
void checkSensorConnection(GPIO_TypeDef* trigPort, uint16_t trigPin, GPIO_TypeDef* echoPort, uint16_t echoPin, int sensorIndex) 
{
	uint64_t startTime=0;

    GPIO_WriteBit(trigPort, trigPin, Bit_SET);  // 发送高电平
    delay_us(10);  // 发送10微秒的脉冲
    GPIO_WriteBit(trigPort, trigPin, Bit_RESET);  // 低电平，结束脉冲

    while (GPIO_ReadInputDataBit(echoPort, echoPin) == Bit_RESET) {
		delay_us(1);
		startTime++;
        if (startTime > 1000) {  // 1毫秒超时	
            return;  // 如果超时，则传感器未连接或故障
        }
    }

    // 如果没有超时，我们接收到了回波
    sensorConnected[sensorIndex] = true;  // 标记该传感器为已连接
}

uint8_t isObjectInRange(void)
{
	uint32_t Distance = 0,Distance_mm = 0;
	uint32_t timeout = 1000; // 超时计数器

//可回收垃圾	
	if (sensorConnected[0]) 
	{
		GPIO_WriteBit(Trig1_GPIO_PORT,Trig1,1);						//输出高电平
		delay_us(15);
		GPIO_WriteBit(Trig1_GPIO_PORT,Trig1,0);	//输出低电平
		time=0;
		while(GPIO_ReadInputDataBit(Trig1_GPIO_PORT,Echo1)==0)		//等待低电平结束
		if(time >= timeout) break;
		time=0;										//计时清零
		while(GPIO_ReadInputDataBit(Trig1_GPIO_PORT,Echo1)==1)		//等待高电平结束
		if(time >= timeout) break;
		time_end=time;
		if(time_end/100<38)									//判断是否小于38毫秒，大于38毫秒的就是超时，直接调到下面返回0
		{
			Distance=(time_end*346)/2;						//计算距离，25°C空气中的音速为346m/s
			Distance_mm=Distance/100;						//因为上面的time_end的单位是10微秒，所以要得出单位为毫米的距离结果，还得除以100
			if (Distance_mm <= 500)                            // 判断距离是否小于等于50厘米
				return 1;
		}
	}
	
//有害垃圾
	Distance=0, Distance_mm = 0;
	if (sensorConnected[1]) 
	{
		GPIO_WriteBit(Trig2_GPIO_PORT,Trig2,1);
		delay_us(15);
		GPIO_WriteBit(Trig2_GPIO_PORT,Trig2,0);
		time = 0;
		while(GPIO_ReadInputDataBit(Trig2_GPIO_PORT,Echo2)==0)		//等待低电平结束
		if(time >= timeout) break;
		time=0;												//计时清零
		while(GPIO_ReadInputDataBit(Trig2_GPIO_PORT,Echo2)==1)		//等待高电平结束
		if(time >= timeout) break;
		time_end=time;										//记录结束时的时间
		if(time_end/100<38)									//判断是否小于38毫秒，大于38毫秒的就是超时，直接调到下面返回0
		{
			Distance=(time_end*346)/2;						//计算距离，25°C空气中的音速为346m/s
			Distance_mm=Distance/100;						//因为上面的time_end的单位是10微秒，所以要得出单位为毫米的距离结果，还得除以100
			if (Distance_mm <= 500)                            // 判断距离是否小于等于50厘米
				return 2;
		}
	}
	
//厨余垃圾
	Distance=0, Distance_mm = 0;
	if (sensorConnected[2]) 
	{
		GPIO_WriteBit(Trig3_GPIO_PORT,Trig3,1);
		delay_us(15);
		GPIO_WriteBit(Trig3_GPIO_PORT,Trig3,0);
		time = 0;
		while(GPIO_ReadInputDataBit(Trig3_GPIO_PORT,Echo3)==0)		//等待低电平结束
		if(time >= timeout) break;
		time=0;												//计时清零
		while(GPIO_ReadInputDataBit(Trig3_GPIO_PORT,Echo3)==1)		//等待高电平结束
		if(time >= timeout) break;
		time_end=time;										//记录结束时的时间
		if(time_end/100<38)									//判断是否小于38毫秒，大于38毫秒的就是超时，直接调到下面返回0
		{
			Distance=(time_end*346)/2;						//计算距离，25°C空气中的音速为346m/s
			Distance_mm=Distance/100;						//因为上面的time_end的单位是10微秒，所以要得出单位为毫米的距离结果，还得除以100
			if (Distance_mm <= 500)                            // 判断距离是否小于等于50厘米
				return 3;
		}
	}
	
//其它垃圾
	Distance=0, Distance_mm = 0;
	if (sensorConnected[3]) {
		GPIO_WriteBit(Trig4_GPIO_PORT,Trig4,1);
		delay_us(15);
		GPIO_WriteBit(Trig4_GPIO_PORT,Trig4,0);
		time = 0;
		while(GPIO_ReadInputDataBit(Trig4_GPIO_PORT,Echo4)==0)		//等待低电平结束
		if(time >= timeout) break;
		time=0;												//计时清零
		while(GPIO_ReadInputDataBit(Trig4_GPIO_PORT,Echo4)==1)		//等待高电平结束
		if(time >= timeout) break;
		time_end=time;										//记录结束时的时间
		if(time_end/100<38)									//判断是否小于38毫秒，大于38毫秒的就是超时，直接调到下面返回0
		{
			Distance=(time_end*346)/2;						//计算距离，25°C空气中的音速为346m/s
			Distance_mm=Distance/100;						//因为上面的time_end的单位是10微秒，所以要得出单位为毫米的距离结果，还得除以100
			if (Distance_mm <= 500)                            // 判断距离是否小于等于50厘米
				return 4;
		}
	}
	return 0;                                        // 返回0表示未检测到物体
}


