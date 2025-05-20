/**
 * @file motor.c
 * @author maker114
 * @brief ??????
 * @version 0.1
 * @date 2025-05-20
 *
 *
 */
#include "stm32f10x.h" // Device header
#include "motor.h"

/********************配置部分**********************/

void MOTOR_Init(void)
{
	PWM_Init(); // 初始化PWM
}




/**
 * @brief 初始化电机驱动PWM
 * @note -频率：10KHz
 * 		 -最大比较值：7200
 *       -通道：TIM3_CH3和TIM3_CH4
 * 		 -引脚：PB0（CH3）和 PB1（CH4）
 *       -电机映射： PB0->MOTOR_A PB1->MOTOR_B
 */
void PWM_Init(void)
{
	// 初始化结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	// 开启时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);						// TIM3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // GPIOB和AFIO时钟

	// 引脚重映射
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // TIM3部分重映射

	// 配置PB0和PB1为PWM输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // PB0(TIM3_CH3)和PB1(TIM3_CH4)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   // 高速模式
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 定时器基本配置
	TIM_TimeBaseStructure.TIM_Period = 7200 - 1;				// PWM周期
	TIM_TimeBaseStructure.TIM_Prescaler = 0;					// 不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				// 无时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	// PWM输出配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  // PWM模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  // 高电平有效

	// 初始化通道3和4
	TIM_OC3Init(TIM3, &TIM_OCInitStructure); // PB0
	TIM_OC4Init(TIM3, &TIM_OCInitStructure); // PB1

	// 使能预装载
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// 启动定时器
	TIM_Cmd(TIM3, ENABLE);
}