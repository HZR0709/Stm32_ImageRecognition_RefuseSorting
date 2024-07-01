#ifndef __HCSR04_H
#define __HCSR04_H

void HC_SR04_Init(void);
int16_t sonar_mm(void);
float sonar(void);

void checkSensorConnection(GPIO_TypeDef* trigPort, uint16_t trigPin, GPIO_TypeDef* echoPort, uint16_t echoPin, int sensorIndex);

uint8_t isObjectInRange(void);

#endif
