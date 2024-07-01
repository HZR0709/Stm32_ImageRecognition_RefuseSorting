#include "../oled/OLED.h"
#include "OLED_Font.h"

#define KEY_ON	0
#define KEY_OFF	1
//extern uint8_t classify;
//extern uint8_t classFlag;
//extern uint8_t model;
extern int Maxtime;
extern float tempFlag;
/*引脚配置*/
#define OLED_SCL GPIO_Pin_12	//OLED屏的SCL脚接GPIOB12
#define OLED_SDA GPIO_Pin_13	//OLED屏的SDA脚接GPIOB13
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, OLED_SCL, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, OLED_SDA, (BitAction)(x))

/*引脚初始化*/
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
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
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
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置低4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置高4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
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
  * @brief  OLED部分清屏
  * @param  Line 行位置，范围：1~4
  * @param  start 列开始位置，范围：1~16
  * @param  end 列开始位置，范围：1~16
  * @retval 无
  */
void OLED_Clear_Part(uint8_t Line, uint8_t start, uint8_t end)
{  
	uint8_t i,Column;
	for(Column = start; Column <= end; Column++)
	{
		OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
		for (i = 0; i < 8; i++)
		{
			OLED_WriteData(0x00);			//显示上半部分内容
		}
		OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
		for (i = 0; i < 8; i++)
		{
			OLED_WriteData(0x00);		//显示下半部分内容
		}
	}
}


/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
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
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
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
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
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
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
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
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
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
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
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
  * @brief  OLED显示中文字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Index 字库中的中文字符索引
  * @retval 无
  */
void OLED_ShowChinese1(uint8_t Line, uint8_t Column, uint16_t Index, uint8_t size)
{
    uint8_t i;
    uint16_t character_offset = Index * 2; // 每个中文字符占32字节, 2行数据

    // 显示第一行
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (i = 0; i < size; i++)
    {
        OLED_WriteData(Hzk1[character_offset][i]);
    }

    // 显示第二行
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (i = 0; i < size; i++)
    {
        OLED_WriteData(Hzk1[character_offset + 1][i]);
    }
}

void OLED_ShowChinese2(uint8_t Line, uint8_t Column, uint16_t Index, uint8_t size)
{
    uint8_t i;
    uint16_t character_offset = Index * 2; // 每个中文字符占32字节, 2行数据

    // 显示第一行
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (i = 0; i < size; i++)
    {
        OLED_WriteData(Hzk2[character_offset][i]);
    }

    // 显示第二行
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (i = 0; i < size; i++)
    {
        OLED_WriteData(Hzk2[character_offset + 1][i]);
    }
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}


