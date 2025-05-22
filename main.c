#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Board.h"
#include "motor.h"
#include "INA226.h"
#include "oled.h"
#include "WS2812.h"
#include "usart_link.h"

int main(void)
{
	delay_init();
	uart_init(9600);
	Board_Init();
	MOTOR_Init();
	INA226_Init();
	WS2812_init(8);
	LINK_Init();

	OLED_Init();
	OLED_ShowString(0, 0, "HELLO WORLD", 8, 1);
	OLED_Refresh();
	int i = 0;
	while (1)
	{
		LINK_SendPack(0xC4, 0xB2E1);
		WS2812_StreamColor(i);
		i++;
		delay_ms(10);
	}
}
