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

// 定义结构体
MOTOR_TypeDef MotorStructure_A, MotorStructure_B;
FastResponseFilter filter_A, filter_B;

/**
 * @brief 定时器1中断服务函数
 *
 */
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		static int16_t SPEED_A, SPEED_B;
		//  滤波对PID计算影响严重，暂时关闭
		//  SPEED_A = Filter_Process(&filter_A, TIM_GetCounter(TIM2)); // 通道A编码器滤波
		//  SPEED_B = Filter_Process(&filter_B, TIM_GetCounter(TIM4)); // 通道B编码器滤波
		SPEED_A = TIM_GetCounter(TIM2);
		SPEED_B = TIM_GetCounter(TIM4);

		// 速度计算
		if (MOTOR_Orientation == MOTOR_Clockwise) // 顺时针安装
		{
			MotorStructure_A.Speed = -SPEED_A;
			MotorStructure_B.Speed = SPEED_B; // 安装方向手性相反，取负
		}
		else if (MOTOR_Orientation == MOTOR_Anticlockwise) // 逆时针安装
		{
			MotorStructure_A.Speed = SPEED_A;
			MotorStructure_B.Speed = -SPEED_B; // 安装方向手性相反，取负
		}
		// PID计算
		if (MOTOR_OperatingMode == MOTOR_AUTO)
		{
			MOTOR_PID_TimLoop();
		}
		TIM_SetCounter(TIM2, 0);					// 清零计数器
		TIM_SetCounter(TIM4, 0);					// 清零计数器
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // 清除中断标志
	}
}

/**
 * @brief 总初始化，仅调用一次此函数即可完成全部初始化
 *
 */
void MOTOR_Init(void)
{
	Filter_Init(&filter_A, 0.6f, 0.0f);
	Filter_Init(&filter_B, 0.6f, 0.0f);
	MOTOR_PWM_Init();	   // 初始化PWM
	MOTOR_ENCODER_Init();  // 初始化编码器
	MOTOR_CountTIM_Init(); // 初始化计数定时器
	MOTOR_GPIO_Init();	   // 方向控制引脚初始化
	MOTOR_ADC_Init();	   // 电池电压检测初始化
	MOTOR_PID_Init();	   // PID初始化
}

/**
 * @brief 初始化电池电压检测ADC
 *
 * @note -通道：ADC1-Channel2
 * 		 -引脚：PA2
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
 * @brief 电机GPIO初始化
 * @note 有关引脚配置修改位于motor.h文件中
 *
 */
