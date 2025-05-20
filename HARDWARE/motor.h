#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
/********************配置部分**********************/
#define MOTOR_A 0
#define MOTOR_B 1

/********************函数部分**********************/
void MOTOR_Init(void);
void MOTOR_CountTIM_Init(void);
int MOTOR_GetSpeed(int Motor_Channel);
void MOTOR_ENCODER_Init(void);
void MOTOR_PWM_Init(void);
void MOTOR_SetPWM(uint8_t Motor_Channel, uint16_t PWM);

#endif
