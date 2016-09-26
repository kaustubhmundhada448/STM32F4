#ifndef _STM32F4xx_MPU6050_H
#define _STM32F4xx_MPU6050_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"


int Offset[3],Offset_accel[3];

void Init_i2c(void);
void Init_led(void);
void Init_usart(void);
void Init_tim(void);
void Init_mpu(void);
uint8_t Rec_data (uint8_t reg);
void Calibrate_Gyros(void);
void Calibrate_Accel(void);
void Send_data_usart(int data);
void Init_Gyroscope(void);
void Get_Data(float* Omega,float *Accel);
#ifdef __cplusplus
}
#endif

#endif
