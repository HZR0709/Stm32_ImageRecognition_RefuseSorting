#ifndef __OLED_H
#define __OLED_H
#include "stm32f10x.h"
#include "../config.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowChinese1(uint8_t Line, uint8_t Column, uint16_t Index, uint8_t size);
void OLED_ShowChinese2(uint8_t Line, uint8_t Column, uint16_t Index, uint8_t size);
void OLED_Clear_Part(uint8_t Line, uint8_t start, uint8_t end);

uint8_t Interface1(uint8_t i);
uint8_t Interface2(uint8_t i);
uint64_t numlen(uint64_t num);//计算数字的长度
void seting(void);
void clue(void);

#endif
