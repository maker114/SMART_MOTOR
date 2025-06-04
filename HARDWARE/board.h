#ifndef __BOARD_H
#define __BOARD_H

#include "delay.h"
#include "usart.h"
#include "sys.h"

#define LED PCout(13)
#define KEY1 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)
#define KEY2 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)
#define KEY3 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)

void Board_Init(void);
int Board_KeyScan(void);
void Board_LED_Toggle(void);
void Board_BEEP_ActiveMs(int ms);
void Board_BEEP_ContinuousActive(int time);

#endif