void MOTOR_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// A通道配置
	RCC_APB2PeriphClockCmd(Direction_TIM_A, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Direction_AIN1 | Direction_AIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 高速模式
	GPIO_Init(Direction_Group_A, &GPIO_InitStructure);
	// B通道配置
	RCC_APB2PeriphClockCmd(Direction_TIM_B, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Direction_BIN1 | Direction_BIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 高速模式
	GPIO_Init(Direction_Group_B, &GPIO_InitStructure);
}

/**
 * @brief 转速定时器初始化
 *
 */
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

/**
 * @brief 选择电机前进方向，一般由内部调用
 *
 * @param Motor_Channel 电机通道
 * @param Motor_State 电机状态
 * @note -MOTOR_Clockwise顺时针，MOTOR_Anticlockwise逆时针
 *       -MOTOR_STOP停止(刹车)，MOTOR_FORWARD前进，MOTOR_BACKWARD后退
 *       -轮子旋转方向与PWM值之间的关系由MOTOR_Orientation决定，即由小车安装与前进方向决定
 * 		 -一般仅内部调用，对电机进行调速需选择MOTOR_Set_PWM函数
 */
void MOTOR_Set_State(int Motor_Channel, int Motor_State)
{
	if (Motor_Channel == MOTOR_A && MOTOR_Orientation == MOTOR_Clockwise) // 通道A顺时针
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
	else if (Motor_Channel == MOTOR_A && MOTOR_Orientation == MOTOR_Anticlockwise) // 通道A逆时针
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
	else if (Motor_Channel == MOTOR_B && MOTOR_Orientation == MOTOR_Anticlockwise) // 通道B逆时针
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
	else if (Motor_Channel == MOTOR_B && MOTOR_Orientation == MOTOR_Clockwise) // 通道B顺时针
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
 * @brief 设置小车安装方向
 *
 * @param Orientation
 * 		|- #define MOTOR_Clockwise 0     // 顺时针旋转为正向
 * 		|- #define MOTOR_Anticlockwise 1 // 逆时针旋转为正向
 */
void MOTOR_Set_Orientation(uint8_t Orientation)
{
	MOTOR_Orientation = Orientation;
}

/**
 * @brief 设置操作模式
 *
 * @param Mode
 * 		|- #define MOTOR_MANUAL 0 // 手动模式
 * 		|- #define MOTOR_AUTO 1 // 自动模式
 */
void MOTOR_Set_OperatingMode(uint8_t Mode)
{
	MOTOR_OperatingMode = Mode;
}

/**
 * @brief 用来设置电机转动速度，自动设置旋转方向
 *
 * @param Motor_Channel 电机通道
 * @param PWM 对应占空比值（MOTOR_PWM_MIN ~ MOTOR_PWM_MAX）
 * @note -设置对应通道的PWM值，根据输入数据的正负与安装方向（MOTOR_Orientation）决定旋转方向
 *       -为避免PWM过零引脚频繁切换，PWM=0时不会执行MOTOR_Set_State(Motor_channel, MOTOR_STOP);
 */
void MOTOR_Set_PWM(uint8_t Motor_channel, int PWM)
{
	if (PWM >= 0) // 正转
	{
		MOTOR_Set_State(Motor_channel, MOTOR_FORWARD); // 设置方向
		PWM = (Motor_channel == MOTOR_A) ? PWM + MOTOR_PWM_Compensate_A : PWM + MOTOR_PWM_Compensate_B;
		PWM = PWM > MOTOR_PWM_MAX ? MOTOR_PWM_MAX : PWM; // 最大值限幅
	}
	else // 反转
	{
		MOTOR_Set_State(Motor_channel, MOTOR_BACKWARD); // 设置方向
		PWM = (Motor_channel == MOTOR_A) ? PWM - MOTOR_PWM_Compensate_A : PWM - MOTOR_PWM_Compensate_B;
		PWM = PWM < MOTOR_PWM_MIN ? MOTOR_PWM_MIN : PWM; // 最小值限幅
		PWM = -PWM;										 // 反转取正数
	}
	MOTOR_PWM_Load(Motor_channel, PWM); // 装载PWM
}

/**
 * @brief  设置PID目标速度
 *
 * @param Motor_channel 电机通道
 * @param Goal_Speed    目标速度
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
 * @brief 获取电机转速
 *
 * @param Motor_Channel 电机通道
 * @return int 返回电机原始转速（正/负）
 */
int MOTOR_Get_Speed(int Motor_Channel)
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
 * @brief 获取电机PWM值
 *
 * @param Motor_Channel 电机通道
 * @return int
 * 		|- 自动模式下返回PID结果
 * 		|- 手动模式下返回传入PWM值
 */
int MOTOR_Get_PWM(int Motor_Channel)
{

	switch (Motor_Channel)
	{
	case MOTOR_A:
		return MotorStructure_A.PWMvalue; // 返回电机A PWM
	case MOTOR_B:
		return MotorStructure_B.PWMvalue; // 返回电机B PWM
	default:
		break;
	}
	return 0;
}

/**
 * @brief 获取电池电压
 *
 * @return float 电池电压返回值
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

	// 编码器接口配置z
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
 * @brief 装载电机PWM
 *
 * @param Motor_Channel 电机通道（motor_a/motor_b）
 * @param PWM PWM值（0~7200）
 * @note -仅对对应通道的定时器进行占空比装载，不接受负数，不处理方向
 *       -一般仅内部调用，对电机进行调速需选择MOTOR_Set_PWM函数
 */
void MOTOR_PWM_Load(uint8_t Motor_Channel, uint16_t PWM)
{
	// 限幅
	PWM = PWM > 7200 ? 7200 : PWM;
	// 设置PWM
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
 * @brief 初始化PID参数
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
 * @brief PID计算函数
 *
 * @param MotorStructure  电机结构体指针
 * @param Goal_Speed      目标速度
 */
void MOTOR_PID_Calculate(MOTOR_TypeDef *MotorStructure)
{
	// 计算误差
	float Error = MotorStructure->Speed - MotorStructure->PID_GoalSpeed;
	// 比例项
	float P_out = MotorStructure->PID_KP * Error;
	// 积分项
	MotorStructure->PID_Integral += Error;
	// 积分限幅
	MotorStructure->PID_Integral = (MotorStructure->PID_Integral > MotorStructure->PID_I_MAX) ? MotorStructure->PID_I_MAX : MotorStructure->PID_Integral;
	MotorStructure->PID_Integral = (MotorStructure->PID_Integral < -MotorStructure->PID_I_MAX) ? -MotorStructure->PID_I_MAX : MotorStructure->PID_Integral;
	float I_out = MotorStructure->PID_KI * MotorStructure->PID_Integral;
	// 微分项
	float D_out = MotorStructure->PID_KD * (Error - MotorStructure->PID_LastError);
	// PID输出
	MotorStructure->PWMvalue = P_out + I_out - D_out;
}

/**
 * @brief PID定时器循环
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
 * @brief 初始化滤波器(内部调用)
 *
 * @param filter 滤波器实例指针
 * @param alpha 滤波系数(建议0.3-0.7)
 * @param init_value 初始值
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
 * @brief 快速响应滤波器处理函数(内部调用)
 *
 * @param filter 滤波器实例指针
 * @param raw_value 原始输入值
 * @return 滤波后的值
 */
int Filter_Process(FastResponseFilter *filter, float raw_value)
{
	// 更新移动平均缓冲区
	filter->buffer[filter->index] = raw_value;
	filter->index = (filter->index + 1) % 3;
	// 计算移动平均
	float moving_avg = (filter->buffer[0] + filter->buffer[1] + filter->buffer[2]) / 3.0f;
	// 一阶低通滤波
	float filtered = filter->alpha * moving_avg + (1.0f - filter->alpha) * filter->prev_value;
	filter->prev_value = filtered;
	return filtered;
}
