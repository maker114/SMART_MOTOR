#ifndef _INA226_H
#define _INA226_H

#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "board.h"

/********** Ӳ���������� **********/
#define SHUNT_RESISTOR 0.010f // ��������10��ŷ
#define MAX_CURRENT 10.0f     // ������

///********** ������� **********/
static float Current_LSB; // �����ֱ���
static float Power_LSB;   // ����У׼ֵ

#define INA_SCL PBout(12) // SCL����
#define INA_SDA PBout(13) // SDA����

#define INA_SCL_GPIO_PORT GPIOB          /* GPIO�˿� */
#define INA_SCL_RCC RCC_APB2Periph_GPIOB /* GPIO�˿�ʱ�� */
#define INA_SCL_PIN GPIO_Pin_12          /* ���ӵ�SCLʱ���ߵ�GPIO */

#define INA_SDA_GPIO_PORT GPIOB          /* GPIO�˿� */
#define INA_SDA_RCC RCC_APB2Periph_GPIOB /* GPIO�˿�ʱ�� */
#define INA_SDA_PIN GPIO_Pin_13          /* ���ӵ�SDA�����ߵ�GPIO */

#define READ_SDA GPIO_ReadInputDataBit(INA_SDA_GPIO_PORT, INA_SDA_PIN)

#define READ_ADDR 0x81  // A0=GND��A1=GND // R=1, W=0
#define WRITE_ADDR 0x80 // оƬIIC��ַ

#define READ_ADDR1 0x8B  // A0=VCC��A1=VCC // R=1, W=0
#define WRITE_ADDR1 0x8A // оƬIIC��ַ

#define Config_Reg 0x00  // ģʽ���üĴ���
#define Shunt_V_Reg 0x01 // ��������ѹ�ȡֵ��2.5uV~81.92mV(0~7FFF)(10m������������������8A)
#define Bus_V_Reg 0x02   // BUS���ߵ�ѹ��1.25mV~40.96V(0~7FFF)
#define Power_Reg 0x03   // ���ʣ�Current_LSB * 25��FSR = 0xFFFF
#define Current_Reg 0x04 // ������LSB = 0.02mA��FSR = 0x7FFF
#define Calib_Reg 0x05   // ������λΪmAʱ����������0.002R=0xA00;0.01R=0x200����д��Calibration Register��������Ҫ����ʵ�ʵ�·�еĲ���������ֵ�Լ������ֱ���������
#define Mask_En_Reg 0x06 //
#define Alert_Reg 0x07   //
#define Man_ID_Reg 0xFE  // 0x5449
#define ID_Reg 0xFF      // 0x2260

void INA226_Init(void);
void INA226_Init1(void);
void INA226_SendData(uint8_t addr, uint8_t reg, uint16_t data);
uint16_t INA226_Get_ID(uint8_t addr);
uint16_t INA226_GetShuntVoltage(uint8_t addr);
uint16_t INA226_GetShuntCurrent(uint8_t addr);
uint16_t INA226_GetVoltage(uint8_t addr);
uint16_t INA226_Get_Power(uint8_t addr);

float INA226_ReadCurrent_A(uint8_t addr);
float INA226_ReadCurrent_mA(uint8_t addr);
float INA226_ReadPower(uint8_t addr);

#endif
