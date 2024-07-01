#include "../oled/OLED.h"
#include "OLED_Font.h"

#define KEY_ON	0
#define KEY_OFF	1
//extern uint8_t classify;
//extern uint8_t classFlag;
//extern uint8_t model;
extern int Maxtime;
extern float tempFlag;
/*��������*/
#define OLED_SCL GPIO_Pin_12	//OLED����SCL�Ž�GPIOB12
#define OLED_SDA GPIO_Pin_13	//OLED����SDA�Ž�GPIOB13
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, OLED_SCL, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, OLED_SDA, (BitAction)(x))

/*���ų�ʼ��*/
void OLED_I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = OLED_SCL;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = OLED_SDA;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

// https://blog.zeruns.tech

/**
  * @brief  I2C��ʼ
  * @param  ��
  * @retval ��
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2Cֹͣ
  * @param  ��
  * @retval ��
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C����һ���ֽ�
  * @param  Byte Ҫ���͵�һ���ֽ�
  * @retval ��
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//�����һ��ʱ�ӣ�������Ӧ���ź�
	OLED_W_SCL(0);
}

/**
  * @brief  OLEDд����
  * @param  Command Ҫд�������
  * @retval ��
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//�ӻ���ַ
	OLED_I2C_SendByte(0x00);		//д����
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLEDд����
  * @param  Data Ҫд�������
  * @retval ��
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//�ӻ���ַ
	OLED_I2C_SendByte(0x40);		//д����
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED���ù��λ��
  * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
  * @retval ��
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//����Yλ��
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//����Xλ�õ�4λ
	OLED_WriteCommand(0x00 | (X & 0x0F));			//����Xλ�ø�4λ
}

/**
  * @brief  OLED����
  * @param  ��
  * @retval ��
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED��������
  * @param  Line ��λ�ã���Χ��1~4
  * @param  start �п�ʼλ�ã���Χ��1~16
  * @param  end �п�ʼλ�ã���Χ��1~16
  * @retval ��
  */
void OLED_Clear_Part(uint8_t Line, uint8_t start, uint8_t end)
{  
	uint8_t i,Column;
	for(Column = start; Column <= end; Column++)
	{
		OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//���ù��λ�����ϰ벿��
		for (i = 0; i < 8; i++)
		{
			OLED_WriteData(0x00);			//��ʾ�ϰ벿������
		}
		OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//���ù��λ�����°벿��
		for (i = 0; i < 8; i++)
		{
			OLED_WriteData(0x00);		//��ʾ�°벿������
		}
	}
}


/**
  * @brief  OLED��ʾһ���ַ�
  * @param  Line ��λ�ã���Χ��1~4
  * @param  Column ��λ�ã���Χ��1~16
  * @param  Char Ҫ��ʾ��һ���ַ�����Χ��ASCII�ɼ��ַ�
  * @retval ��
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//���ù��λ�����ϰ벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//��ʾ�ϰ벿������
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//���ù��λ�����°벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//��ʾ�°벿������
	}
}

/**
  * @brief  OLED��ʾ�ַ���
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  String Ҫ��ʾ���ַ�������Χ��ASCII�ɼ��ַ�
  * @retval ��
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED�η�����
  * @retval ����ֵ����X��Y�η�
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED��ʾ���֣�ʮ���ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~4294967295
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @retval ��
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED��ʾ���֣�ʮ���ƣ�����������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��-2147483648~2147483647
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @retval ��
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED��ʾ���֣�ʮ�����ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~0xFFFFFFFF
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~8
  * @retval ��
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED��ʾ���֣������ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~1111 1111 1111 1111
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~16
  * @retval ��
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED��ʾ�����ַ�
  * @param  Line ��λ�ã���Χ��1~4
  * @param  Column ��λ�ã���Χ��1~16
  * @param  Index �ֿ��е������ַ�����
  * @retval ��
  */
void OLED_ShowChinese1(uint8_t Line, uint8_t Column, uint16_t Index, uint8_t size)
{
    uint8_t i;
    uint16_t character_offset = Index * 2; // ÿ�������ַ�ռ32�ֽ�, 2������

    // ��ʾ��һ��
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (i = 0; i < size; i++)
    {
        OLED_WriteData(Hzk1[character_offset][i]);
    }

    // ��ʾ�ڶ���
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (i = 0; i < size; i++)
    {
        OLED_WriteData(Hzk1[character_offset + 1][i]);
    }
}

