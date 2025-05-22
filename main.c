#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Board.h"
#include "motor.h"
#include "INA226.h"
#include "oled.h"
#include "WS2812.h"

int main(void)
{
	delay_init();
	uart_init(9600);
	Board_Init();
	MOTOR_Init();
	INA226_Init();	
	WS2812_init(8);
	OLED_Init();
	OLED_ShowString(0,0,"HELLO WORLD",8,1);
	OLED_Refresh();
	int i =0;
	while (1)
	{
		
		WS2812_StreamColor(i);
		i++;
		delay_ms(10);
	}
}
