/**
 * @file WS2812.h
 * @author maker114
 * @brief WS2812驱动头文件
 * @version 0.1
 * @date 2025-06-16
 */
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
#define WS2812_BLACK 0x000000   ///< 黑色
#define WS2812_WHITE 0xFFFFFF   ///< 白色
#define WS2812_RED 0x00FF00     ///< 红色
#define WS2812_GREEN 0xFF0000   ///< 绿色
#define WS2812_BLUE 0x0000FF    ///< 蓝色
#define WS2812_YELLOW 0xFFFF00  ///< 黄色
#define WS2812_PURPLE 0x00FFFF  ///< 紫色
#define WS2812_CYAN 0xFF00FF    ///< 青色
#define WS2812_ICEBLUE 0xFF99FF ///< 雪蓝色

#endif
