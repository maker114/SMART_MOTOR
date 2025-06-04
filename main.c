#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Board.h"
#include "motor.h"
#include "INA226.h"
#include "WS2812.h"
#include "oled.h"
#include "usart_link.h"

void main_UI(void);

int main(void)
{
	delay_init();
	uart_init(9600);
	Board_Init();
	MOTOR_Init();
	INA226_Init();
	OLED_Init();
	WS2812_init(4);
	LINK_Init();
	Board_BEEP_ContinuousActive(2);
	while (1)
	{
		static uint8_t Display_Mode = 1;
		uint8_t Key_Num = Board_KeyScan();
		if (Key_Num != 0)
			Display_Mode = Key_Num;
		switch (Display_Mode)
		{
		case 1:
			main_UI();
			break;
		case 2:
			OLED_DrawCurve(0.15, -5, INA226_ReadCurrent_mA(WRITE_ADDR1));
		default:
			break;
		}
		MOTOR_SetPWM(MOTOR_A, 3000);
		MOTOR_SetPWM(MOTOR_B, 3000);
		delay_ms(20);
	}
}

uint8_t Font_W = 6;
uint8_t Font_H = 8;
void main_UI(void)
{
	OLED_Clear();
	OLED_ShowString(0, 0, "Power.C =", Font_H, 1);
	OLED_ShowFNum(Font_W * 11, 0, INA226_ReadCurrent_mA(WRITE_ADDR1), 4, Font_H, 1);
	OLED_ShowString(Font_W * (11 + 7), 0, "MA", Font_H, 1);

	OLED_ShowString(0, Font_H, "Power.V =", Font_H, 1);
	OLED_ShowFNum(Font_W * 13, Font_H, INA226_GetVoltage(WRITE_ADDR1) * 1.25f * 0.001f, 2, Font_H, 1);
	OLED_ShowString(Font_W * (11 + 7), Font_H, "V", Font_H, 1);

	OLED_ShowString(0, Font_H * 2, "Battery.V =", Font_H, 1);
	OLED_ShowFNum(Font_W * 13, Font_H * 2, MOTOR_GetBatteryVoltage(), 2, Font_H, 1);
	OLED_ShowString(Font_W * (11 + 7), Font_H * 2, "V", Font_H, 1);

	OLED_ShowString(0, Font_H * 4, "EN-A:", Font_H, 1);
	OLED_ShowSNum(Font_W * 6, Font_H * 4, MOTOR_GetSpeed(MOTOR_A), 3, Font_H, 1);
	OLED_ShowString(0, Font_H * 5, "PW-A:", Font_H, 1);
	OLED_ShowNum(Font_W * 6, Font_H * 5, MOTOR_GetPWM(MOTOR_A), 4, Font_H, 1);

	OLED_DrawFastVLine(64 - 2, Font_H * 4, Font_H * 2, 1);

	OLED_ShowString(64, Font_H * 4, "EN-B:", Font_H, 1);
	OLED_ShowSNum(64 + Font_W * 6, Font_H * 4, MOTOR_GetSpeed(MOTOR_B), 3, Font_H, 1);
	OLED_ShowString(64, Font_H * 5, "PW-B:", Font_H, 1);
	OLED_ShowNum(64 + Font_W * 6, Font_H * 5, MOTOR_GetPWM(MOTOR_B), 4, Font_H, 1);

	delay_ms(50);
	OLED_Refresh();
}
