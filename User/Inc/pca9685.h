#ifndef __STM32PCA9685_H
#define __STM32PCA9685_H

//#include "stm32f10x.h"
#include "stm32g4xx_hal.h"

#define pca_adrr 0x80

#define pca_mode1 0x0
#define pca_pre 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define jdMIN  115 // minimum
#define jdMAX  590 // maximum
#define jd000  102.4 //0度对应4096的脉宽计数值
#define jd180  512 //180度对应4096的脉宽计算值

extern int bias[24];

void pca_write(uint8_t adrr,uint8_t data,int num);
uint8_t pca_read(uint8_t adrr,int num);
void PCA_SPT_Init(float hz);
void pca_setfreq(float freq);
void pca_setpwm(uint8_t num, uint32_t on, uint32_t off);
void PCA_SPT(uint8_t num,float end_angle);
void SERVO_SPT(uint8_t num,float angle);

#endif









