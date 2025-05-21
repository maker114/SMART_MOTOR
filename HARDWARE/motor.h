#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
/********************配置部分**********************/
// 电机编号
#define MOTOR_A 0
#define MOTOR_B 1
// 运行状态编号
#define MOTOR_STOP 0
#define MOTOR_FORWARD 1
#define MOTOR_BACKWARD 2
// 安装方向
#define MOTOR_Clockwise 0                     // 顺时针旋转为正向
#define MOTOR_Anticlockwise 1                 // 逆时针旋转为正向
#define MOTOR_Orientation MOTOR_Anticlockwise // 根据小车安装与前进方向进行设置
// PWM限幅（可调范围-7199~+7199）
#define MOTOR_PWM_MAX 7000         // PWM最大限幅（补偿后）
#define MOTOR_PWM_MIN -7000        // PWM最小限幅（补偿后）
#define MOTOR_PWM_Compensate_A 500 // A通道PWM补偿值
#define MOTOR_PWM_Compensate_B 530 // B通道PWM补偿值
//  通道A引脚
#define Direction_TIM_A RCC_APB2Periph_GPIOA // A通道引脚时钟
#define Direction_Group_A GPIOA              // A通道引脚组
#define Direction_AIN1 GPIO_Pin_3            // A通道方向引脚1
#define Direction_AIN2 GPIO_Pin_4            // A通道方向引脚2
// 通道B引脚
#define Direction_TIM_B RCC_APB2Periph_GPIOA // B通道引脚时钟
#define Direction_Group_B GPIOA              // B通道引脚组
#define Direction_BIN1 GPIO_Pin_5            // B通道方向引脚1
#define Direction_BIN2 GPIO_Pin_6            // B通道方向引脚2

/********************结构体部分**********************/
typedef struct // 电机结构体
{
    int16_t PWMvalue; // PWM装载值
    int16_t Speed;    // 电机转速
} MOTOR_TypeDef;

typedef struct // 快速滤波结构体
{
    float alpha;      // 滤波系数 (0 < alpha < 1)
    float prev_value; // 前一次滤波值
    float buffer[3];  // 移动平均窗口
    uint8_t index;    // 当前缓冲区索引
} FastResponseFilter;

/********************函数部分**********************/
// 电机驱动函数
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
// 辅助函数
void Filter_Init(FastResponseFilter *filter, float alpha, float init_value);
int Filter_Process(FastResponseFilter *filter, float raw_value);

#endif
