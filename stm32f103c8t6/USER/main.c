/***************STM32F103C8T6***********************/
#include "stm32f10x.h"           
#include "sys.h"  
#include "main.h"
#include "../config.h"
//----------------------------------------------
uchar BUF[8];        // �������ݻ�����      	
int dis_data;        // ����
int mcy;

extern int Maxtime;
extern float tempFlag;
extern volatile uint8_t motionDetected;  // ��⵽���˻�ı�־
volatile uint8_t sensor_last_state = 0; // ��һ�δ�����״̬
//----------------------------------------------

u8 IRQURA3 = 0;                  // ����3 ����������ϱ�־
u8 IRQURA2 = 0;                  // ����2 ����������ϱ�־
u8 USART2_RX_BUF[USART2_REC_LEN] = {0};  // ����2���ݻ�����
u8 USART3_RX_BUF[USART3_REC_LEN] = {0};  // ����3���ݻ����� 

volatile u8 timecount1 = 0;  // ���״̬
volatile u8 timecount2 = 0;  // ����ָ���ʱ
volatile u8 timecount3 = 0;  // ������ʱ
volatile u8 timecount4 = 0;
// ��ʼ��ʱ��־
u8 timeFlag1 = 1;

volatile uint8_t classify = 0;    // 0:�ޣ�1:�ɻ��գ�2:�к���3:���࣬4:����
volatile uint8_t classFlag = 1;   // ��¼��һ�����������״̬
volatile uint8_t model = 0;       // ģʽѡ��
float temp = 0;					  // ����ǿ��ֵ

int main(void)  
{ 
    uint8_t i = 1;				//OLEDѡ��
	uint8_t objectDetected = 0;	//�������ر��
	
    SystemInit(); // ����ϵͳʱ��Ϊ 72M 
    delay_init(); // ��ʱ������ʼ��	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	// ����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ� 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // ʹ�ܵ�Դ����Ԫ��ʱ��,����Ҫʹ��ʱ�Ӳ��ܽ������ģʽ 
	
//	usart3_init(115200); // ����
    uart1_init(115200); // ���õ������
    usart2_init(9600);  // �ɼ� K210ģ��  ���ڽ�� 
    GPIO_Config();
    OLED_Init();			//OLED��ʼ��
    Key_GPIO_Config();		//������ʼ��
    EXTI0_Config();			//�ⲿ�жϳ�ʼ��	
    TIM2_Int_Init();		//TIM2��ʱ����ʼ��
    TIM3_Int_Init(49, 719); //100Khz�ļ���Ƶ�ʣ�������5000Ϊ50ms   49�� 500us
    Init_BH1750();          //BH1750��GY30���գ���ʼ��
	HC_SR04_Init();			//��ʼ�����������ģ��
	
	//RestoreModel();
	if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET) 	//��⸴λ��Դ
	{
		RestoreModel(); 				// ���Ѻ�ӱ��ݼĴ����ָ�model��Maxtime��tempFlag��ֵ
		
		printf("\r\n �������Ѹ�λ \r\n");
	}
	else
	{
		printf("\r\n �Ǵ������Ѹ�λ \r\n");
	}
    delay_ms(180);
	clue();
	timecount4 = Maxtime;
    while (1)  
    {
        while (model == 0) // 0ģʽѡ��:1����ģʽ��2�ֶ�ģʽ
        {
			clue();
            i = Interface1(i);
        }
        if (model == 1)
        {
			if(timecount4 == Maxtime) {
				timecount4 = 0;
				temp = GY30_ReadLight();//���ռ��
				if(temp < tempFlag) {
					GPIO_SetBits(LED_GPIO_PORT, LED_PIN);
				}
				else {
					GPIO_ResetBits(LED_GPIO_PORT, LED_PIN);
				}
			}
			getResultTemp();					//������2���յ�������������Ϣ
			objectDetected = isObjectInRange(); //������⿪��
			switch(objectDetected) {
				case 1:
					SG90Flag = 1;
					classify = 1;
					SG90_PWM1 = OPEN;//�ɻ�������
					printf("�ɻ�������\n");
					break;
				case 2:
					SG90Flag = 1;
					classify = 2;
					SG90_PWM2 = OPEN;//�к�����
					printf("�к�����\n");
					break;
				case 3:
					SG90Flag = 1;
					classify = 3;
					SG90_PWM3 = OPEN;//��������
					printf("��������\n");
					break;
				case 4:
					SG90Flag = 1;
					classify = 4;
					SG90_PWM4 = OPEN;//��������
					printf("��������\n");
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
			SG90_PWM1 = CLOSE; // �ر�����Ͱ��
			SG90_PWM2 = CLOSE; // �ر�����Ͱ��
			SG90_PWM3 = CLOSE; // �ر�����Ͱ��
			SG90_PWM4 = CLOSE; // �ر�����Ͱ��
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



