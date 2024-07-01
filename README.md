# Stm32_ImageRecognition_RefuseSorting
 基于stm32的图像识别智能垃圾分类系统设计
双处理器实现，k210作图像处理单元，stm32f103c8t6作核心控制器。
## 主要有以下功能：
### 智能模式：
* 图像识别到垃圾类别后控制舵机打开对应垃圾箱盖
* 感应开关触发后打开对应的垃圾箱盖
* HC-SR501感应到环境无人后倒计时time秒后进入低功耗待机模式，其中time可手动设置。
* GY30感知环境光照低于阈值tempFlag时打开聚光灯，为图像识别提供照明，其中tempFlag可手动设置。
* OLED实时显示系统状态和垃圾分类结果
### 手动模式
* 按键控制选择打开对应的垃圾箱盖
* 按键控制打开照明灯
* OLED实时显示

### 系统优点：
* 自动模式通过图像识别自动打开垃圾桶盖，提高了投放效率和用户体验；而手动模式则是一个备用方案，可以在图像识别故障或其他意外情况下手动操作，以确保系统不至于因单一模式故障而瘫痪。这种双重模式设计有效地增强了系统的健壮性和可靠性，为用户提供了更加稳定、灵活的使用体验。
* 结合光照传感器和人体红外传感器对环境感知，以实现更加智能的能效管理和辅助照明提高图像识别精度。
* 简单友好的OLED用户操作界面。
```
//接线说明
//HC-SR04	STM32
//VCC	        5V
//Trig	        GPIO PB5
//Echo	        GPIO PB4
//Gnd	        GND

//OLED	        STM32
//VCC	        3.3V
//GND	        GND
//SCL	        GPIO PB12
//SDA	        GPIO PB13

//SG90            STM32
//褐色              5V
//红色              GND
//黄色              PA7、PB1、PB10、PB11

//GY30            STM32
//VCC              3.3V
//SCL               PB6
//SDA              PB7  
//GND             GND

//HC-SR501   STM32
//VCC              5V
//OUT              PA0
//GND              GND

//5v灯             STM32
//
//

//K210             STM32
//RX6               TX-PA2
//TX7               RX-PA3

//ConfirmKey_PIN      PA4 
//SelectKey_PIN         PA1
//EscKey_PIN              PA6
```
