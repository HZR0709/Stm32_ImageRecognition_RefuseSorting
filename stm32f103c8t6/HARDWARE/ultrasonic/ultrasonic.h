#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#include "stm32f10x.h"
#include "../config.h"

//void Ultrasonic_Init(void);
uint32_t Ultrasonic_GetDistance(void);

#endif // ULTRASONIC_H
