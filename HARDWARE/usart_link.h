#ifndef __USARTLINK_H
#define __USARTLINK_H

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

#define LINK_FRAME_HEADER1 0xA1
#define LINK_FRAME_HEADER2 0xB1

typedef enum
{
    LINK_FrameHeader1, // 帧头1
    LINK_FrameHeader2, // 帧头2
    LINK_CMD,          // 指令
    LINK_DATA_H,       // 数据高字节
    LINK_DATA_L,       // 数据低字节
    LINK_CheckSum,     // 校验和
    SUM                // 数据包长度
} linkpackenum;

typedef enum
{
    LINK_CMD_IN_NULL = 0x00,
    LINK_CMD_IN_SET_LED = 0x01,             // 设置LED，低字节LED编号，高字节LED状态
    LINK_CMD_IN_SET_MotorAPWM = 0x12,       // 设置电机A PWM（仅手动模式有效）
    LINK_CMD_IN_SET_MotorBPWM = 0x22,       // 设置电机B PWM（仅手动模式有效）
    LINK_CMD_IN_SET_MotorAGoalSpeed = 0x13, // 设置电机A 目标速度（仅自动模式有效）
    LINK_CMD_IN_SET_MotorBGoalSpeed = 0x23, // 设置电机B 目标速度（仅自动模式有效）
    LINK_CMD_IN_SET_MotorMode = 0x04,       // 设置电机模式，1为手动模式，0为自动模式
    LINK_CMD_IN_SET_Orientation = 0x05,     // 设置模块安装方向，1为逆时针，0为顺时针
    LINK_CMD_IN_SET_SwitchOLED = 0x06,      // 设置OLED开关，1为开，0为关
    LINK_CMD_IN_SET_MotorStatue = 0x07,     // 设置电机状态，1为启动，0为停止
    LINK_CMD_IN_ASK_ADC = 0x08,             // 查询电池电量
    LINK_CMD_IN_ASK_MotorASpeed = 0x09,     // 查询电机A速度
    LINK_CMD_IN_ASK_MotorBSpeed = 0x0A,     // 查询电机B速度
} linkcmd_in;                               // 接收指令

typedef enum
{
    LINK_CMD_OUT_NULL = 0x00,           // 无
    LINK_CMD_OUT_RE_ADC = 0x01,         // 回传电池电量(倍率=100)
    LINK_CMD_OUT_RE_MotorASpeed = 0x02, // 回传电机A速度(倍率=1)
    LINK_CMD_OUT_RE_MotorBSpeed = 0x03, // 回传电机B速度(倍率=1)
} linkcmd_out;                          // 发送指令

void LINK_Init(void);
void LINK_SendPack(uint8_t CMD, uint16_t DATA);
void LINK_GetPack(void);
void LINK_HandleData(uint8_t CMD, int16_t DATA);

#endif
