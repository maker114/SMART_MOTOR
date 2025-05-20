#ifndef __BOARD_H
#define __BOARD_H

#include "delay.h"
#include "usart.h"
#include "sys.h"

#define LED PCout(13)

#define KEY GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

void Board_Init(void);
int Board_KeyScan(int mode);
void Board_LED_Toggle(void);

#endif
