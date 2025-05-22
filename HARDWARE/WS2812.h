#ifndef __WS2812_H
#define __WS2812_H

#include "delay.h"
#include "usart.h"
#include "sys.h"

void WS2812_init(uint8_t NUM);

void ws281x_delay(unsigned int delay_num);

void ws281x_sendLow(void);

void ws281x_sendHigh(void);

void ws2811_Reset(void);

void ws281x_sendOne(uint32_t dat);

void WS2812_SendColor(uint8_t NUM, uint8_t R, uint8_t G, uint8_t B);

void WS2812_SendColor_u32(uint8_t NUM, uint32_t Color);

uint32_t WS2812_Wheel(uint32_t pos);

void WS2812_StreamColor(u8 pos);

// 常用颜色GRB编码宏定义，搭配WS2812_SendColor_u32使用
#define WS2812_BLACK 0x000000
#define WS2812_WHITE 0xFFFFFF
#define WS2812_RED 0x00FF00
#define WS2812_GREEN 0xFF0000
#define WS2812_BLUE 0x0000FF
#define WS2812_YELLOW 0xFFFF00
#define WS2812_PURPLE 0x00FFFF
#define WS2812_CYAN 0xFF00FF
#define WS2812_ICEBLUE 0xFF99FF

#endif
