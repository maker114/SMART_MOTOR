/**
 * @file usart_link.h
 * @author maker114
 * @brief ����ͨ��Э�����ͷ�ļ�
 * @version 0.1
 * @date 2025-06-16
 */
#ifndef __USARTLINK_H
#define __USARTLINK_H

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

#define LINK_FRAME_HEADER1 0xA1 ///<  ֡ͷ1
#define LINK_FRAME_HEADER2 0xB1 ///<  ֡ͷ2

typedef enum
{
    LINK_FrameHeader1, ///<  ֡ͷ1
    LINK_FrameHeader2, ///<  ֡ͷ2
    LINK_CMD,          ///<  ָ��
    LINK_DATA_H,       ///<  ���ݸ��ֽ�
    LINK_DATA_L,       ///<  ���ݵ��ֽ�
    LINK_CheckSum,     ///<  У���
    SUM                ///<  ���ݰ�����
} linkpackenum;

typedef enum
{
    LINK_CMD_IN_NULL = 0x00,
    LINK_CMD_IN_SET_LED = 0x01,             ///<  ����LED�����ֽ�LED��ţ����ֽ�LED״̬
    LINK_CMD_IN_SET_MotorAPWM = 0x12,       ///<  ���õ��A PWM�����ֶ�ģʽ��Ч��
    LINK_CMD_IN_SET_MotorBPWM = 0x22,       ///<  ���õ��B PWM�����ֶ�ģʽ��Ч��
    LINK_CMD_IN_SET_MotorAGoalSpeed = 0x13, ///<  ���õ��A Ŀ���ٶȣ����Զ�ģʽ��Ч��
    LINK_CMD_IN_SET_MotorBGoalSpeed = 0x23, ///<  ���õ��B Ŀ���ٶȣ����Զ�ģʽ��Ч��
    LINK_CMD_IN_SET_MotorMode = 0x04,       ///<  ���õ��ģʽ��1Ϊ�ֶ�ģʽ��0Ϊ�Զ�ģʽ
    LINK_CMD_IN_SET_Orientation = 0x05,     ///<  ����ģ�鰲װ����1Ϊ��ʱ�룬0Ϊ˳ʱ��
    LINK_CMD_IN_SET_SwitchOLED = 0x06,      ///<  ����OLED���أ�1Ϊ����0Ϊ��
    LINK_CMD_IN_SET_MotorStatue = 0x07,     ///<  ���õ��״̬��1Ϊ������0Ϊֹͣ
    LINK_CMD_IN_ASK_ADC = 0x08,             ///<  ��ѯ��ص���
    LINK_CMD_IN_ASK_MotorASpeed = 0x09,     ///<  ��ѯ���A�ٶ�
    LINK_CMD_IN_ASK_MotorBSpeed = 0x0A,     ///<  ��ѯ���B�ٶ�
} linkcmd_in;                               ///<  ����ָ��

typedef enum
{
    LINK_CMD_OUT_NULL = 0x00,           ///<  ��
    LINK_CMD_OUT_RE_ADC = 0x01,         ///<  �ش���ص���(����=100)
    LINK_CMD_OUT_RE_MotorASpeed = 0x02, ///<  �ش����A�ٶ�(����=1)
    LINK_CMD_OUT_RE_MotorBSpeed = 0x03, ///<  �ش����B�ٶ�(����=1)
} linkcmd_out;                          ///<  ����ָ��

void LINK_Init(void);
void LINK_SendPack(uint8_t CMD, uint16_t DATA);
void LINK_GetPack(void);
void LINK_HandleData(uint8_t CMD, int16_t DATA);

#endif
