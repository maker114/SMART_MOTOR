#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
/********************���ò���**********************/
// ������
#define MOTOR_A 0
#define MOTOR_B 1
// ����״̬���
#define MOTOR_STOP 0
#define MOTOR_FORWARD 1
#define MOTOR_BACKWARD 2
// ��װ����
#define MOTOR_Clockwise 0                 // ˳ʱ����תΪ����
#define MOTOR_Anticlockwise 1             // ��ʱ����תΪ����
#define MOTOR_Orientation MOTOR_Clockwise // ����С����װ��ǰ�������������
// PWM�޷����ɵ���Χ-7199~+7199��
#define MOTOR_PWM_MAX 7100       // PWM����޷���������
#define MOTOR_PWM_MIN -7100      // PWM��С�޷���������
#define MOTOR_PWM_Compensate_A 0 // Aͨ��PWM����ֵ
#define MOTOR_PWM_Compensate_B 0 // Bͨ��PWM����ֵ
//  ͨ��A����
#define Direction_TIM_A RCC_APB2Periph_GPIOA // Aͨ������ʱ��
#define Direction_Group_A GPIOA              // Aͨ��������
#define Direction_AIN1 GPIO_Pin_3            // Aͨ����������1
#define Direction_AIN2 GPIO_Pin_4            // Aͨ����������2
// ͨ��B����
#define Direction_TIM_B RCC_APB2Periph_GPIOA // Bͨ������ʱ��
#define Direction_Group_B GPIOA              // Bͨ��������
#define Direction_BIN1 GPIO_Pin_5            // Bͨ����������1
#define Direction_BIN2 GPIO_Pin_6            // Bͨ����������2

/********************��������**********************/
void MOTOR_Init(void);
void MOTOR_GPIO_Init(void);
void MOTOR_CountTIM_Init(void);
int MOTOR_GetSpeed(int Motor_Channel);
void MOTOR_ENCODER_Init(void);
void MOTOR_PWM_Init(void);
void MOTOR_LoadPWM(uint8_t Motor_Channel, uint16_t PWM);
void MOTOR_SetPWM(uint8_t Motor_Channel, int PWM);

#endif
