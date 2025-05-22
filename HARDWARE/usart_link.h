#ifndef __USARTLINK_H
#define __USARTLINK_H

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

#define LINK_FRAME_HEADER1 0xA1
#define LINK_FRAME_HEADER2 0xB1

typedef enum
{
    LINK_FrameHeader1,
    LINK_FrameHeader2,
    LINK_CMD,
    LINK_DATA_H,
    LINK_DATA_L,
    LINK_CheckSum,
    SUM
} linkpackenum;


void LINK_Init(void);
void LINK_SendPack(uint8_t CMD, uint16_t DATA);

#endif
