/**
 * @file motor.c
 * @author maker114
 * @brief �����������
 * @version 0.1
 * @date 2025-05-20
 *
 *
 */
#include "stm32f10x.h" // Device header
#include "motor.h"
#include "board.h"
#include "usart.h"

typedef struct
{
	int8_t PWMvalue; // PWMװ��ֵ
	int8_t Speed;	 // ���ת��
} MOTOR_TypeDef;
MOTOR_TypeDef MotorStructure_A, MotorStructure_B;

void MOTOR_Init(void)
{
	MOTOR_PWM_Init();	   // ��ʼ��PWM
	MOTOR_ENCODER_Init();  // ��ʼ��������
	MOTOR_CountTIM_Init(); // ��ʼ��������ʱ��
}

void MOTOR_GPIO_Init(void)
{
}

void MOTOR_CountTIM_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// ʹ��TIM1ʱ�ӣ�TIM1��APB2�豸��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	// ��ʱ����������
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;  // �Զ���װ��ֵ��ARR��
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; // Ԥ��Ƶֵ��PSC��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	// ʹ��TIM1�����ж�
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	// ����NVIC��ע��TIM1�ж�ͨ����ͬ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; // TIM1�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// ����TIM1���߼���ʱ����Ҫ����ʹ���������
	TIM_CtrlPWMOutputs(TIM1, ENABLE); // �߼���ʱ������
	TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		MotorStructure_A.Speed = TIM_GetCounter(TIM2);
		MotorStructure_B.Speed = TIM_GetCounter(TIM4);
		TIM_SetCounter(TIM2, 0);					// ���������
		TIM_SetCounter(TIM4, 0);					// ���������
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // ����жϱ�־
	}
}

/**
 * @brief ��ȡ���ת��
 *
 * @param Motor_Channel ���ͨ��
 * @return int ���ص��ԭʼת�٣���/����
 */
int MOTOR_GetSpeed(int Motor_Channel)
{
	switch (Motor_Channel)
	{
	case MOTOR_A:
		return MotorStructure_A.Speed; // ���ص��Aת��
	case MOTOR_B:
		return MotorStructure_B.Speed; // ���ص��Bת��
	default:
		break;
	}
	return 0;
}

/**
 * @brief ��ʼ�����������
 * @note -��ʱ����TIM2/TIM4
 * 		 -���ţ�PA0/PA1/PB6/PB7
 *       -ͨ��ӳ�䣺
 * 			MOTOR_A��TIM2
 * 				|- PA0->EA-A
 * 				|- PA1->EA-B
 * 			MOTOR_B��TIM4
 * 				|- PB6->EB-A
 * 				|- PB7->EB-B
 *
 */
void MOTOR_ENCODER_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	/*--------------------- MOTOR_A--TIM2 ����������(PA0/PA1) ---------------------*/
	// ʹ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// ����PA0(TIM2_CH1)��PA1(TIM2_CH2)Ϊ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// ��ʱ����������
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65535; // 16λ���ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;  // ����Ƶ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	// �������ӿ�����
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF; // ǿ�˲�
	TIM_ICInit(TIM2, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);

	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM2, ENABLE);

	/*--------------------- MOTOR_B--TIM4 ����������(PB6/PB7) ---------------------*/
	// ʹ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// ����PB6(TIM4_CH1)��PB7(TIM4_CH2)Ϊ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// ��ʱ����������(��TIM2��ͬ)
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	// �������ӿ�����
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM4, ENABLE);
}

/**
 *
 * @brief ��ʼ���������PWM
 * @note -Ƶ�ʣ�10KHz
 * 		 -���Ƚ�ֵ��7200
 *       -ͨ����TIM3_CH3��TIM3_CH4
 * 		 -���ţ�PB0��CH3���� PB1��CH4��
 *       -���ӳ�䣺 PB0->MOTOR_A PB1->MOTOR_B
 */
void MOTOR_PWM_Init(void)
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

/**
 * @brief ���õ��PWM
 *
 * @param Motor_Channel ���ͨ����motor_a/motor_b��
 * @param PWM PWMֵ��0~7200��
 */
void MOTOR_SetPWM(uint8_t Motor_Channel, uint16_t PWM)
{
	// �޷�
	PWM = PWM > 7200 ? 7200 : PWM;
	// ����PWM
	switch (Motor_Channel)
	{
	case MOTOR_A:
		TIM_SetCompare3(TIM3, PWM);
		break;
	case MOTOR_B:
		TIM_SetCompare4(TIM3, PWM);
		break;
	default:
		break;
	}
}
