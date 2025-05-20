/**
 * @file motor.c
 * @author maker114
 * @brief 电机驱动函数
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
	int8_t PWMvalue; // PWM装载值
	int8_t Speed;	 // 电机转速
} MOTOR_TypeDef;
MOTOR_TypeDef MotorStructure_A, MotorStructure_B;

void MOTOR_Init(void)
{
	MOTOR_PWM_Init();	   // 初始化PWM
	MOTOR_ENCODER_Init();  // 初始化编码器
	MOTOR_CountTIM_Init(); // 初始化计数定时器
}

void MOTOR_GPIO_Init(void)
{
}

void MOTOR_CountTIM_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// 使能TIM1时钟（TIM1是APB2设备）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	// 定时器基础配置
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;  // 自动重装载值（ARR）
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; // 预分频值（PSC）
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	// 使能TIM1更新中断
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	// 配置NVIC（注意TIM1中断通道不同）
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; // TIM1更新中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// 启动TIM1（高级定时器需要额外使能主输出）
	TIM_CtrlPWMOutputs(TIM1, ENABLE); // 高级定时器特有
	TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		MotorStructure_A.Speed = TIM_GetCounter(TIM2);
		MotorStructure_B.Speed = TIM_GetCounter(TIM4);
		TIM_SetCounter(TIM2, 0);					// 清零计数器
		TIM_SetCounter(TIM4, 0);					// 清零计数器
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // 清除中断标志
	}
}

/**
 * @brief 获取电机转速
 *
 * @param Motor_Channel 电机通道
 * @return int 返回电机原始转速（正/负）
 */
int MOTOR_GetSpeed(int Motor_Channel)
{
	switch (Motor_Channel)
	{
	case MOTOR_A:
		return MotorStructure_A.Speed; // 返回电机A转速
	case MOTOR_B:
		return MotorStructure_B.Speed; // 返回电机B转速
	default:
		break;
	}
	return 0;
}

/**
 * @brief 初始化电机编码器
 * @note -定时器：TIM2/TIM4
 * 		 -引脚：PA0/PA1/PB6/PB7
 *       -通道映射：
 * 			MOTOR_A：TIM2
 * 				|- PA0->EA-A
 * 				|- PA1->EA-B
 * 			MOTOR_B：TIM4
 * 				|- PB6->EB-A
 * 				|- PB7->EB-B
 *
 */
void MOTOR_ENCODER_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	/*--------------------- MOTOR_A--TIM2 编码器配置(PA0/PA1) ---------------------*/
	// 使能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// 配置PA0(TIM2_CH1)和PA1(TIM2_CH2)为上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 定时器基础配置
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65535; // 16位最大值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;  // 不分频
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	// 编码器接口配置
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF; // 强滤波
	TIM_ICInit(TIM2, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);

	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM2, ENABLE);

	/*--------------------- MOTOR_B--TIM4 编码器配置(PB6/PB7) ---------------------*/
	// 使能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// 配置PB6(TIM4_CH1)和PB7(TIM4_CH2)为上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 定时器基础配置(与TIM2相同)
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	// 编码器接口配置
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
 * @brief 初始化电机驱动PWM
 * @note -频率：10KHz
 * 		 -最大比较值：7200
 *       -通道：TIM3_CH3和TIM3_CH4
 * 		 -引脚：PB0（CH3）和 PB1（CH4）
 *       -电机映射： PB0->MOTOR_A PB1->MOTOR_B
 */
void MOTOR_PWM_Init(void)
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

/**
 * @brief 设置电机PWM
 *
 * @param Motor_Channel 电机通道（motor_a/motor_b）
 * @param PWM PWM值（0~7200）
 */
void MOTOR_SetPWM(uint8_t Motor_Channel, uint16_t PWM)
{
	// 限幅
	PWM = PWM > 7200 ? 7200 : PWM;
	// 设置PWM
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
