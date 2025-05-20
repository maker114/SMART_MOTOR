#include "esp_uart.h"

u8 USART_GetFlag=0;//串口接收计数位
uint8_t USART_TemporarilyStore = 0; // 串口接收数据暂存
uint8_t USART_ID = 0; // 串口接收ID

void USART1_IRQHandler(void)   
{
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		switch (USART_GetFlag)
		{
		case 0: // 帧头
			if (USART_ReceiveData(USART1) == 0xA1)
				USART_GetFlag = 1;
			else
				USART_GetFlag = 0;
			break;

		case 1: // 数据
			USART_TemporarilyStore = USART_ReceiveData(USART1);
			USART_GetFlag = 2;
			break;

		case 2: // 帧尾
			if (USART_ReceiveData(USART1) == 0xC2)
			{
				USART_ID = USART_TemporarilyStore;
			}
			USART_GetFlag = 0;
			break;

		default:
			USART_GetFlag = 0;
			break;
		}
	}   
}

// 手动向esp发送一个数据包
void USART_SendPage(uint16_t DATA)
{
		//拆分16位DATA
		uint8_t high_byte = (DATA >> 8) & 0xFF;  // 提取高8位
    uint8_t low_byte 	= DATA & 0xFF;          // 提取低8位
		//传输数据包(低位在前)
    USART_SendData(USART1, 0xA1);
    while ((USART1->SR & 1 << 7) == 0)
        ;
    USART_SendData(USART1, low_byte);
    while ((USART1->SR & 1 << 7) == 0)
        ;
		USART_SendData(USART1, high_byte);
    while ((USART1->SR & 1 << 7) == 0)
        ;
    USART_SendData(USART1, 0xA2);
    while ((USART1->SR & 1 << 7) == 0)
        ;
}

u8 USART_GetID(void)
{
	return USART_ID;
}


void USART_ResetID(void)
{
	USART_ID=0;
}


