#ifndef __SOIL_H
#define __SOIL_H
#include "usart.h"
#include "delay.h"
#include "board.h"
#include "stm32f10x.h"  


void AD_Init(void);
uint16_t AD_GetValue(void);

#endif
