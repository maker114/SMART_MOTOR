/**
 * @file motor.c
 * @author maker114
 * @brief �����������
 * @version 0.1
 * @date 2025-05-20
 * @note github�ֿ����:[SMART MOTOR](https://github.com/maker114/SMART_MOTOR)
 *
 */
#include "stm32f10x.h" // Device header
#include "motor.h"
#include "board.h"
#include "usart.h"

// ����ṹ��

MOTOR_TypeDef
	MotorStructure_A, ///< Aͨ������ṹ��
	MotorStructure_B; ///< Bͨ������ṹ��
FastResponseFilter
	filter_A, ///< Aͨ���˲���
	filter_B; ///< Bͨ���˲���

/**
 * @brief ��ʱ��1�жϷ�����
 *
 */
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		static int16_t SPEED_A, SPEED_B;
		//  �˲���PID����Ӱ�����أ���ʱ�ر�
		//  SPEED_A = Filter_Process(&filter_A, TIM_GetCounter(TIM2)); // ͨ��A�������˲�
		//  SPEED_B = Filter_Process(&filter_B, TIM_GetCounter(TIM4)); // ͨ��B�������˲�
		SPEED_A = TIM_GetCounter(TIM2);
		SPEED_B = TIM_GetCounter(TIM4);

		// �ٶȼ���
		if (MOTOR_Orientation == MOTOR_Clockwise) // ˳ʱ�밲װ
		{
			MotorStructure_A.Speed = -SPEED_A;
			MotorStructure_B.Speed = SPEED_B; // ��װ���������෴��ȡ��
		}
		else if (MOTOR_Orientation == MOTOR_Anticlockwise) // ��ʱ�밲װ
		{
			MotorStructure_A.Speed = SPEED_A;
			MotorStructure_B.Speed = -SPEED_B; // ��װ���������෴��ȡ��
		}
		// PID����
		if (MOTOR_OperatingMode == MOTOR_AUTO)
		{
			MOTOR_PID_TimLoop();
		}
		TIM_SetCounter(TIM2, 0);					// ���������
		TIM_SetCounter(TIM4, 0);					// ���������
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // ����жϱ�־
	}
}

/**
 * @brief �ܳ�ʼ����������һ�δ˺����������ȫ����ʼ��
 *
 */
void MOTOR_Init(void)
{
	Filter_Init(&filter_A, 0.6f, 0.0f);
	Filter_Init(&filter_B, 0.6f, 0.0f);
	MOTOR_PWM_Init();	   // ��ʼ��PWM
	MOTOR_ENCODER_Init();  // ��ʼ��������
	MOTOR_CountTIM_Init(); // ��ʼ��������ʱ��
	MOTOR_GPIO_Init();	   // ����������ų�ʼ��
	MOTOR_ADC_Init();	   // ��ص�ѹ����ʼ��
	MOTOR_PID_Init();	   // PID��ʼ��
}

/**
 * @brief ��ʼ����ص�ѹ���ADC
 *
 * @note ͨ����ADC1-Channel2
 * @note ���ţ�PA2
 *
 */
void MOTOR_ADC_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);

	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET)
		;
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET)
		;
}

/**
 * @brief ���GPIO��ʼ��
 * @note �й����������޸�λ��motor.h�ļ���
 *
 */
