#ifndef __BH1750_H__
#define __BH1750_H__

#include "stm32f10x.h"
#include "../config.h"
#define uchar unsigned char 
#define uint  unsigned int

#define sda   GPIO_Pin_7    //20
#define scl   GPIO_Pin_6    //21

#define	  SlaveAddress   0x46   //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
                              //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A
															
extern uchar    BUF[8];                         //�������ݻ�����      	
extern int     dis_data;                       //����		
extern int   mcy;              //��ʾ��λ��־λ

void Init_BH1750(void);
void conversion(uint temp_data);
void  Single_Write_BH1750(uchar REG_Address);//����д������
uchar Single_Read_BH1750(uchar REG_Address);   //������ȡ�ڲ��Ĵ�������
void  mread(void);         //�����Ķ�ȡ�ڲ��Ĵ�������
float GY30_ReadLight(void);

void Delay_5us(void);
void Delay_mms(uint16_t tmp);
void GPIOConfig(void);

#endif