void OLED_ShowChinese2(uint8_t Line, uint8_t Column, uint16_t Index, uint8_t size)
{
    uint8_t i;
    uint16_t character_offset = Index * 2; // ÿ�������ַ�ռ32�ֽ�, 2������

    // ��ʾ��һ��
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (i = 0; i < size; i++)
    {
        OLED_WriteData(Hzk2[character_offset][i]);
    }

    // ��ʾ�ڶ���
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (i = 0; i < size; i++)
    {
        OLED_WriteData(Hzk2[character_offset + 1][i]);
    }
}

/**
  * @brief  OLED��ʼ��
  * @param  ��
  * @retval ��
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//�ϵ���ʱ
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//�˿ڳ�ʼ��
	
	OLED_WriteCommand(0xAE);	//�ر���ʾ
	
	OLED_WriteCommand(0xD5);	//������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//���ö�·������
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//������ʾƫ��
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//������ʾ��ʼ��
	
	OLED_WriteCommand(0xA1);	//�������ҷ���0xA1���� 0xA0���ҷ���
	
	OLED_WriteCommand(0xC8);	//�������·���0xC8���� 0xC0���·���

	OLED_WriteCommand(0xDA);	//����COM����Ӳ������
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//���öԱȶȿ���
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//����Ԥ�������
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//����VCOMHȡ��ѡ�񼶱�
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//����������ʾ��/�ر�

	OLED_WriteCommand(0xA6);	//��������/��ת��ʾ

	OLED_WriteCommand(0x8D);	//���ó���
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//������ʾ
		
	OLED_Clear();				//OLED����
}


uint8_t Interface1(uint8_t i)
{
	OLED_ShowChinese2(1, 5, 0, 12);//��
	OLED_ShowChinese2(1, 7, 1, 12);//��
	OLED_ShowChinese2(1, 9, 2, 12);//ģ
	OLED_ShowChinese2(1, 11, 3, 12);//ʽ
	
	OLED_ShowChinese2(2, 5, 4, 12);//��
	OLED_ShowChinese2(2, 7, 5, 12);//��
	OLED_ShowChinese2(2, 9, 2, 12);//ģ
	OLED_ShowChinese2(2, 11, 3, 12);//ʽ
	
	OLED_ShowChinese2(3, 7, 29, 12);//��
	OLED_ShowChinese2(3, 9, 30, 12);//��
	
	OLED_ShowString(i, 4, ">");		//OLED������ַ���
	if( Key_Scan(ConfirmKey_GPIO_PORT,ConfirmKey_PIN) == KEY_ON  )
	{
		OLED_Clear();
		if(i == 1) 
		{
			model = 1;
			TIM_Cmd(TIM3, ENABLE);
			clue();
		}
		if(i == 2) 
		{
			model = 2;
			i = 1;
			TIM_Cmd(TIM3, ENABLE);
			clue();		
		}
		if(i == 3) {
			seting();
			BackupModel(); // ����
		}
		
	}
	if( Key_Scan(SelectKey_GPIO_PORT,SelectKey_PIN) == KEY_ON  )
	{
		if(i == 3)
		{
			OLED_Clear_Part(i, 4, 4);
			i = 0;
		}
		OLED_Clear_Part(i, 4, 4);
		i = i+1;
		
	}
	return i;
}

uint8_t Interface2(uint8_t i)
{
	if(model == 1)
	{
		OLED_ShowString(1, 3, ">>");		//OLED������ַ���
		OLED_ShowChinese1(1, 5, 0, 16);//��
		OLED_ShowChinese1(1, 7, 1, 16);//��
		OLED_ShowChinese1(1, 9, 2, 16);//ģ
		OLED_ShowChinese1(1, 11, 3, 16);//ʽ
		OLED_ShowString(1, 13, "<<");		//OLED������ַ���
		
		i = classify;
	}
	else if(model ==2)
	{
		OLED_ShowString(1, 3, ">>");
		OLED_ShowChinese1(1, 5, 4, 16);//��
		OLED_ShowChinese1(1, 7, 5, 16);//��
		OLED_ShowChinese1(1, 9, 2, 16);//ģ
		OLED_ShowChinese1(1, 11, 3, 16);//ʽ
		OLED_ShowString(1, 13, "<<");
	}
	if( Key_Scan(ConfirmKey_GPIO_PORT,ConfirmKey_PIN) == KEY_ON  && model == 2)
	{
		switch(i)
		{
			case 1:
				if(SG90Flag == 0){
					SG90Flag = 1;SG90_PWM1 = OPEN;  //�ɻ���-��
				}
				else {
					SG90Flag = 0;SG90_PWM1 = CLOSE;	//��
				}		 
				break;
			case 2:
				if(SG90Flag == 0){
					SG90Flag = 1;SG90_PWM2 = OPEN;  //�к�-��
				}
				else {
					SG90Flag = 0;SG90_PWM2 = CLOSE;	//��
				}	
				break;
			case 3:
				if(SG90Flag == 0){
					SG90Flag = 1;SG90_PWM3 = OPEN;  //����-��
				}
				else {
					SG90Flag = 0;SG90_PWM3 = CLOSE;	//��
				}	
				break;
			case 4:
				if(SG90Flag == 0){
					SG90Flag = 1;SG90_PWM4 = OPEN;  //����-��
				}
				else {
					SG90Flag = 0;SG90_PWM4 = CLOSE;	//��
				}	
				break;
			case 5:
				GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)((1-GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5))));//LED1��ת
				break;
			default:
				break;
		}
		if(i > 0 && i < 5) {
			if(SG90Flag == 0) OLED_ShowChinese2(3, 14, 19, 12);//��
			if(SG90Flag == 1) OLED_ShowChinese2(3, 14, 18, 12);//��
		}
		if(i >= 5) {
			if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5) == Bit_SET) {
				OLED_ShowChinese2(3, 14, 18, 12);//��
			}
			else{
				OLED_ShowChinese2(3, 14, 19, 12);//��
			}
		}
	}
	if(i > 0 && i < 5) {
			if(SG90Flag == 0) OLED_ShowChinese2(3, 14, 19, 12);//��
			if(SG90Flag == 1) OLED_ShowChinese2(3, 14, 18, 12);//��
		}
		if(i >= 5) {
			if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5) == Bit_SET) {
				OLED_ShowChinese2(3, 14, 18, 12);//��
			}
			else{
				OLED_ShowChinese2(3, 14, 19, 12);//��
			}
		}
	if( (Key_Scan(SelectKey_GPIO_PORT,SelectKey_PIN) == KEY_ON)  && (model == 2) &&(SG90Flag == 0))
	{
		OLED_Clear_Part(3, 4, 16);
		if(i == 5)
		{
			i = 1;
		}
		else {
			i = i+1;
		}
	}
	
	if(i != classFlag && model == 1)
	{
		OLED_Clear_Part(3, 4, 13);
	}
	delay_ms(500);
	switch(i)
	{
		case 0:
			OLED_ShowChinese2(3, 6, 20, 12);//��
			OLED_ShowChinese2(3, 8, 15, 12);//��
			OLED_ShowChinese2(3, 10, 16, 12);//��
			break;
		case 1:
			OLED_ShowChinese2(3, 4, 6, 12);//��
			OLED_ShowChinese2(3, 6, 7, 12);//��
			OLED_ShowChinese2(3, 8, 8, 12);//��
			OLED_ShowChinese2(3, 10, 15, 12);//��
			OLED_ShowChinese2(3, 12, 16, 12);//��
			break;
		case 2:
			OLED_ShowChinese2(3, 5, 11, 12);//��
			OLED_ShowChinese2(3, 7, 12, 12);//��
			OLED_ShowChinese2(3, 9, 15, 12);//��
			OLED_ShowChinese2(3, 11, 16, 12);//��
			break;
		case 3:
			OLED_ShowChinese2(3, 5, 9, 12);//��
			OLED_ShowChinese2(3, 7, 10, 12);//��
			OLED_ShowChinese2(3, 9, 15, 12);//��
			OLED_ShowChinese2(3, 11, 16, 12);//��
			break;
		case 4:
			OLED_ShowChinese2(3, 5, 13, 12);//��
			OLED_ShowChinese2(3, 7, 14, 12);//��
			OLED_ShowChinese2(3, 9, 15, 12);//��
			OLED_ShowChinese2(3, 11, 16, 12);//��
			break;
		default:
			OLED_ShowChinese2(3, 6, 21, 12);//��
			OLED_ShowChinese2(3, 8, 22, 12);//��
			OLED_ShowChinese2(3, 10, 17, 12);//��
			break;
	}
	classFlag = i;
	return i;
}

uint64_t numlen(uint64_t num)//�������ֵĳ���
{
    uint64_t len = 1;        // ��ʼ����Ϊ1
    for(; num > 9; ++len)    // �ж�num�Ƿ����9�����򳤶�+1
        num /= 10;	         // ʹ�ó����������㣬ֱ��numС��1
    return len;              // ���س��ȵ�ֵ
}

void seting(void)
{
	int select = 0, keyflag = 1, i = 1;

	OLED_ShowChinese2(1, 8, 29, 12);//��
	OLED_ShowChinese2(1, 10, 30, 12);//��
	clue();
	while(1)
	{
		if(i == 1 && keyflag == 1) {
			keyflag = 0;
			OLED_ShowChinese2(1, 8, 29, 12);//��
			OLED_ShowChinese2(1, 10, 30, 12);//��
			
			OLED_Clear_Part(2, 1, 16);
			OLED_ShowChinese2(2, 2, 33, 12);//��
			OLED_ShowChinese2(2, 4, 34, 12);//��
			OLED_ShowChinese2(2, 6, 35, 12);//��
			OLED_ShowChinese2(2, 8, 36, 12);//ʱ
			OLED_ShowString(2, 10, ":");
			OLED_ShowNum(2, 12, Maxtime, numlen(Maxtime));
		}
		if(i == 2 && keyflag == 1) {
			keyflag = 0;
			OLED_ShowChinese2(1, 8, 29, 12);//��
			OLED_ShowChinese2(1, 10, 30, 12);//��
			
			OLED_Clear_Part(2, 1, 16);
			OLED_ShowChinese2(2, 2, 37, 12);//��
			OLED_ShowChinese2(2, 4, 38, 12);//��
			OLED_ShowChinese2(2, 6, 39, 12);//��
			OLED_ShowChinese2(2, 8, 40, 12);//ֵ
			OLED_ShowString(2, 10, ":");
			OLED_ShowNum(2, 12, tempFlag, numlen(tempFlag));
		}
//		if(select != 0)
//		{
//			OLED_Clear_Part(4, 1, 16);
//			OLED_ShowChinese2(4, 2, 23, 12);//ȷ
//			OLED_ShowChinese2(4, 4, 24, 12);//��
//			OLED_ShowChinese2(4, 8, 31, 12);//��
//			OLED_ShowChinese2(4, 14, 32, 12);//��
//		}
//		if(select == 0)
//		{
//			OLED_Clear_Part(4, 1, 16);
//			clue();
//		}
		if( (Key_Scan(SelectKey_GPIO_PORT, SelectKey_PIN) == KEY_ON) && select == 0)
		{
			keyflag = 1;
			OLED_Clear_Part(2, 1, 16);
			if(i == 2)
			{
				i = 1;
			}
			else {
				i = i+1;
			}
		}
		if( Key_Scan(ConfirmKey_GPIO_PORT, ConfirmKey_PIN) == KEY_ON)
		{
			if(select != 0)
			{
				select = 0;
				OLED_Clear_Part(4, 1, 16);
				clue();
				continue;
			}
			if(select == 0)
			{
				if(i == 1) select = 1;
				if(i == 2) select = 2;
				OLED_Clear_Part(4, 1, 16);
				OLED_ShowChinese2(4, 2, 23, 12);//ȷ
				OLED_ShowChinese2(4, 4, 24, 12);//��
				OLED_ShowChinese2(4, 8, 31, 12);//��
				OLED_ShowChinese2(4, 14, 32, 12);//��
			}	
		}
		if( (Key_Scan(SelectKey_GPIO_PORT,SelectKey_PIN) == KEY_ON) && select != 0) 
		{
			OLED_Clear_Part(2, 12, 16);
			if(select == 1) {
				Maxtime = Maxtime+1;
				OLED_ShowNum(2, 12, Maxtime, numlen(Maxtime));
			}
			if(select == 2) {
				tempFlag = tempFlag+1;
				OLED_ShowNum(2, 12, tempFlag, numlen(tempFlag));
			}
		}
		if (Key_Scan(EscKey_GPIO_PORT, EscKey_PIN) == KEY_ON && select != 0) 
		{
			OLED_Clear_Part(2, 12, 16);
			if(select == 1) {
				Maxtime = Maxtime-1;
				OLED_ShowNum(2, 12, Maxtime, numlen(Maxtime));
			}
			if(select == 2) {
				tempFlag = tempFlag-1;
				OLED_ShowNum(2, 12, tempFlag, numlen(tempFlag));
			}
		}
		if (Key_Scan(EscKey_GPIO_PORT, EscKey_PIN) == KEY_ON && select == 0) 
		{
			OLED_Clear();
			break;
		}
	}
}

void clue(void)
{
	OLED_ShowChinese2(4, 2, 23, 12);//
	OLED_ShowChinese2(4, 4, 24, 12);//
	
	OLED_ShowChinese2(4, 8, 25, 12);//
	OLED_ShowChinese2(4, 10, 26, 12);//
	
	OLED_ShowChinese2(4, 13, 27, 12);//
	OLED_ShowChinese2(4, 15, 28, 12);//
}
