#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
/********************���ò���**********************/
// ������
#define MOTOR_A 0
#define MOTOR_B 1
// ����״̬���
#define MOTOR_STOP 0
#define MOTOR_FORWARD 1
#define MOTOR_BACKWARD 2
// ��װ����
#define MOTOR_Clockwise 0                     // ˳ʱ����תΪ����
#define MOTOR_Anticlockwise 1                 // ��ʱ����תΪ����
#define MOTOR_Orientation MOTOR_Anticlockwise // ����С����װ��ǰ�������������
// PWM�޷����ɵ���Χ-7199~+7199��
#define MOTOR_PWM_MAX 7000         // PWM����޷���������
#define MOTOR_PWM_MIN -7000        // PWM��С�޷���������
#define MOTOR_PWM_Compensate_A 500 // Aͨ��PWM����ֵ
#define MOTOR_PWM_Compensate_B 530 // Bͨ��PWM����ֵ
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

/********************�ṹ�岿��**********************/
typedef struct // ����ṹ��
{
    int16_t PWMvalue; // PWMװ��ֵ
    int16_t Speed;    // ���ת��
} MOTOR_TypeDef;

typedef struct // �����˲��ṹ��
{
    float alpha;      // �˲�ϵ�� (0 < alpha < 1)
    float prev_value; // ǰһ���˲�ֵ
    float buffer[3];  // �ƶ�ƽ������
    uint8_t index;    // ��ǰ����������
} FastResponseFilter;

/********************��������**********************/
// �����������
void MOTOR_Init(void);
void MOTOR_ADC_Init(void);
float MOTOR_GetBatteryVoltage(void);
void MOTOR_GPIO_Init(void);
void MOTOR_CountTIM_Init(void);
int MOTOR_GetSpeed(int Motor_Channel);
void MOTOR_ENCODER_Init(void);
void MOTOR_PWM_Init(void);
void MOTOR_LoadPWM(uint8_t Motor_Channel, uint16_t PWM);
void MOTOR_SetPWM(uint8_t Motor_Channel, int PWM);
// ��������
void Filter_Init(FastResponseFilter *filter, float alpha, float init_value);
int Filter_Process(FastResponseFilter *filter, float raw_value);

#endif
