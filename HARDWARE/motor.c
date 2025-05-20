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

/********************���ò���**********************/

void MOTOR_Init(void)
{
	PWM_Init(); // ��ʼ��PWM
}




/**
 * @brief ��ʼ���������PWM
 * @note -Ƶ�ʣ�10KHz
 * 		 -���Ƚ�ֵ��7200
 *       -ͨ����TIM3_CH3��TIM3_CH4
 * 		 -���ţ�PB0��CH3���� PB1��CH4��
 *       -���ӳ�䣺 PB0->MOTOR_A PB1->MOTOR_B
 */
void PWM_Init(void)
{
	// ��ʼ���ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	// ����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);						// TIM3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // GPIOB��AFIOʱ��

	// ������ӳ��
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // TIM3������ӳ��

	// ����PB0��PB1ΪPWM���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // PB0(TIM3_CH3)��PB1(TIM3_CH4)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   // ����ģʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// ��ʱ����������
	TIM_TimeBaseStructure.TIM_Period = 7200 - 1;				// PWM����
	TIM_TimeBaseStructure.TIM_Prescaler = 0;					// ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				// ��ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	// PWM�������
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  // PWMģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // ʹ�����
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  // �ߵ�ƽ��Ч

	// ��ʼ��ͨ��3��4
	TIM_OC3Init(TIM3, &TIM_OCInitStructure); // PB0
	TIM_OC4Init(TIM3, &TIM_OCInitStructure); // PB1

	// ʹ��Ԥװ��
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// ������ʱ��
	TIM_Cmd(TIM3, ENABLE);
}