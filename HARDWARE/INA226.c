/**
 * @file INA226.c
 * @author maker114
 * @brief INA226����
 * @version 0.1
 * @date 2025-05-21
 *
 *
 */

#include "INA226.h"

// delay
static void delay_nns(uint16_t D) // 30����ns  �����ֲ�Ҫ�õ�IIC��HS����ģʽ
{
    while (--D)
        ;
}

void delay_nms(uint16_t ms) // ����
{
    uint16_t i;
    uint32_t M = 0; // 720W
    for (i = 0; i < ms; i++)
        for (M = 12000; M > 0; M--)
            ;
}

void delay_nus(uint16_t us) // ΢��
{
    uint16_t i;
    uint16_t M = 0; // 720W
    for (i = 0; i < us; i++)
        for (M = 72; M > 0; M--)
            ;
}

// IIC

/****************IIC***************************/
void INA226_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(INA_SCL_RCC | INA_SDA_RCC, ENABLE); /* ��GPIOʱ�� */

    GPIO_InitStructure.GPIO_Pin = INA_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  //��Ҫ��SCL,SDA ����10k ��VCC
    GPIO_Init(INA_SCL_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = INA_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  //��Ҫ��SCL,SDA ����10k ��VCC
    GPIO_Init(INA_SDA_GPIO_PORT, &GPIO_InitStructure);

    INA_SCL = 1;
    INA_SDA = 1;
    delay_nms(5);
}

/**********************IIC_END*************************/

void INA226_Init(void)
{
    uint16_t Cal;
    INA226_IIC_Init();
    delay_nms(10);

    INA226_SendData(WRITE_ADDR, 0x00, 0x4807); // ���ò�����ƽ��ֵ128�����ߵ�ѹת��ʱ��0.14ms��������ѹת��ʱ��0.14ms����������ģʽ
                                               // ����У׼ֵ
    Current_LSB = (float)MAX_CURRENT / 32768;
    // Current_LSB = 0.000305f;		// �� MAX_CURRENT / 2��15�η�
    Cal = (uint16_t)(0.00512f / (Current_LSB * SHUNT_RESISTOR));
    INA226_SendData(WRITE_ADDR1, 0x05, Cal);
}

void INA226_Init1(void)
{
    uint16_t Cal;
    INA226_IIC_Init();
    delay_nms(10);

    INA226_SendData(WRITE_ADDR1, 0x00, 0x4807); // ���ò�����ƽ��ֵ128�����ߵ�ѹת��ʱ��0.14ms��������ѹת��ʱ��0.14ms����������ģʽ

    // ����У׼ֵ
    Current_LSB = (float)MAX_CURRENT / 32768;
    // Current_LSB = 0.000305f;		// �� MAX_CURRENT / 2��15�η�
    Cal = (uint16_t)(0.00512f / (Current_LSB * SHUNT_RESISTOR));
    INA226_SendData(WRITE_ADDR1, 0x05, Cal);

    Power_LSB = Current_LSB * 25.0f; // ����У׼ֵ = ������Сֵ * 25��
}

void INA226_IIC_Start(void)
{
    INA_SCL = 1;
    INA_SDA = 1;
    delay_nns(5);
    INA_SDA = 0; // START:when CLK is high,DATA change form high to low
    delay_nns(5);
    INA_SCL = 0; // ǯסI2C���ߣ�׼�����ͻ��������
    delay_nns(5);
}

void INA226_IIC_Stop(void)
{
    INA_SDA = 0; // STOP:when CLK is high DATA change form low to high
    delay_nns(5);
    INA_SCL = 1;
    delay_nns(5);
    INA_SDA = 1; // ����I2C���߽����ź�
    delay_nns(5);
}

void INA226_IIC_Ack(void)
{
    INA_SDA = 0;
    delay_nns(5);
    INA_SCL = 1;
    delay_nns(5);
    INA_SCL = 0;
    delay_nns(5);
    INA_SDA = 1;
}

void INA226_IIC_NAck(void)
{
    INA_SDA = 1;
    delay_nns(5);
    INA_SCL = 1;
    delay_nns(5);
    INA_SCL = 0;
    ;
    delay_nns(5);
    INA_SDA = 0;
}

uint8_t INA226_IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;

    INA_SDA = 1;
    delay_nns(5);
    INA_SCL = 1;
    delay_nns(5);

    while (READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            INA226_IIC_Stop();
            return 1;
        }
    }
    INA_SCL = 0; // ʱ�����0
    return 0;
}

void INA226_IIC_Send_Byte(uint8_t txd)
{
    int i;
    INA_SCL = 0;
    ; // ����ʱ�ӿ�ʼ���ݴ���
    for (i = 0; i < 8; i++)
    {
        if (txd & 0x80)
            INA_SDA = 1;
        else
            INA_SDA = 0;
        txd <<= 1;

        INA_SCL = 1;
        delay_nns(5);
        INA_SCL = 0;
        delay_nns(5);
    }
}

