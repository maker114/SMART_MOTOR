#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Board.h"
#include "motor.h"

int main(void)
{
	u16 led0pwmval = 0;
	u8 dir = 1;
	delay_init();
	uart_init(9600);
	Board_Init();
	MOTOR_Init();
	while (1)
	{
		delay_ms(10);
		if (dir)
			led0pwmval++;
		else
			led0pwmval--;

		if (led0pwmval > 800)
			dir = 0;
		if (led0pwmval == 0)
			dir = 1;
		TIM_SetCompare4(TIM3, led0pwmval);
		TIM_SetCompare3(TIM3, led0pwmval);
	}
}
