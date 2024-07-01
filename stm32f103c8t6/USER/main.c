/***************STM32F103C8T6***********************/
#include "stm32f10x.h"           
#include "sys.h"  
#include "main.h"
#include "../config.h"
//----------------------------------------------
uchar BUF[8];        // 接收数据缓存区      	
int dis_data;        // 变量
int mcy;

extern int Maxtime;
extern float tempFlag;
extern volatile uint8_t motionDetected;  // 检测到有人活动的标志
volatile uint8_t sensor_last_state = 0; // 上一次传感器状态
//----------------------------------------------

u8 IRQURA3 = 0;                  // 串口3 接受数据完毕标志
u8 IRQURA2 = 0;                  // 串口2 接受数据完毕标志
u8 USART2_RX_BUF[USART2_REC_LEN] = {0};  // 串口2数据缓冲区
u8 USART3_RX_BUF[USART3_REC_LEN] = {0};  // 串口3数据缓冲区 

volatile u8 timecount1 = 0;  // 舵机状态
volatile u8 timecount2 = 0;  // 舵机恢复计时
volatile u8 timecount3 = 0;  // 开机计时
volatile u8 timecount4 = 0;
// 开始计时标志
u8 timeFlag1 = 1;

volatile uint8_t classify = 0;    // 0:无，1:可回收，2:有害，3:厨余，4:其它
volatile uint8_t classFlag = 1;   // 记录上一次垃圾分类的状态
volatile uint8_t model = 0;       // 模式选择
float temp = 0;					  // 光照强度值

int main(void)  
{ 
    uint8_t i = 1;				//OLED选项
	uint8_t objectDetected = 0;	//声波开关标记
	
    SystemInit(); // 配置系统时钟为 72M 
    delay_init(); // 延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	// 设置NVIC中断分组2:2位抢占优先级，2位响应优先级 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // 使能电源管理单元的时钟,必须要使能时钟才能进入待机模式 
	
//	usart3_init(115200); // 备用
    uart1_init(115200); // 备用调试输出
    usart2_init(9600);  // 采集 K210模块  串口结果 
    GPIO_Config();
    OLED_Init();			//OLED初始化
    Key_GPIO_Config();		//按键初始化
    EXTI0_Config();			//外部中断初始化	
    TIM2_Int_Init();		//TIM2定时器初始化
    TIM3_Int_Init(49, 719); //100Khz的计数频率，计数到5000为50ms   49即 500us
    Init_BH1750();          //BH1750（GY30光照）初始化
	HC_SR04_Init();			//初始化超声波测距模块
	
	//RestoreModel();
	if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET) 	//检测复位来源
	{
		RestoreModel(); 				// 唤醒后从备份寄存器恢复model、Maxtime、tempFlag的值
		
		printf("\r\n 待机唤醒复位 \r\n");
	}
	else
	{
		printf("\r\n 非待机唤醒复位 \r\n");
	}
    delay_ms(180);
	clue();
	timecount4 = Maxtime;
    while (1)  
    {
        while (model == 0) // 0模式选择:1智能模式，2手动模式
        {
			clue();
            i = Interface1(i);
        }
        if (model == 1)
        {
			if(timecount4 == Maxtime) {
				timecount4 = 0;
				temp = GY30_ReadLight();//光照检测
				if(temp < tempFlag) {
					GPIO_SetBits(LED_GPIO_PORT, LED_PIN);
				}
				else {
					GPIO_ResetBits(LED_GPIO_PORT, LED_PIN);
				}
			}
			getResultTemp();					//处理串口2接收到的垃圾分类信息
			objectDetected = isObjectInRange(); //声波检测开关
			switch(objectDetected) {
				case 1:
					SG90Flag = 1;
					classify = 1;
					SG90_PWM1 = OPEN;//可回收垃圾
					printf("可回收垃圾\n");
					break;
				case 2:
					SG90Flag = 1;
					classify = 2;
					SG90_PWM2 = OPEN;//有害垃圾
					printf("有害垃圾\n");
					break;
				case 3:
					SG90Flag = 1;
					classify = 3;
					SG90_PWM3 = OPEN;//厨余垃圾
					printf("厨余垃圾\n");
					break;
				case 4:
					SG90Flag = 1;
					classify = 4;
					SG90_PWM4 = OPEN;//其它垃圾
					printf("其它垃圾\n");
					break;
				default:
					break;
			}
        }
		if(model == 1 || model == 2)
		{
			i = Interface2(i);
		}      
        if (Key_Scan(EscKey_GPIO_PORT, EscKey_PIN) == KEY_ON)
        {
			SG90_PWM1 = CLOSE; // 关闭垃圾桶盖
			SG90_PWM2 = CLOSE; // 关闭垃圾桶盖
			SG90_PWM3 = CLOSE; // 关闭垃圾桶盖
			SG90_PWM4 = CLOSE; // 关闭垃圾桶盖
			GPIO_ResetBits(LED_GPIO_PORT, LED_PIN);
            OLED_Clear();
            TIM_Cmd(TIM3, DISABLE);
            model = 0;
			SG90Flag = 0;
			classify = 0;
            i = 1;
        }			
    }   
}



