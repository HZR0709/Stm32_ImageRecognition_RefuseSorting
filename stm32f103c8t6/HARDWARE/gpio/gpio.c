#include "../gpio/gpio.h"

void GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	//  SG90-1 ���  �ɻ���
	GPIO_InitStructure.GPIO_Pin = SG90_1_PIN;				 // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50
	GPIO_Init(SG90_1_GPIO_PORT, &GPIO_InitStructure);	  				 //������� ��
	//  SG90-2 ���     �к�
	GPIO_InitStructure.GPIO_Pin = SG90_2_PIN;				 // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50
	GPIO_Init(SG90_2_GPIO_PORT, &GPIO_InitStructure);	
	//  SG90-3 ���     ����
	GPIO_InitStructure.GPIO_Pin = SG90_3_PIN;				 // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50
	GPIO_Init(SG90_3_GPIO_PORT, &GPIO_InitStructure);
	//  SG90-4 ���      ����
	GPIO_InitStructure.GPIO_Pin = SG90_4_PIN;				 // 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50
	GPIO_Init(SG90_4_GPIO_PORT, &GPIO_InitStructure);
	
    // 5V-LED��  PA5
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(LED_GPIO_PORT, LED_PIN);	 // �ر�����LED
	
	//GY30 ����ǿ�ȸ�Ӧ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = sda | scl ;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	 // HC-SR501��������Ӧ
    GPIO_InitStructure.GPIO_Pin = TRIGGER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(TRIGGER_PORT, &GPIO_InitStructure);
}