void MOTOR_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// Aͨ������
	RCC_APB2PeriphClockCmd(Direction_TIM_A, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Direction_AIN1 | Direction_AIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // ����ģʽ
	GPIO_Init(Direction_Group_A, &GPIO_InitStructure);
	// Bͨ������
	RCC_APB2PeriphClockCmd(Direction_TIM_B, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Direction_BIN1 | Direction_BIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // ����ģʽ
	GPIO_Init(Direction_Group_B, &GPIO_InitStructure);
}

/**
 * @brief ת�ٶ�ʱ����ʼ��
 *
 */
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

/**
 * @brief ѡ����ǰ������һ�����ڲ�����
 *
 * @param Motor_Channel ���ͨ��
 * @param Motor_State ���״̬
 * - MOTOR_STOPֹͣ(ɲ��)
 * - MOTOR_FORWARDǰ��
 * - MOTOR_BACKWARD����
 * @note
 * - ���з���OTOR_Clockwise˳ʱ�룬MOTOR_Anticlockwise��ʱ��
 * - ������ת������PWMֵ֮��Ĺ�ϵ��MOTOR_Orientation����������С����װ��ǰ���������
 * - �˺���һ����ڲ����ã��Ե�����е�����ѡ��MOTOR_Set_PWM����
 */
void MOTOR_Set_State(int Motor_Channel, int Motor_State)
{
	if (Motor_Channel == MOTOR_A && MOTOR_Orientation == MOTOR_Clockwise) // ͨ��A˳ʱ��
	{
		switch (Motor_State)
		{
		case MOTOR_STOP:
			GPIO_SetBits(Direction_Group_A, Direction_AIN1);
			GPIO_SetBits(Direction_Group_A, Direction_AIN2);
			break;
		case MOTOR_FORWARD:
			GPIO_SetBits(Direction_Group_A, Direction_AIN1);
			GPIO_ResetBits(Direction_Group_A, Direction_AIN2);
			break;
		case MOTOR_BACKWARD:
			GPIO_ResetBits(Direction_Group_A, Direction_AIN1);
			GPIO_SetBits(Direction_Group_A, Direction_AIN2);
			break;
		}
	}
	else if (Motor_Channel == MOTOR_A && MOTOR_Orientation == MOTOR_Anticlockwise) // ͨ��A��ʱ��
	{
		switch (Motor_State)
		{
		case MOTOR_STOP:
			GPIO_SetBits(Direction_Group_A, Direction_AIN1);
			GPIO_SetBits(Direction_Group_A, Direction_AIN2);
			break;
		case MOTOR_FORWARD:
			GPIO_ResetBits(Direction_Group_A, Direction_AIN1);
			GPIO_SetBits(Direction_Group_A, Direction_AIN2);
			break;
		case MOTOR_BACKWARD:
			GPIO_SetBits(Direction_Group_A, Direction_AIN1);
			GPIO_ResetBits(Direction_Group_A, Direction_AIN2);
			break;
		}
	}
	else if (Motor_Channel == MOTOR_B && MOTOR_Orientation == MOTOR_Anticlockwise) // ͨ��B��ʱ��
	{
		switch (Motor_State)
		{
		case MOTOR_STOP:
			GPIO_SetBits(Direction_Group_B, Direction_BIN1);
			GPIO_SetBits(Direction_Group_B, Direction_BIN2);
			break;
		case MOTOR_FORWARD:
			GPIO_SetBits(Direction_Group_B, Direction_BIN1);
			GPIO_ResetBits(Direction_Group_B, Direction_BIN2);
			break;
		case MOTOR_BACKWARD:
			GPIO_ResetBits(Direction_Group_B, Direction_BIN1);
			GPIO_SetBits(Direction_Group_B, Direction_BIN2);
			break;
		}
	}
	else if (Motor_Channel == MOTOR_B && MOTOR_Orientation == MOTOR_Clockwise) // ͨ��B˳ʱ��
	{
		switch (Motor_State)
		{
		case MOTOR_STOP:
			GPIO_SetBits(Direction_Group_B, Direction_BIN1);
			GPIO_SetBits(Direction_Group_B, Direction_BIN2);
			break;
		case MOTOR_FORWARD:
			GPIO_ResetBits(Direction_Group_B, Direction_BIN1);
			GPIO_SetBits(Direction_Group_B, Direction_BIN2);
			break;
		case MOTOR_BACKWARD:
			GPIO_SetBits(Direction_Group_B, Direction_BIN1);
			GPIO_ResetBits(Direction_Group_B, Direction_BIN2);
			break;
		}
	}
}

/**
 * @brief ����С����װ����
 *
 * @param Orientation ��ת����
 * - #define MOTOR_Clockwise 0     // ˳ʱ����תΪ����
 * - #define MOTOR_Anticlockwise 1 // ��ʱ����תΪ����
 */
void MOTOR_Set_Orientation(uint8_t Orientation)
{
	MOTOR_Orientation = Orientation;
}

/**
 * @brief ���ò���ģʽ
 *
 * @param Mode ����ģʽ
 * - #define MOTOR_MANUAL 0 // �ֶ�ģʽ
 * - #define MOTOR_AUTO 1 // �Զ�ģʽ
 */
void MOTOR_Set_OperatingMode(uint8_t Mode)
{
	MOTOR_OperatingMode = Mode;
}

/**
 * @brief �������õ��ת���ٶȣ��Զ�������ת����
 *
 * @param Motor_Channel ���ͨ��
 * @param PWM ��Ӧռ�ձ�ֵ��MOTOR_PWM_MIN ~ MOTOR_PWM_MAX��
 * @note
 * - ���ö�Ӧͨ����PWMֵ�������������ݵ������밲װ����MOTOR_Orientation��������ת����
 * - Ϊ����PWM��������Ƶ���л���PWM=0ʱ����ִ��MOTOR_Set_State(Motor_channel, MOTOR_STOP);
 */
void MOTOR_Set_PWM(uint8_t Motor_channel, int PWM)
{
	if (PWM >= 0) // ��ת
	{
		MOTOR_Set_State(Motor_channel, MOTOR_FORWARD); // ���÷���
		PWM = (Motor_channel == MOTOR_A) ? PWM + MOTOR_PWM_Compensate_A : PWM + MOTOR_PWM_Compensate_B;
		PWM = PWM > MOTOR_PWM_MAX ? MOTOR_PWM_MAX : PWM; // ���ֵ�޷�
	}
	else // ��ת
	{
		MOTOR_Set_State(Motor_channel, MOTOR_BACKWARD); // ���÷���
		PWM = (Motor_channel == MOTOR_A) ? PWM - MOTOR_PWM_Compensate_A : PWM - MOTOR_PWM_Compensate_B;
		PWM = PWM < MOTOR_PWM_MIN ? MOTOR_PWM_MIN : PWM; // ��Сֵ�޷�
		PWM = -PWM;										 // ��תȡ����
	}
	MOTOR_PWM_Load(Motor_channel, PWM); // װ��PWM
}

/**
 * @brief  ����PIDĿ���ٶ�
 *
 * @param Motor_channel ���ͨ��
 * @param Goal_Speed    Ŀ���ٶ�
 */
void MOTOR_Set_PIDGoalSpeed(uint8_t Motor_channel, float Goal_Speed)
{
	if (Motor_channel == MOTOR_A)
	{
		MotorStructure_A.PID_GoalSpeed = Goal_Speed;
	}
	else if (Motor_channel == MOTOR_B)
	{
		MotorStructure_B.PID_GoalSpeed = Goal_Speed;
	}
}

/**
 * @brief ��ȡ���ת��
 *
 * @param Motor_Channel ���ͨ��
 * @return int ���ص��ԭʼת�٣���/����
 */
int MOTOR_Get_Speed(int Motor_Channel)
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
 * @brief ��ȡ���PWMֵ
 *
 * @param Motor_Channel ���ͨ��
 * @return int ���ص��PWMֵ
 * 	- �Զ�ģʽ�·���PID���
 * 	- �ֶ�ģʽ�·��ش���PWMֵ
 */
int MOTOR_Get_PWM(int Motor_Channel)
{

	switch (Motor_Channel)
	{
	case MOTOR_A:
		return MotorStructure_A.PWMvalue; // ���ص��A PWM
	case MOTOR_B:
		return MotorStructure_B.PWMvalue; // ���ص��B PWM
	default:
		break;
	}
	return 0;
}

/**
 * @brief ��ȡ��ص�ѹ
 *
 * @return float ��ص�ѹ����ֵ
 */
float MOTOR_Get_BatteryVoltage(void)
{
	static float voltage = 0;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
		;
	voltage = (float)ADC_GetConversionValue(ADC1) * 3.3f / 4096.0f * 4.08f;
	return voltage;
}

/**
 * @brief ��ȡ���Ŀ���ٶȣ��Զ�ģʽ����Ч��
 *
 * @param Motor_Channel ���ͨ��
 * @return float PIDĿ���ٶ�
 */
float MOTOR_Get_GoalSpeed(uint8_t Motor_Channel)
{
	if (Motor_Channel == MOTOR_A)
	{
		return MotorStructure_A.PID_GoalSpeed;
	}
	else if (Motor_Channel == MOTOR_B)
	{
		return MotorStructure_B.PID_GoalSpeed;
	}
	return 0;
}

/**
 * @brief ��ʼ�����������
 * @note
 *  - ��ʱ����TIM2/TIM4
 *  - ���ţ�PA0/PA1/PB6/PB7
 *  - ͨ��ӳ�䣺
 *  	- MOTOR_A��TIM2
 *  		- PA0->EA-A
 *  		- PA1->EA-B
 *  	- MOTOR_B��TIM4
 * 			- PB6->EB-A
 * 			- PB7->EB-B
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

	// �������ӿ�����z
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
 * @note ��ϸ����
 * 	- Ƶ�ʣ�10KHz
 * 	- ���Ƚ�ֵ��7200
 *  - ͨ����TIM3_CH3��TIM3_CH4
 * 	- ���ţ�PB0��CH3���� PB1��CH4��
 *  - ���ӳ�䣺 PB0->MOTOR_A PB1->MOTOR_B
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
 * @brief װ�ص��PWM
 *
 * @param Motor_Channel ���ͨ����motor_a/motor_b��
 * @param PWM PWMֵ��0~7200��
 * @note
 *  - ���Զ�Ӧͨ���Ķ�ʱ������ռ�ձ�װ�أ������ܸ�������������
 *  - һ����ڲ����ã��Ե�����е�����ѡ��MOTOR_Set_PWM����
 */
void MOTOR_PWM_Load(uint8_t Motor_Channel, uint16_t PWM)
{
	// �޷�
	PWM = PWM > 7200 ? 7200 : PWM;
	// ����PWM
	switch (Motor_Channel)
	{
	case MOTOR_A:
		// printf("A%d  ", PWM);
		MotorStructure_A.PWMvalue = PWM;
		TIM_SetCompare3(TIM3, 7200 - PWM);
		break;
	case MOTOR_B:
		// printf("B%d\r\n", PWM);
		MotorStructure_B.PWMvalue = PWM;
		TIM_SetCompare4(TIM3, 7200 - PWM);
		break;
	default:
		break;
	}
}

/**
 * @brief ��ʼ��PID����
 *
 */
void MOTOR_PID_Init(void)
{
	MotorStructure_A.PID_KP = MOTORA_KP;
	MotorStructure_A.PID_KI = MOTORA_KI;
	MotorStructure_A.PID_KD = MOTORA_KD;
	MotorStructure_B.PID_KP = MOTORB_KP;
	MotorStructure_B.PID_KI = MOTORB_KI;
	MotorStructure_B.PID_KD = MOTORB_KD;
	MotorStructure_A.PID_I_MAX = MOTORA_I_MAX;
	MotorStructure_B.PID_I_MAX = MOTORB_I_MAX;
}

/**
 * @brief PID���㺯��
 *
 * @param MotorStructure  ����ṹ��ָ��
 * @param Goal_Speed      Ŀ���ٶ�
 */
void MOTOR_PID_Calculate(MOTOR_TypeDef *MotorStructure)
{
	// �������
	float Error = MotorStructure->Speed - MotorStructure->PID_GoalSpeed;
	// ������
	float P_out = MotorStructure->PID_KP * Error;
	// ������
	MotorStructure->PID_Integral += Error;
	// �����޷�
	MotorStructure->PID_Integral = (MotorStructure->PID_Integral > MotorStructure->PID_I_MAX) ? MotorStructure->PID_I_MAX : MotorStructure->PID_Integral;
	MotorStructure->PID_Integral = (MotorStructure->PID_Integral < -MotorStructure->PID_I_MAX) ? -MotorStructure->PID_I_MAX : MotorStructure->PID_Integral;
	float I_out = MotorStructure->PID_KI * MotorStructure->PID_Integral;
	// ΢����
	float D_out = MotorStructure->PID_KD * (Error - MotorStructure->PID_LastError);
	// PID���
	MotorStructure->PWMvalue = P_out + I_out - D_out;
}

/**
 * @brief PID��ʱ��ѭ��
 *
 */
void MOTOR_PID_TimLoop(void)
{
	MOTOR_PID_Calculate(&MotorStructure_A);
	MOTOR_PID_Calculate(&MotorStructure_B);
	MOTOR_Set_PWM(MOTOR_A, MotorStructure_A.PWMvalue);
	MOTOR_Set_PWM(MOTOR_B, MotorStructure_B.PWMvalue);
	// printf("%f,%d,%d\r\n",MotorStructure_A.PID_GoalSpeed,MotorStructure_A.PWMvalue,MotorStructure_A.Speed);
}

/**
 * @brief ��ʼ���˲���(�ڲ�����)
 *
 * @param filter �˲���ʵ��ָ��
 * @param alpha �˲�ϵ��(����0.3-0.7)
 * @param init_value ��ʼֵ
 */
void Filter_Init(FastResponseFilter *filter, float alpha, float init_value)
{
	filter->alpha = alpha;
	filter->prev_value = init_value;
	for (int i = 0; i < 3; i++)
	{
		filter->buffer[i] = init_value;
	}
	filter->index = 0;
}

/**
 * @brief ������Ӧ�˲���������(�ڲ�����)
 *
 * @param filter �˲���ʵ��ָ��
 * @param raw_value ԭʼ����ֵ
 * @return �˲����ֵ
 */
int Filter_Process(FastResponseFilter *filter, float raw_value)
{
	// �����ƶ�ƽ��������
	filter->buffer[filter->index] = raw_value;
	filter->index = (filter->index + 1) % 3;
	// �����ƶ�ƽ��
	float moving_avg = (filter->buffer[0] + filter->buffer[1] + filter->buffer[2]) / 3.0f;
	// һ�׵�ͨ�˲�
	float filtered = filter->alpha * moving_avg + (1.0f - filter->alpha) * filter->prev_value;
	filter->prev_value = filtered;
	return filtered;
}
