#ifndef __CONFIG_H
#define __CONFIG_H


#include "../gpio/gpio.h"
#include "../exti/exti.h"
#include "adc.h"
#include "timer.h"
#include "led.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h" 
#include "usart3.h"
#include "SysTick.h"
#include "sys.h"
#include "../bh1750/bh1750.h"
#include "../oled/OLED.h"
#include "../KEY/key.h"
#include "../hcsr04/HCSR04.h"
//#include "../ultrasonic/ultrasonic.h"

/************************* ���峣�� *************************/
//#define Maxtime 60              // ��ʱ����/��    60�����˻����������ģʽ
#define USART2_REC_LEN 100      // ����2���ջ���������
#define USART3_REC_LEN 100      // ����3���ջ���������

#define MODE_BACKUP_REG BKP_DR1  // ʹ�ñ��ݼĴ���1���洢model����
#define TIME_BACKUP_REG BKP_DR2
#define TEMP_BACKUP_REG BKP_DR3

/************************* �ⲿ�������� *************************/
extern u8 IRQURA3;                         // ����3 ����������ϱ�־ 
extern u8 IRQURA2;                         // ����2 ����������ϱ�־ 
extern u8 USART2_RX_BUF[USART2_REC_LEN];   // ����2�������ݻ�����
extern u8 USART3_RX_BUF[USART3_REC_LEN];   // ����3�������ݻ�����
extern volatile u8 timecount1;             // ��ʱ��1��ʱ
extern volatile u8 timecount2;             // ����ָ���ʱ
extern volatile u8 timecount3;             // ������ʱ
extern volatile u8 timecount4;
extern volatile u8 timecount5;

extern volatile uint8_t classify;
extern volatile uint8_t classFlag;

extern volatile uint8_t sensor_last_state;
extern volatile uint8_t model;
extern volatile uint64_t time;

/****************** �ж����ȼ����� ******************/
#define USART_PREEMPTION_PRIORITY   3   // �����ж���ռ���ȼ�
#define USART_SUB_PRIORITY          3   // �����ж������ȼ�

#define EXTI_PREEMPTION_PRIORITY    1   // �ⲿ�ж���ռ���ȼ�
#define EXTI_SUB_PRIORITY           1   // �ⲿ�ж������ȼ�

#define TIM2_PREEMPTION_PRIORITY    2   // TIM2�ж���ռ���ȼ�
#define TIM2_SUB_PRIORITY           2   // TIM2�ж������ȼ�

#define TIM3_PREEMPTION_PRIORITY    4   // TIM3�ж���ռ���ȼ�
#define TIM3_SUB_PRIORITY           4   // TIM3�ж������ȼ�

/****************** �����ж����������� ******************/

#define USART2_IRQ_INDEX    USART2_IRQn

#define EXTI_IRQ_INDEX      EXTI0_IRQn

#define TIM2_IRQ_INDEX      TIM2_IRQn

#define TIM3_IRQ_INDEX      TIM3_IRQn

/****************** ������ģ�鶨�� ******************/

#define Echo1 GPIO_Pin_4		//HC-SR04ģ�� 1 ��Echo�Ž�GPIOB4
#define Trig1 GPIO_Pin_5		//HC-SR04ģ�� 1 ��Trig�Ž�GPIOB5
#define Trig1_GPIO_PORT GPIOB

#define Echo2 GPIO_Pin_8		//HC-SR04ģ�� 2 
#define Trig2 GPIO_Pin_9
#define Trig2_GPIO_PORT GPIOB

#define Echo3 GPIO_Pin_14		//HC-SR04ģ�� 3 
#define Trig3 GPIO_Pin_15
#define Trig3_GPIO_PORT GPIOB

#define Echo4 GPIO_Pin_11		//HC-SR04ģ�� 4 
#define Trig4 GPIO_Pin_12
#define Trig4_GPIO_PORT GPIOA

/****************** �������� ******************/

#define ConfirmKey_PIN      GPIO_Pin_4 // ȷ��
#define ConfirmKey_GPIO_PORT GPIOA

#define SelectKey_PIN       GPIO_Pin_1 // ѡ��
#define SelectKey_GPIO_PORT GPIOA

#define EscKey_PIN          GPIO_Pin_6 // �˳�
#define EscKey_GPIO_PORT    GPIOA

/****************** LED���� ******************/

#define LED_PIN             GPIO_Pin_5  // 5V LED��
#define LED_GPIO_PORT       GPIOA

/****************** �ⲿ�жϴ������� ******************/

#define TRIGGER_PIN         GPIO_Pin_0  			// �ⲿ�жϴ�������
#define TRIGGER_PORT        GPIOA       			// GPIO�˿�ΪGPIOA
#define TRIGGER_PORT_SOURCE GPIO_PortSourceGPIOA 	// GPIO�˿�ԴΪGPIOA
#define TRIGGER_PIN_SOURCE  GPIO_PinSource0     	// GPIO����ԴΪPA0

/****************** SG90 ������� ******************/

#define SG90_1_PIN          GPIO_Pin_7
#define SG90_1_GPIO_PORT    GPIOA
#define SG90_2_PIN          GPIO_Pin_1
#define SG90_2_GPIO_PORT    GPIOB
#define SG90_3_PIN          GPIO_Pin_10
#define SG90_3_GPIO_PORT    GPIOB
#define SG90_4_PIN          GPIO_Pin_11
#define SG90_4_GPIO_PORT    GPIOB

#define SG90_1              PAout(7)
#define SG90_2              PBout(1)
#define SG90_3              PBout(10)
#define SG90_4              PBout(11)


#endif
