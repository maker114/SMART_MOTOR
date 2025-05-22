/**
 * @file usart_link.c
 * @author maker114
 * @brief 串口连接驱动程序
 * @version 0.1
 * @date 2025-05-22
 *
 *
 */
#include "usart_link.h"

void LINK_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
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
    USART_Cmd(USART3, ENABLE);
}

/**
 * @brief  发送数据包函数
 * @param  CMD: 命令字节
 * @param  DATA: 16位数据
 * @retval None
 */
uint8_t packet[SUM]; // 完整数据包: 帧头(2)+CMD(1)+DATA(2)+校验和(1)
void LINK_SendPack(uint8_t CMD, uint16_t DATA)
{
    uint8_t checksum = 0;
    packet[LINK_FrameHeader1] = LINK_FRAME_HEADER1;
    packet[LINK_FrameHeader2] = LINK_FRAME_HEADER2;
    packet[LINK_CMD] = CMD;
    packet[LINK_DATA_H] = (uint8_t)(DATA >> 8);   // DATA_H
    packet[LINK_DATA_L] = (uint8_t)(DATA & 0xFF); // DATA_L
    for (int i = 0; i < 5; i++)                   // 计算校验和
        checksum ^= packet[i];
    packet[LINK_CheckSum] = checksum;
    for (int i = 0; i < sizeof(packet); i++)
    {
        USART_SendData(USART3, packet[i]);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
            ;
    }
}
