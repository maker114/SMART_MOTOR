#ifndef __esp_H
#define __esp_H
#include "usart.h"
#include "delay.h"
#include "board.h"

void USART_SendPage(uint16_t DATA);
void USART_ResetID(void);
u8 USART_GetID(void);

#endif
