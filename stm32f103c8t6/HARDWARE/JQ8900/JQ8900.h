#ifndef __JQ8900_H 
#define __JQ8900_H

#include "delay.h"
 
 
#define VBUSY  PAout(6)// P 
#define VSDA   PAout(5)// P 
 
 

 

void JQ8900_Init(void);
void SendJQ8900Data (u8 addr);
void JQ8900Speak(u8 num);

#endif
