#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
/********************配置部分**********************/
// 电机积分限幅
#define MOTORA_I_MAX 4000 // 电机A积分限幅
#define MOTORB_I_MAX 4000 // 电机B积分限幅
// 电机PID
#define MOTORA_KP -150.0f // 电机A PID-KP值
#define MOTORA_KI -3.5f   // 电机A PID-KI值
#define MOTORA_KD -10.0f  // 电机A PID-KD值

#define MOTORB_KP -150.0f // 电机B PID-KP值
#define MOTORB_KI -3.5f   // 电机B PID-KI值
#define MOTORB_KD -10.0f  // 电机B PID-KD值
// 电机编号
#define MOTOR_A 0 // 电机A
#define MOTOR_B 1 // 电机B
// 运行状态编号
#define MOTOR_STOP 0     // 电机停止
#define MOTOR_FORWARD 1  // 电机正向
#define MOTOR_BACKWARD 2 // 电机反向
// 安装方向
#define MOTOR_Clockwise 0     // 顺时针旋转为正向
#define MOTOR_Anticlockwise 1 // 逆时针旋转为正向
// 操作模式
#define MOTOR_AUTO 0   // 自动模式（使用内部PID）
#define MOTOR_MANUAL 1 // 手动模式（使用外部PID）
// PWM限幅（可调范围-7199~+7199）
#define MOTOR_PWM_MAX 7000         // PWM最大限幅（补偿后）
#define MOTOR_PWM_MIN -7000        // PWM最小限幅（补偿后）
#define MOTOR_PWM_Compensate_A 000 // A通道PWM补偿值
#define MOTOR_PWM_Compensate_B 000 // B通道PWM补偿值
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

/********************可变设置部分**********************/
// 安装方向
static uint8_t MOTOR_Orientation = MOTOR_Anticlockwise; // 根据小车安装与前进方向进行设置
// 操作模式
static uint8_t MOTOR_OperatingMode = MOTOR_AUTO; // 根据实际需求进行设置，默认为自动模式
/********************结构体部分**********************/
typedef struct // 电机结构体
{
    int16_t PWMvalue;    // PWM装载值
    int16_t Speed;       // 电机转速（编码KPKP
    float PID_GoalSpeed; // 目标转速
    float PID_KP;        // 电机PID-KP值
    float PID_KI;        // 电机PID-KI值
    float PID_KD;        // 电机PID-KD值
    float PID_LastError; // 上一次ERROR
    float PID_Integral;  // 当前ERROR
    float PID_I_MAX;     // PID积分限幅
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
// 辅助函数
void Filter_Init(FastResponseFilter *filter, float alpha, float init_value);
int Filter_Process(FastResponseFilter *filter, float raw_value);

#endif
