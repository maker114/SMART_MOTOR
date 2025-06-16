/**
 * @file usart_link.c
 * @author maker114
 * @brief  串口通信协议程序
 * @version 0.1
 * @date 2025-05-22
 *
 *
 */
#include "usart_link.h"
#include "WS2812.h"
#include "motor.h"
#include "oled.h"
#include "board.h"
#include "usart.h"

/**
 * @brief 初始化串口连接
 * @note 使用串口三接收
 */
void LINK_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    // PB10 - USART3_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // PB11 - USART3_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // 配置USART3
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

uint8_t packet[SUM]; // 完整数据包: 帧头(2)+CMD(1)+DATA(2)+校验和(1)
/**
 * @brief  发送数据包函数
 * @param  CMD: 命令字节
 * @param  DATA: 16位数据
 */
void LINK_SendPack(uint8_t CMD, uint16_t DATA)
{
    uint8_t checksum = 0;
    packet[LINK_FrameHeader1] = LINK_FRAME_HEADER1;
    packet[LINK_FrameHeader2] = LINK_FRAME_HEADER2;
    packet[LINK_CMD] = CMD;
    packet[LINK_DATA_H] = (uint8_t)(DATA >> 8);   // DATA_H
    packet[LINK_DATA_L] = (uint8_t)(DATA & 0xFF); // DATA_L
    for (int i = 0; i < SUM - 1; i++)             // 计算校验和
        checksum ^= packet[i];
    packet[LINK_CheckSum] = checksum;
    for (int i = 0; i < sizeof(packet); i++)
    {
        USART_SendData(USART3, packet[i]);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
            ;
    }
}

/**
 * @brief 串口中断处理函数
 *
 */
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        Board_LED_Toggle();
        LINK_GetPack();
    }
}

uint8_t GetPack_Buffer[SUM]; // 串口接收缓冲区，接收程序结束后即清零，验证校验和之后存入存储区
uint8_t GetPack_DATA[SUM];   // 串口接收储存区，接收程序结束后不会销毁，可以随时调用
/**
 * @brief 从USART3接收数据包并进行处理。
 * @note 该函数通过USART3接收一个数据包，包括帧头、命令、数据和高低字节，并进行校验。如果校验成功，则处理数据；否则，丢弃数据包。
 */
void LINK_GetPack(void)
{
    static uint8_t Status_count = 0;
    uint8_t checksum = 0;
    switch (Status_count)
    {
    case 0: // 接收帧头 1
        if (USART_ReceiveData(USART3) == LINK_FRAME_HEADER1)
            Status_count = 1;
        else
            Status_count = 0;
        break;

    case 1: // 接收帧头 2
        if (USART_ReceiveData(USART3) == LINK_FRAME_HEADER2)
            Status_count = 2;
        else
            Status_count = 0;
        break;

    case 2: // 接收命令
        GetPack_Buffer[LINK_CMD] = USART_ReceiveData(USART3);
        Status_count = 3;
        break;

    case 3: // 接收数据 1
        GetPack_Buffer[LINK_DATA_H] = USART_ReceiveData(USART3);
        Status_count = 4;
        break;

    case 4: // 接收数据 2
        GetPack_Buffer[LINK_DATA_L] = USART_ReceiveData(USART3);
        Status_count = 5;
        break;

    case 5: // 接收校验和
        GetPack_Buffer[LINK_CheckSum] = USART_ReceiveData(USART3);
        for (int i = 0; i < SUM - 1; i++) // 计算校验和
            checksum ^= packet[i];
        if (checksum == GetPack_Buffer[LINK_CheckSum])
        {
            for (uint8_t i = 0; i < SUM; i++)
            {
                GetPack_DATA[i] = GetPack_Buffer[i]; // 保存数据
            }
            int16_t DATA = (GetPack_DATA[LINK_DATA_H] << 8) | GetPack_DATA[LINK_DATA_L]; // 还原数据
            // printf("CMD: %x, DATA: %x \n", GetPack_DATA[LINK_CMD], DATA);
            LINK_HandleData(GetPack_DATA[LINK_CMD], DATA); // 处理数据
        }
        Status_count = 0; // 清空计数器
        for (uint8_t i = 0; i < SUM; i++)
        {
            GetPack_Buffer[i] = 0; // 清空缓冲区
        }
        break;

    default:
        break;
    }
}

/**
 * @brief 处理接收到的数据命令。
 * @note 根据接收到的命令字节（CMD）和数据（DATA），执行相应的操作。
 * @param CMD 命令字节，指定要执行的操作。
 * @param DATA 数据，与命令相关联的具体数据。
 */
