#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
/********************���ò���**********************/
// ��������޷�
#define MOTORA_I_MAX 4000 // ���A�����޷�
#define MOTORB_I_MAX 4000 // ���B�����޷�
// ���PID
#define MOTORA_KP -150.0f // ���A PID-KPֵ
#define MOTORA_KI -3.5f   // ���A PID-KIֵ
#define MOTORA_KD -10.0f  // ���A PID-KDֵ

#define MOTORB_KP -150.0f // ���B PID-KPֵ
#define MOTORB_KI -3.5f   // ���B PID-KIֵ
#define MOTORB_KD -10.0f  // ���B PID-KDֵ
// ������
#define MOTOR_A 0 // ���A
#define MOTOR_B 1 // ���B
// ����״̬���
#define MOTOR_STOP 0     // ���ֹͣ
#define MOTOR_FORWARD 1  // �������
#define MOTOR_BACKWARD 2 // �������
// ��װ����
#define MOTOR_Clockwise 0     // ˳ʱ����תΪ����
#define MOTOR_Anticlockwise 1 // ��ʱ����תΪ����
// ����ģʽ
#define MOTOR_AUTO 0   // �Զ�ģʽ��ʹ���ڲ�PID��
#define MOTOR_MANUAL 1 // �ֶ�ģʽ��ʹ���ⲿPID��
// PWM�޷����ɵ���Χ-7199~+7199��
#define MOTOR_PWM_MAX 7000         // PWM����޷���������
#define MOTOR_PWM_MIN -7000        // PWM��С�޷���������
#define MOTOR_PWM_Compensate_A 000 // Aͨ��PWM����ֵ
#define MOTOR_PWM_Compensate_B 000 // Bͨ��PWM����ֵ
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

/********************�ɱ����ò���**********************/
// ��װ����
static uint8_t MOTOR_Orientation = MOTOR_Anticlockwise; // ����С����װ��ǰ�������������
// ����ģʽ
static uint8_t MOTOR_OperatingMode = MOTOR_AUTO; // ����ʵ������������ã�Ĭ��Ϊ�Զ�ģʽ
/********************�ṹ�岿��**********************/
typedef struct // ����ṹ��
{
    int16_t PWMvalue;    // PWMװ��ֵ
    int16_t Speed;       // ���ת�٣�����KPKP
    float PID_GoalSpeed; // Ŀ��ת��
    float PID_KP;        // ���PID-KPֵ
    float PID_KI;        // ���PID-KIֵ
    float PID_KD;        // ���PID-KDֵ
    float PID_LastError; // ��һ��ERROR
    float PID_Integral;  // ��ǰERROR
    float PID_I_MAX;     // PID�����޷�
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
void MOTOR_GPIO_Init(void);
void MOTOR_CountTIM_Init(void);
void MOTOR_Set_State(int Motor_Channel, int Motor_State);
void MOTOR_Set_Orientation(uint8_t Orientation);
void MOTOR_Set_OperatingMode(uint8_t Mode);
int MOTOR_Get_Speed(int Motor_Channel);
int MOTOR_Get_PWM(int Motor_Channel);
float MOTOR_Get_BatteryVoltage(void);
void MOTOR_ENCODER_Init(void);
void MOTOR_PWM_Init(void);
void MOTOR_PWM_Load(uint8_t Motor_Channel, uint16_t PWM);
void MOTOR_Set_PWM(uint8_t Motor_channel, int PWM);
void MOTOR_Set_PIDGoalSpeed(uint8_t Motor_channel, float Goal_Speed);
void MOTOR_PID_Init(void);
void MOTOR_PID_Calculate(MOTOR_TypeDef *MotorStructure);
void MOTOR_PID_TimLoop(void);
// ��������
void Filter_Init(FastResponseFilter *filter, float alpha, float init_value);
int Filter_Process(FastResponseFilter *filter, float raw_value);

#endif
