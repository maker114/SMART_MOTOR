#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Board.h"
#include "esp_uart.h"
#include "soil.h"

// ID Setting
u8 ID = 0xB1;

int main(void)
{
	delay_init();
	uart_init(9600);
	Board_Init();
	LED = !LED;
	AD_Init();

	while (1)
	{

		if (USART_GetID() == ID)
		{
			LED = !LED;
			USART_SendPage(AD_GetValue());
			// delay_ms(50);
			// USART_SendPage(AD_GetValue());
			USART_ResetID();
		}
	}
}
