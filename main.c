#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Board.h"
#include "motor.h"
#include "INA226.h"
#include "oled.h"

int main(void)
{
	delay_init();
	uart_init(9600);
	Board_Init();
	MOTOR_Init();
	INA226_Init();		
	OLED_Init();
	OLED_ShowString(0,0,"HELLO WORLD",8,1);
	OLED_Refresh();
	

	while (1)
	{
		MOTOR_SetPWM(MOTOR_A, 6000);
		MOTOR_SetPWM(MOTOR_B, 6000);
		float v = INA226_GetVoltage(WRITE_ADDR1) * 0.00125f;
		delay_ms(10);
		float i = INA226_ReadCurrent_mA(WRITE_ADDR1);
		delay_ms(10);

		//printf("%.1fv , %.1fma\r\n", v, i);
		
		delay_ms(100);
	}
}