void LINK_HandleData(uint8_t CMD, int16_t DATA)
{
    uint8_t LED_ID = 0;
    uint8_t LED_STATUE = 0;
    switch (CMD)
    {
    case LINK_CMD_IN_NULL: // 0x00
        // Null command (no operation)
        break;

    case LINK_CMD_IN_SET_LED: // 0x01
        // 设置LED，低字节LED编号，高字节LED状态
        LED_STATUE = (uint8_t)(DATA & 0xFF);
        LED_ID = (uint8_t)(DATA >> 8);
        switch (LED_STATUE)
        {
        case 0:
            WS2812_SendColor_u32(LED_ID, WS2812_BLACK);
            break;
        case 1:
            WS2812_SendColor_u32(LED_ID, WS2812_RED);
            break;
        case 2:
            WS2812_SendColor_u32(LED_ID, WS2812_GREEN);
            break;
        case 3:
            WS2812_SendColor_u32(LED_ID, WS2812_BLUE);
            break;
        case 4:
            WS2812_SendColor_u32(LED_ID, WS2812_YELLOW);
            break;
        case 5:
            WS2812_SendColor_u32(LED_ID, WS2812_PURPLE);
            break;
        case 6:
            WS2812_SendColor_u32(LED_ID, WS2812_CYAN);
            break;
        default:
            break;
        }
        break;

    case LINK_CMD_IN_SET_MotorAPWM: // 0x12
        // 设置电机A PWM（仅手动模式有效）
        if (MOTOR_OperatingMode == MOTOR_MANUAL)
            MOTOR_Set_PWM(MOTOR_A, DATA);
        break;

    case LINK_CMD_IN_SET_MotorBPWM: // 0x22
        // 设置电机B PWM（仅手动模式有效）
        if (MOTOR_OperatingMode == MOTOR_MANUAL)
            MOTOR_Set_PWM(MOTOR_B, DATA);
        break;

    case LINK_CMD_IN_SET_MotorAGoalSpeed: // 0x13
        // 设置电机A目标速度（仅自动模式有效）
        if (MOTOR_OperatingMode == MOTOR_AUTO)
            MOTOR_Set_PIDGoalSpeed(MOTOR_A, DATA);
        break;

    case LINK_CMD_IN_SET_MotorBGoalSpeed: // 0x23
        // 设置电机B目标速度（仅自动模式有效）
        if (MOTOR_OperatingMode == MOTOR_AUTO)
            MOTOR_Set_PIDGoalSpeed(MOTOR_B, DATA);
        break;

    case LINK_CMD_IN_SET_MotorMode: // 0x04
        // 设置电机模式，1为手动模式，0为自动模式
        if (DATA == 1)
            MOTOR_Set_OperatingMode(MOTOR_MANUAL);
        else if (DATA == 0)
            MOTOR_Set_OperatingMode(MOTOR_AUTO);
        break;

    case LINK_CMD_IN_SET_Orientation: // 0x05
        // 设置模块安装方向，1为逆时针，0为顺时针
        if (DATA == 0)
            MOTOR_Set_Orientation(MOTOR_Clockwise);
        else if (DATA == 1)
            MOTOR_Set_Orientation(MOTOR_Anticlockwise);
        break;

    case LINK_CMD_IN_SET_SwitchOLED: // 0x06
        // 设置OLED开关，1为开，0为关
        if (DATA == 1)
            OLED_DisPlay_On();
        else if (DATA == 0)
            OLED_DisPlay_Off();
        break;

    case LINK_CMD_IN_SET_MotorStatue: // 0x07
        // 设置电机状态，1为启动，0为停止
        // TODO: 电机启动与停止
        break;

    case LINK_CMD_IN_ASK_ADC: // 0x08
        // 查询电池电量
        LINK_SendPack(LINK_CMD_OUT_RE_ADC, MOTOR_Get_BatteryVoltage() * 100);
        break;

    case LINK_CMD_IN_ASK_MotorASpeed: // 0x09
        // 查询电机A速度
        LINK_SendPack(LINK_CMD_OUT_RE_MotorASpeed, MOTOR_Get_Speed(MOTOR_A));
        break;

    case LINK_CMD_IN_ASK_MotorBSpeed: // 0x0A
        // 查询电机B速度
        LINK_SendPack(LINK_CMD_OUT_RE_MotorBSpeed, MOTOR_Get_Speed(MOTOR_B));
        break;

    default:
        // 未知命令处理
        break;
    }
}
