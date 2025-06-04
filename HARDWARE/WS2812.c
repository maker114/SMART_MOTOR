#include "WS2812.h"

// 定义部分
int LED_NUM = 0;                // LED灯数量
uint32_t LED_BUFFER[100] = {0}; // LED缓冲区,所存储的数据为24位的RGB数据

/**
 * @brief 初始化WS2812引脚并定义灯的数量
 *
 * @param NUM LED灯数量
 */
void WS2812_init(uint8_t NUM)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能PA端口时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;             // WS2811-->PA.0 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);                // 根据设定参数初始化GPIOA.0
  GPIO_ResetBits(GPIOA, GPIO_Pin_8);                    // PA.0 输出低电平
  LED_NUM = NUM; // 初始化LED数量
	WS2812_SendColor(0,0x00,0x00,0x00);
	WS2812_SendColor(1,0x00,0x00,0x00);
	WS2812_SendColor(2,0x00,0x00,0x00);
	WS2812_SendColor(3,0x00,0x00,0x00);
}

/**
 * @brief  ws281x模块用到的延时函数
 * @param  delay_num :延时数 （示波器测量延时时间 = delay_num * 440ns ）
 * @retval None
 */
void ws281x_delay(unsigned int delay_num)
{
  while (delay_num--)
    ;
}

/**
 * @brief  根据WS281x芯片时序图编写的发送0码，1码与RESET码的函数
 * @param
 * @retval None
 */
void ws281x_sendLow(void) // 发送0码
{
  PAout(8) = 1;
  ws281x_delay(1); // 示波器测试约为440ns
  PAout(8) = 0;
  ws281x_delay(2);
}

void ws281x_sendHigh(void) // 发送1码
{
  PAout(8) = 1;
  ws281x_delay(2);
  PAout(8) = 0;
  ws281x_delay(1);
}

void ws2811_Reset(void) // 发送RESET码
{
  PAout(8) = 0;
  delay_us(60);
  PAout(8) = 1;
  PAout(8) = 0;
}

/**
 * @brief  发送点亮一个灯的数据（即24bit）
 * @param  dat：颜色的24位编码
 * @retval None
 * @note 不包含帧间延时
 */
void ws281x_sendOne(uint32_t dat)
{
  int8_t i;
  unsigned char byte;
  for (i = 23; (i < 24) & (i >= 0); i--)
  {
    byte = ((dat >> i) & 0x01); // 位操作，读取dat数据的第i位
    if (byte == 1)
    {
      ws281x_sendHigh();
    }
    else
    {
      ws281x_sendLow();
    }
  }
}

/**
 * @brief 以RGB的形式发送颜色
 *
 * @param NUM 对应灯珠的编号，从0开始
 * @param R R通道颜色值
 * @param G G通道颜色值
 * @param B B通道颜色值
 */
void WS2812_SendColor(uint8_t NUM, uint8_t R, uint8_t G, uint8_t B)
{
  // 索引限幅
  NUM = (NUM >= LED_NUM) ? LED_NUM - 1 : NUM;
  // 颜色转换(将输入的RGB颜色转换为24位GRB编码)
  LED_BUFFER[NUM] = ((uint32_t)G << 16) | ((uint32_t)R << 8) | (uint32_t)B;
  // 发送帧间延时（60us）
  delay_us(60);
  // 发送全体数组w
  for (int i = 0; i < LED_NUM; i++)
  {
    ws281x_sendOne(LED_BUFFER[i]);
  }
}
/**
 * @brief 以24位GRB的形式发送颜色
 *
 * @param NUM 对应灯珠的编号，从0开始
 * @param Color 颜色的GRB编码（24位）
 * @note 常常搭配WS2812_Wheel函数使用
 */
void WS2812_SendColor_u32(uint8_t NUM, uint32_t Color)
{
  // 索引限幅
  NUM = (NUM >= LED_NUM) ? LED_NUM - 1 : NUM;
  // 存入数组
  LED_BUFFER[NUM] = Color;
  // 发送帧间延时（60us）
  delay_us(60);
  // 发送全体数组w
  for (int i = 0; i < LED_NUM; i++)
  {
    ws281x_sendOne(LED_BUFFER[i]);
  }
}

/**
 * @brief 色彩轮盘（256色）
 *
 * @param pos 颜色值
 * @return uint32_t 输出的GRB编码，搭配WS2812_SendColor_u32函数使用
 */
uint32_t WS2812_Wheel(uint32_t pos)
{
  uint32_t WheelPos = pos % 0xff;
  if (WheelPos < 85)
  {
    return ((255 - WheelPos * 3) << 16) | ((WheelPos * 3) << 8);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return (((255 - WheelPos * 3) << 8) | (WheelPos * 3));
  }
  WheelPos -= 170;
  return ((WheelPos * 3) << 16 | (255 - WheelPos * 3));
}

/**
 * @brief 让所有灯珠显示以pos为起始的颜色流，在色轮上以步长step递增
 *
 * @param pos 起始颜色
 */
void WS2812_StreamColor(u8 pos)
{
  int step = 3;
  for (uint8_t i = 0; i < LED_NUM; i++)
  {
    uint32_t color = WS2812_Wheel(pos + step * i);
    WS2812_SendColor_u32(i, color);
  }
}