uint8_t INA226_IIC_Read_Byte(unsigned char ack)
{
    uint8_t TData = 0, i;
    for (i = 0; i < 8; i++)
    {
        INA_SCL = 1;
        delay_nns(5);
        TData = TData << 1;
        if (READ_SDA)
        {
            TData |= 0x01;
        }
        INA_SCL = 0;
        ;
        delay_nns(5);
    }
    if (!ack)
        INA226_IIC_NAck();
    else
        INA226_IIC_Ack();
    return TData;
}

void INA226_SendData(uint8_t addr, uint8_t reg, uint16_t data)
{
    uint8_t temp = 0;
    INA226_IIC_Start();
    INA226_IIC_Send_Byte(addr);
    INA226_IIC_Wait_Ack();

    INA226_IIC_Send_Byte(reg);
    INA226_IIC_Wait_Ack();

    temp = (uint8_t)(data >> 8);
    INA226_IIC_Send_Byte(temp);
    INA226_IIC_Wait_Ack();

    temp = (uint8_t)(data & 0x00FF);
    INA226_IIC_Send_Byte(temp);
    INA226_IIC_Wait_Ack();

    INA226_IIC_Stop();
}

void INA226_SetRegPointer(uint8_t addr, uint8_t reg)
{
    INA226_IIC_Start();

    INA226_IIC_Send_Byte(addr);
    INA226_IIC_Wait_Ack();

    INA226_IIC_Send_Byte(reg);
    INA226_IIC_Wait_Ack();

    INA226_IIC_Stop();
}

uint16_t INA226_ReadData(uint8_t addr)
{
    uint16_t temp = 0;
    INA226_IIC_Start();

    INA226_IIC_Send_Byte(addr + 1);
    INA226_IIC_Wait_Ack();

    temp = INA226_IIC_Read_Byte(1);
    temp <<= 8;
    temp |= INA226_IIC_Read_Byte(0);

    INA226_IIC_Stop();
    return temp;
}

// ��ȡ��������λ��A��
float INA226_ReadCurrent_A(uint8_t addr)
{
    uint16_t raw = INA226_GetShuntCurrent(addr);
    return raw * Current_LSB; // ����ʵ��LSB����
}
// ��ȡ��������λ��mA��
float Last_Curren=0;
float INA226_ReadCurrent_mA(uint8_t addr)
{
    float Curren;
    uint16_t raw = INA226_GetShuntCurrent(addr);
    Curren = raw * Current_LSB;
    Curren *= 1000;
	
		if(Curren>9000)
			return Last_Curren;
		else
		{
			Last_Curren = Curren;
			return Curren; // ����ʵ��LSB����
		}
			
}

// ��ȡ���ʣ���λ��W��
float INA226_ReadPower(uint8_t addr)
{
    uint16_t raw = INA226_Get_Power(addr);
    return raw * Power_LSB; // 25 * ������Сֵ
}

uint16_t INA226_GetShuntCurrent(uint8_t addr)
{
    uint16_t temp = 0;
    INA226_SetRegPointer(addr, Current_Reg);
    temp = INA226_ReadData(addr);
    //   if(temp&0x8000)    temp = ~(temp - 1);//������������������������
    return temp;
}

// ��ȡ id
uint16_t INA226_Get_ID(uint8_t addr)
{
    uint16_t temp = 0;
    INA226_SetRegPointer(addr, ID_Reg);
    temp = INA226_ReadData(addr);
    return (uint16_t)temp;
}

// ��ȡУ׼ֵ
uint16_t INA226_GET_CAL_REG(uint8_t addr)
{
    uint16_t temp = 0;
    INA226_SetRegPointer(addr, Calib_Reg);
    temp = INA226_ReadData(addr);
    return temp;
}

// 1.25mV/bit
uint16_t INA226_GetVoltage(uint8_t addr)
{
    uint16_t temp = 0;
    INA226_SetRegPointer(addr, Bus_V_Reg);
    temp = INA226_ReadData(addr);
    return temp;
}

// 2.5uV/bit
uint16_t INA226_GetShuntVoltage(uint8_t addr)
{
    uint16_t temp = 0;
    INA226_SetRegPointer(addr, Shunt_V_Reg);
    temp = INA226_ReadData(addr);
    if (temp & 0x8000)
        temp = ~(temp - 1);
    return temp;
}

// 2.5mW/bit
uint16_t INA226_Get_Power(uint8_t addr)
{
    uint16_t temp = 0;
    INA226_SetRegPointer(addr, Power_Reg);
    temp = INA226_ReadData(addr);
    return temp;
}
