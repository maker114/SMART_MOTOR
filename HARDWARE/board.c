/**
 * @file board.c
 * @author maker114
 * @brief ���ذ�����LED��������������ĳ�ʼ��
 * @version 0.1
 * @date 2025-06-16
 * @note
 * - �����˿������ϵ������Է���ʹ��
 * - ���ſ�����ͷ�ļ��и���
 */
#include "board.h"

/*==========��ʼ������==========*/
void Board_Init(void)
{
    // LED -> PC13
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    // BEEP -> PB3
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_3);
    // KEY1 -> PA7
    // KEY2 -> PA11
    // KEY3 -> PA12
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// �԰��ذ�������ɨ��
int Board_KeyScan(void)
{
    static uint8_t Key_State = 0; // 0���ް������£�1���а�������
    uint8_t Key_num = 0;
    if ((KEY1 == 0 || KEY2 == 0 || KEY3 == 0) && Key_State == 0)
    {
        Key_State = 1;
        Key_num = (KEY1 == 0) ? 1 : ((KEY2 == 0) ? 2 : 3);
    }
    if (KEY1 == 1 && KEY2 == 1 && KEY3 == 1)
        Key_State = 0;
    return Key_num;
}

// ��תLED
void Board_LED_Toggle(void)
{
    LED = !LED;
}

void Board_BEEP_ActiveMs(int ms)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_3);
    delay_ms(ms);
    GPIO_ResetBits(GPIOB, GPIO_Pin_3);
}

void Board_BEEP_ContinuousActive(int time)
{
    for (uint8_t i = 0; i < time; i++)
    {
        Board_BEEP_ActiveMs(20);
        delay_ms(50);
    }
}
