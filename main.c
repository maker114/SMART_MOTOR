#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Board.h"
#include "motor.h"

int main(void)
{
	delay_init();
	uart_init(9600);
	Board_Init();
	MOTOR_Init();

	while (1)
	{
		// printf("%d,%d\r\n", MOTOR_GetSpeed(MOTOR_A), MOTOR_GetSpeed(MOTOR_B));
		//  delay_ms(100);
		MOTOR_SetPWM(MOTOR_A, 1000);
		MOTOR_SetPWM(MOTOR_B, 1000);
		delay_ms(1000);
		MOTOR_SetPWM(MOTOR_A, -1000);
		MOTOR_SetPWM(MOTOR_B, 1000);
		delay_ms(1000);
		MOTOR_SetPWM(MOTOR_A, 1000);
		MOTOR_SetPWM(MOTOR_B, -1000);
		delay_ms(1000);
		MOTOR_SetPWM(MOTOR_A, -1000);
		MOTOR_SetPWM(MOTOR_B, -1000);
		delay_ms(1000);
	}
}
