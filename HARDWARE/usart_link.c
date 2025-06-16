/**
 * @file usart_link.c
 * @author maker114
 * @brief  ����ͨ��Э�����
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
 * @brief ��ʼ����������
 * @note ʹ�ô���������
 */
void LINK_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    // ʹ��ʱ��
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
    // ����USART3
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

uint8_t packet[SUM]; // �������ݰ�: ֡ͷ(2)+CMD(1)+DATA(2)+У���(1)
/**
 * @brief  �������ݰ�����
 * @param  CMD: �����ֽ�
 * @param  DATA: 16λ����
 */
void LINK_SendPack(uint8_t CMD, uint16_t DATA)
{
    uint8_t checksum = 0;
    packet[LINK_FrameHeader1] = LINK_FRAME_HEADER1;
    packet[LINK_FrameHeader2] = LINK_FRAME_HEADER2;
    packet[LINK_CMD] = CMD;
    packet[LINK_DATA_H] = (uint8_t)(DATA >> 8);   // DATA_H
    packet[LINK_DATA_L] = (uint8_t)(DATA & 0xFF); // DATA_L
    for (int i = 0; i < SUM - 1; i++)             // ����У���
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
 * @brief �����жϴ�����
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

uint8_t GetPack_Buffer[SUM]; // ���ڽ��ջ����������ճ�����������㣬��֤У���֮�����洢��
uint8_t GetPack_DATA[SUM];   // ���ڽ��մ����������ճ�������󲻻����٣�������ʱ����
/**
 * @brief ��USART3�������ݰ������д���
 * @note �ú���ͨ��USART3����һ�����ݰ�������֡ͷ��������ݺ͸ߵ��ֽڣ�������У�顣���У��ɹ����������ݣ����򣬶������ݰ���
 */
void LINK_GetPack(void)
{
    static uint8_t Status_count = 0;
    uint8_t checksum = 0;
    switch (Status_count)
    {
    case 0: // ����֡ͷ 1
        if (USART_ReceiveData(USART3) == LINK_FRAME_HEADER1)
            Status_count = 1;
        else
            Status_count = 0;
        break;

    case 1: // ����֡ͷ 2
        if (USART_ReceiveData(USART3) == LINK_FRAME_HEADER2)
            Status_count = 2;
        else
            Status_count = 0;
        break;

    case 2: // ��������
        GetPack_Buffer[LINK_CMD] = USART_ReceiveData(USART3);
        Status_count = 3;
        break;

    case 3: // �������� 1
        GetPack_Buffer[LINK_DATA_H] = USART_ReceiveData(USART3);
        Status_count = 4;
        break;

    case 4: // �������� 2
        GetPack_Buffer[LINK_DATA_L] = USART_ReceiveData(USART3);
        Status_count = 5;
        break;

    case 5: // ����У���
        GetPack_Buffer[LINK_CheckSum] = USART_ReceiveData(USART3);
        for (int i = 0; i < SUM - 1; i++) // ����У���
            checksum ^= packet[i];
        if (checksum == GetPack_Buffer[LINK_CheckSum])
        {
            for (uint8_t i = 0; i < SUM; i++)
            {
                GetPack_DATA[i] = GetPack_Buffer[i]; // ��������
            }
            int16_t DATA = (GetPack_DATA[LINK_DATA_H] << 8) | GetPack_DATA[LINK_DATA_L]; // ��ԭ����
            // printf("CMD: %x, DATA: %x \n", GetPack_DATA[LINK_CMD], DATA);
            LINK_HandleData(GetPack_DATA[LINK_CMD], DATA); // ��������
        }
        Status_count = 0; // ��ռ�����
        for (uint8_t i = 0; i < SUM; i++)
        {
            GetPack_Buffer[i] = 0; // ��ջ�����
        }
        break;

    default:
        break;
    }
}

/**
 * @brief ������յ����������
 * @note ���ݽ��յ��������ֽڣ�CMD�������ݣ�DATA����ִ����Ӧ�Ĳ�����
 * @param CMD �����ֽڣ�ָ��Ҫִ�еĲ�����
 * @param DATA ���ݣ�������������ľ������ݡ�
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
        // ����LED�����ֽ�LED��ţ����ֽ�LED״̬
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
        // ���õ��A PWM�����ֶ�ģʽ��Ч��
        if (MOTOR_OperatingMode == MOTOR_MANUAL)
            MOTOR_Set_PWM(MOTOR_A, DATA);
        break;

    case LINK_CMD_IN_SET_MotorBPWM: // 0x22
        // ���õ��B PWM�����ֶ�ģʽ��Ч��
        if (MOTOR_OperatingMode == MOTOR_MANUAL)
            MOTOR_Set_PWM(MOTOR_B, DATA);
        break;

    case LINK_CMD_IN_SET_MotorAGoalSpeed: // 0x13
        // ���õ��AĿ���ٶȣ����Զ�ģʽ��Ч��
        if (MOTOR_OperatingMode == MOTOR_AUTO)
            MOTOR_Set_PIDGoalSpeed(MOTOR_A, DATA);
        break;

    case LINK_CMD_IN_SET_MotorBGoalSpeed: // 0x23
        // ���õ��BĿ���ٶȣ����Զ�ģʽ��Ч��
        if (MOTOR_OperatingMode == MOTOR_AUTO)
            MOTOR_Set_PIDGoalSpeed(MOTOR_B, DATA);
        break;

    case LINK_CMD_IN_SET_MotorMode: // 0x04
        // ���õ��ģʽ��1Ϊ�ֶ�ģʽ��0Ϊ�Զ�ģʽ
        if (DATA == 1)
            MOTOR_Set_OperatingMode(MOTOR_MANUAL);
        else if (DATA == 0)
            MOTOR_Set_OperatingMode(MOTOR_AUTO);
        break;

    case LINK_CMD_IN_SET_Orientation: // 0x05
        // ����ģ�鰲װ����1Ϊ��ʱ�룬0Ϊ˳ʱ��
        if (DATA == 0)
            MOTOR_Set_Orientation(MOTOR_Clockwise);
        else if (DATA == 1)
            MOTOR_Set_Orientation(MOTOR_Anticlockwise);
        break;

    case LINK_CMD_IN_SET_SwitchOLED: // 0x06
        // ����OLED���أ�1Ϊ����0Ϊ��
        if (DATA == 1)
            OLED_DisPlay_On();
        else if (DATA == 0)
            OLED_DisPlay_Off();
        break;

    case LINK_CMD_IN_SET_MotorStatue: // 0x07
        // ���õ��״̬��1Ϊ������0Ϊֹͣ
        // TODO: ���������ֹͣ
        break;

    case LINK_CMD_IN_ASK_ADC: // 0x08
        // ��ѯ��ص���
        LINK_SendPack(LINK_CMD_OUT_RE_ADC, MOTOR_Get_BatteryVoltage() * 100);
        break;

    case LINK_CMD_IN_ASK_MotorASpeed: // 0x09
        // ��ѯ���A�ٶ�
        LINK_SendPack(LINK_CMD_OUT_RE_MotorASpeed, MOTOR_Get_Speed(MOTOR_A));
        break;

    case LINK_CMD_IN_ASK_MotorBSpeed: // 0x0A
        // ��ѯ���B�ٶ�
        LINK_SendPack(LINK_CMD_OUT_RE_MotorBSpeed, MOTOR_Get_Speed(MOTOR_B));
        break;

    default:
        // δ֪�����
        break;
    }
}
