#include "board.h"
/*==========文件说明==========*/
// 配置了开发板上的外设以方便使用
// 引脚可以在头文件中更改

/*==========初始化外设==========*/
void Board_Init(void)
{
    /***配置LED***/
    // LED-> PC13
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);

//    /***配置按键***/
//    // KEY->PA0
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*==========按键扫描函数==========*/
int key_up = 1; // 松开：1  按下：0
// 对板载按键进行扫描
// mode=0 -> 不支持连按
// mode=1 -> 支持连按
// 无按键按下 -> 返回0
// KEY -> 返回1
int Board_KeyScan(int mode)
{
    if (mode == 1)
        key_up = 1;
    if (key_up == 1 && (KEY == 0))
    {
        key_up = 0;
        if (KEY == 0)
            return 1; // 按键0按下
    }
    if (KEY == 1)
        key_up = 1;
    return 0;
}

// 反转LED
void Board_LED_Toggle(void)
{
    LED = !LED;
}