uint8_t Interface1(uint8_t i)
{
	OLED_ShowChinese2(1, 5, 0, 12);//智
	OLED_ShowChinese2(1, 7, 1, 12);//能
	OLED_ShowChinese2(1, 9, 2, 12);//模
	OLED_ShowChinese2(1, 11, 3, 12);//式
	
	OLED_ShowChinese2(2, 5, 4, 12);//手
	OLED_ShowChinese2(2, 7, 5, 12);//动
	OLED_ShowChinese2(2, 9, 2, 12);//模
	OLED_ShowChinese2(2, 11, 3, 12);//式
	
	OLED_ShowChinese2(3, 7, 29, 12);//设
	OLED_ShowChinese2(3, 9, 30, 12);//置
	
	OLED_ShowString(i, 4, ">");		//OLED屏输出字符串
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
			BackupModel(); // 备份
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
		OLED_ShowString(1, 3, ">>");		//OLED屏输出字符串
		OLED_ShowChinese1(1, 5, 0, 16);//智
		OLED_ShowChinese1(1, 7, 1, 16);//能
		OLED_ShowChinese1(1, 9, 2, 16);//模
		OLED_ShowChinese1(1, 11, 3, 16);//式
		OLED_ShowString(1, 13, "<<");		//OLED屏输出字符串
		
		i = classify;
	}
	else if(model ==2)
	{
		OLED_ShowString(1, 3, ">>");
		OLED_ShowChinese1(1, 5, 4, 16);//手
		OLED_ShowChinese1(1, 7, 5, 16);//动
		OLED_ShowChinese1(1, 9, 2, 16);//模
		OLED_ShowChinese1(1, 11, 3, 16);//式
		OLED_ShowString(1, 13, "<<");
	}
	if( Key_Scan(ConfirmKey_GPIO_PORT,ConfirmKey_PIN) == KEY_ON  && model == 2)
	{
		switch(i)
		{
			case 1:
				if(SG90Flag == 0){
					SG90Flag = 1;SG90_PWM1 = OPEN;  //可回收-开
				}
				else {
					SG90Flag = 0;SG90_PWM1 = CLOSE;	//关
				}		 
				break;
			case 2:
				if(SG90Flag == 0){
					SG90Flag = 1;SG90_PWM2 = OPEN;  //有害-开
				}
				else {
					SG90Flag = 0;SG90_PWM2 = CLOSE;	//关
				}	
				break;
			case 3:
				if(SG90Flag == 0){
					SG90Flag = 1;SG90_PWM3 = OPEN;  //厨余-开
				}
				else {
					SG90Flag = 0;SG90_PWM3 = CLOSE;	//关
				}	
				break;
			case 4:
				if(SG90Flag == 0){
					SG90Flag = 1;SG90_PWM4 = OPEN;  //其它-开
				}
				else {
					SG90Flag = 0;SG90_PWM4 = CLOSE;	//关
				}	
				break;
			case 5:
				GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)((1-GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5))));//LED1翻转
				break;
			default:
				break;
		}
		if(i > 0 && i < 5) {
			if(SG90Flag == 0) OLED_ShowChinese2(3, 14, 19, 12);//关
			if(SG90Flag == 1) OLED_ShowChinese2(3, 14, 18, 12);//开
		}
		if(i >= 5) {
			if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5) == Bit_SET) {
				OLED_ShowChinese2(3, 14, 18, 12);//开
			}
			else{
				OLED_ShowChinese2(3, 14, 19, 12);//关
			}
		}
	}
	if(i > 0 && i < 5) {
			if(SG90Flag == 0) OLED_ShowChinese2(3, 14, 19, 12);//关
			if(SG90Flag == 1) OLED_ShowChinese2(3, 14, 18, 12);//开
		}
		if(i >= 5) {
			if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5) == Bit_SET) {
				OLED_ShowChinese2(3, 14, 18, 12);//开
			}
			else{
				OLED_ShowChinese2(3, 14, 19, 12);//关
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
			OLED_ShowChinese2(3, 6, 20, 12);//无
			OLED_ShowChinese2(3, 8, 15, 12);//垃
			OLED_ShowChinese2(3, 10, 16, 12);//圾
			break;
		case 1:
			OLED_ShowChinese2(3, 4, 6, 12);//可
			OLED_ShowChinese2(3, 6, 7, 12);//回
			OLED_ShowChinese2(3, 8, 8, 12);//收
			OLED_ShowChinese2(3, 10, 15, 12);//垃
			OLED_ShowChinese2(3, 12, 16, 12);//圾
			break;
		case 2:
			OLED_ShowChinese2(3, 5, 11, 12);//有
			OLED_ShowChinese2(3, 7, 12, 12);//害
			OLED_ShowChinese2(3, 9, 15, 12);//垃
			OLED_ShowChinese2(3, 11, 16, 12);//圾
			break;
		case 3:
			OLED_ShowChinese2(3, 5, 9, 12);//厨
			OLED_ShowChinese2(3, 7, 10, 12);//余
			OLED_ShowChinese2(3, 9, 15, 12);//垃
			OLED_ShowChinese2(3, 11, 16, 12);//圾
			break;
		case 4:
			OLED_ShowChinese2(3, 5, 13, 12);//其
			OLED_ShowChinese2(3, 7, 14, 12);//它
			OLED_ShowChinese2(3, 9, 15, 12);//垃
			OLED_ShowChinese2(3, 11, 16, 12);//圾
			break;
		default:
			OLED_ShowChinese2(3, 6, 21, 12);//照
			OLED_ShowChinese2(3, 8, 22, 12);//明
			OLED_ShowChinese2(3, 10, 17, 12);//灯
			break;
	}
	classFlag = i;
	return i;
}

uint64_t numlen(uint64_t num)//计算数字的长度
{
    uint64_t len = 1;        // 初始长度为1
    for(; num > 9; ++len)    // 判断num是否大于9，否则长度+1
        num /= 10;	         // 使用除法进行运算，直到num小于1
    return len;              // 返回长度的值
}

void seting(void)
{
	int select = 0, keyflag = 1, i = 1;

	OLED_ShowChinese2(1, 8, 29, 12);//设
	OLED_ShowChinese2(1, 10, 30, 12);//置
	clue();
	while(1)
	{
		if(i == 1 && keyflag == 1) {
			keyflag = 0;
			OLED_ShowChinese2(1, 8, 29, 12);//设
			OLED_ShowChinese2(1, 10, 30, 12);//置
			
			OLED_Clear_Part(2, 1, 16);
			OLED_ShowChinese2(2, 2, 33, 12);//待
			OLED_ShowChinese2(2, 4, 34, 12);//机
			OLED_ShowChinese2(2, 6, 35, 12);//超
			OLED_ShowChinese2(2, 8, 36, 12);//时
			OLED_ShowString(2, 10, ":");
			OLED_ShowNum(2, 12, Maxtime, numlen(Maxtime));
		}
		if(i == 2 && keyflag == 1) {
			keyflag = 0;
			OLED_ShowChinese2(1, 8, 29, 12);//设
			OLED_ShowChinese2(1, 10, 30, 12);//置
			
			OLED_Clear_Part(2, 1, 16);
			OLED_ShowChinese2(2, 2, 37, 12);//光
			OLED_ShowChinese2(2, 4, 38, 12);//照
			OLED_ShowChinese2(2, 6, 39, 12);//阈
			OLED_ShowChinese2(2, 8, 40, 12);//值
			OLED_ShowString(2, 10, ":");
			OLED_ShowNum(2, 12, tempFlag, numlen(tempFlag));
		}
//		if(select != 0)
//		{
//			OLED_Clear_Part(4, 1, 16);
//			OLED_ShowChinese2(4, 2, 23, 12);//确
//			OLED_ShowChinese2(4, 4, 24, 12);//认
//			OLED_ShowChinese2(4, 8, 31, 12);//加
//			OLED_ShowChinese2(4, 14, 32, 12);//减
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
				OLED_ShowChinese2(4, 2, 23, 12);//确
				OLED_ShowChinese2(4, 4, 24, 12);//认
				OLED_ShowChinese2(4, 8, 31, 12);//加
				OLED_ShowChinese2(4, 14, 32, 12);//减
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
