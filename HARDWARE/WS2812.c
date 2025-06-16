/**
 * @file WS2812.c
 * @author maker114
 * @brief WS2812��������
 * @version 0.1
 * @date 2025-06-16
 */
#include "WS2812.h"

// ���岿��
int LED_NUM = 0;                // LED������
uint32_t LED_BUFFER[100] = {0}; // LED������,���洢������Ϊ24λ��RGB����

/**
 * @brief ��ʼ��WS2812���Ų�����Ƶ�����
 * @param NUM LED������
 */
void WS2812_init(uint8_t NUM)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��PA�˿�ʱ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;             // WS2811-->PA.0 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // �������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);                // �����趨������ʼ��GPIOA.0
  GPIO_ResetBits(GPIOA, GPIO_Pin_8);                    // PA.0 ����͵�ƽ
  LED_NUM = NUM;                                        // ��ʼ��LED����
  WS2812_SendColor(0, 0x00, 0x00, 0x00);
  WS2812_SendColor(1, 0x00, 0x00, 0x00);
  WS2812_SendColor(2, 0x00, 0x00, 0x00);
  WS2812_SendColor(3, 0x00, 0x00, 0x00);
}

/**
 * @brief  ws281xģ���õ�����ʱ����
 * @param  delay_num :��ʱ�� ��ʾ����������ʱʱ�� = delay_num * 440ns ��
 */
void ws281x_delay(unsigned int delay_num)
{
  while (delay_num--)
    ;
}

/**
 * @brief  ����WS281xоƬʱ��ͼ��д�ķ���0�룬1����RESET��ĺ���
 */
void ws281x_sendLow(void) // ����0��
{
  PAout(8) = 1;
  ws281x_delay(1); // ʾ��������ԼΪ440ns
  PAout(8) = 0;
  ws281x_delay(2);
}

/**
 * @brief ����1��
 *
 */
void ws281x_sendHigh(void) // ����1��
{
  PAout(8) = 1;
  ws281x_delay(2);
  PAout(8) = 0;
  ws281x_delay(1);
}

/**
 * @brief ����RESET��
 *
 */
void ws2811_Reset(void) // ����RESET��
{
  PAout(8) = 0;
  delay_us(60);
  PAout(8) = 1;
  PAout(8) = 0;
}

/**
 * @brief  ���͵���һ���Ƶ����ݣ���24bit��
 * @param  dat����ɫ��24λ����
 * @note ������֡����ʱ
 */
void ws281x_sendOne(uint32_t dat)
{
  int8_t i;
  unsigned char byte;
  for (i = 23; (i < 24) & (i >= 0); i--)
  {
    byte = ((dat >> i) & 0x01); // λ��������ȡdat���ݵĵ�iλ
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
 * @brief ��RGB����ʽ������ɫ
 *
 * @param NUM ��Ӧ����ı�ţ���0��ʼ
 * @param R Rͨ����ɫֵ
 * @param G Gͨ����ɫֵ
 * @param B Bͨ����ɫֵ
 */
void WS2812_SendColor(uint8_t NUM, uint8_t R, uint8_t G, uint8_t B)
{
  // �����޷�
  NUM = (NUM >= LED_NUM) ? LED_NUM - 1 : NUM;
  // ��ɫת��(�������RGB��ɫת��Ϊ24λGRB����)
  LED_BUFFER[NUM] = ((uint32_t)G << 16) | ((uint32_t)R << 8) | (uint32_t)B;
  // ����֡����ʱ��60us��
  delay_us(60);
  // ����ȫ������w
  for (int i = 0; i < LED_NUM; i++)
  {
    ws281x_sendOne(LED_BUFFER[i]);
  }
}
/**
 * @brief ��24λGRB����ʽ������ɫ
 *
 * @param NUM ��Ӧ����ı�ţ���0��ʼ
 * @param Color ��ɫ��GRB���루24λ��
 * @note ��������WS2812_Wheel����ʹ��
 */
void WS2812_SendColor_u32(uint8_t NUM, uint32_t Color)
{
  // �����޷�
  NUM = (NUM >= LED_NUM) ? LED_NUM - 1 : NUM;
  // ��������
  LED_BUFFER[NUM] = Color;
  // ����֡����ʱ��60us��
  delay_us(60);
  // ����ȫ������w
  for (int i = 0; i < LED_NUM; i++)
  {
    ws281x_sendOne(LED_BUFFER[i]);
  }
}

/**
 * @brief ɫ�����̣�256ɫ��
 *
 * @param pos ��ɫֵ
 * @return uint32_t �����GRB���룬����WS2812_SendColor_u32����ʹ��
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
 * @brief �����е�����ʾ��posΪ��ʼ����ɫ������ɫ�����Բ���step����
 *
 * @param pos ��ʼ��ɫ
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
