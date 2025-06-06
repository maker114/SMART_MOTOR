/**
 * @file oled.h
 * @author maker114
 * @brief OLED��
 * @version 2.1
 * @date 2024-02-12
 */

/*
ͼ�ο���ɣ�
|-OLED ����ͼ�ο�
    |-oled.h
    |-oledfont.h <-�洢����
    |-bmp.h <-�洢ͼƬ����
    |-oled.c
|-MENU ��ʽ�˵�
    |-oled_menu.h
    |-oled_menu.c
|-START ����������ײ�ʵ��
    |-oled_start.h
    |-oled_start.c
*/

#ifndef __OLED_H
#define __OLED_H

#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "stdlib.h"
#include <math.h>

//-----------------OLED����----------------
// #define LED_ON GPIO_SetBits(GPIOB, GPIO_Pin_8)
// #define LED_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_8) // PD10 vcc

#define OLED_SCL_Clr() GPIO_ResetBits(GPIOB, GPIO_Pin_5) // SCL
#define OLED_SCL_Set() GPIO_SetBits(GPIOB, GPIO_Pin_5)

#define OLED_SDA_Clr() GPIO_ResetBits(GPIOB, GPIO_Pin_8) // SDA
#define OLED_SDA_Set() GPIO_SetBits(GPIOB, GPIO_Pin_8)

#define OLED_RES_Clr() GPIO_ResetBits(GPIOB, GPIO_Pin_9) // RES
#define OLED_RES_Set() GPIO_SetBits(GPIOB, GPIO_Pin_9)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOB, GPIO_Pin_14) // DC
#define OLED_DC_Set() GPIO_SetBits(GPIOB, GPIO_Pin_14)

#define OLED_CS_Clr() GPIO_ResetBits(GPIOB, GPIO_Pin_15) // CS
#define OLED_CS_Set() GPIO_SetBits(GPIOB, GPIO_Pin_15)

#define OLED_CMD 0
#define OLED_DATA 1

// ��Ļ��������
#define Middle_X 64
#define Middle_Y 32
// ������ѧʵ��
#define RADIAN(angle) ((angle == 0) ? 0 : (3.14159f * angle / 180))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define SWAP(x, y)   \
    (y) = (x) + (y); \
    (x) = (y) - (x); \
    (y) = (y) - (x);

/*========================OLED========================*/

// |--SHOW
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode);                // ��ʾ�����ַ�
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1, u8 mode);             // ��ʾ�ַ���
void OLED_ShowNum(u8 x, u8 y, int num, u8 len, u8 size1, u8 mode);        // ��ʾ����
void OLED_ShowSNum(u8 x, u8 y, int num, u8 len, u8 size1, u8 mode);       // ��ʾ�з�������
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1, u8 mode);             // ��ʾ�����ַ�
void OLED_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode); // ��ʾͼƬ
void OLED_ShowNum16(u8 x, u8 y, u8 num, u8 size, u8 mode);                // ��ʾ16��������
void OLED_ShowFNum(u8 x, u8 y, float data, u8 len, u8 size, u8 mode);     // ��ʾ������
void OLED_ShowSFNum(u8 x, u8 y, float data, u8 len, u8 size, u8 mode);    // ��ʾ���з��ŵĸ�����

// |--DRAW
void OLED_ClearPoint(u8 x, u8 y);                                          // ���OLED�ϵ�ĳ����
void OLED_DrawPoint(u8 x, u8 y, u8 t);                                     // ��OLED�ϻ�һ����
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);                   // ��OLED�ϻ�һ����
void OLED_DrawCircle_math(u8 x, u8 y, u8 r);                               // ��OLED�ϻ���Բ�Σ�ʹ����ѧ������
void OLED_DrawFastHLine(int x, int y, int w, int mode);                    // ���ٻ�ˮƽ��
void OLED_DrawFastVLine(int x, int y, int h, int mode);                    // ���ٻ���ֱ��
void OLED_DrawCircleQuadrant(u8 x, u8 y, u8 r, u8 quadrant, u8 mode);      // ����Բ�ε�һ������
void OLED_DrawFill(int x, int y, int w, int h, int mode);                  // ����������
void OLED_DrawFill_T(int x, int y, int w, int h, u8 Thickness, int mode);  // ���ض��������������
void OLED_DrawBox(int x, int y, int w, int h, int mode);                   // ���ƾ��ο�
void OLED_DrawRoundedBox(int x, int y, int w, int h, int r, int mode);     // ����Բ�Ǿ��ο�
void OLED_DrawDashedLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);             // ��OLED�ϻ�������
void OLED_DrawCircle(int usX_Center, int usY_Center, int usRadius);        // ��OLED�ϻ���Բ��
void OLED_DrawParallelHLine(u8 x, u8 y, u8 w, u8 h, u8 section, u8 mode);  // �ȼ�໭ˮƽ��
void OLED_DrawParallelVLine(u8 x, u8 y, u8 w, u8 h, u8 section, u8 mode);  // �ȼ�໭��ֱ��
void OLED_DrawVacuumization(u8 x, u8 y, u8 w, u8 h);                       // ����"�黯"Ч��
void OLED_DrawBezierCurve(int x0, int y0, int x1, int y1, int x2, int y2); // ���Ʊ���������
void OLED_DrawBezierBox(int x, int y, int w, int h, int r);                // ���Ʊ��������ο�

/*========================OLED_MENU========================*/
// |--CHANGE
void OLED_MoveCursor(float goal_cursor, float *now_cursor, u8 delay);              // �ƶ����
void OLED_Rotate(u8 *x, u8 *y, u8 Core_x, u8 Core_y, float angle, float distance); // �����ת����
void MENU_SetMode(int set, int Mode);                                              // ������ʾģʽ
void MENU_RollingClear(void);                                                      // ���������ʾ������
void MENU_SignKeyFun(int put_in_fun(void));                                        // �˵�ע�ᰴ������

// |--DISPLAY
void OLED_DrawCurve(float zoom, float skew, float data);              // ��������
int MENU_NormalDisplay(u8 *menu[], u8 arr_len);                       // ������ʾ�˵�
int MENU_SideDisplay(u8 *menu[], u8 arr_len, u8 w);                   // �����ʾ�˵�
void MENU_MiddleDisplay(u8 *chr, float MIN, float MAX, float *value); // �ٷֱȲ˵�
void MENU_USARTDisplay(void);                                         // USART��ʾ
void MENU_RollingDisplaced(u8 *chr);                                  // ������ʾ����
float MENU_ChangeDisplay(u8 *chr, float step, float num);             // ��ֵ���ڲ˵�

// |--ANIMATIONS
void Normal_First_IN(u8 *menu[], u8 arr_len, u8 size);      // ������ʾ�˵��ĳ��ν���
void Side_First_IN(u8 *menu[], u8 arr_len, u8 size, u8 w);  // �����ʾ�˵��ĳ��ν���
void Side_First_OUT(u8 *menu[], u8 arr_len, u8 size, u8 w); // �����ʾ�˵����˳�
void Middle_First_IN(float data, float percent, u8 *chr);   // �м���ʾ���ܵĳ��ν���
void Middle_First_OUT(float data, float percent, u8 *chr);  // �м���ʾ���ܵ��˳�
void Change_First_IN(float num, u8 *chr);                   // ��ֵ�ı�ĳ��ν���
void Change_First_OUT(float num, u8 *chr);                  // ��ֵ�ı���˳�
void Curve_First_IN(void);                                  // ������ʾ�ĳ��ν���

/*========================OLED_START========================*/

void OLED_ColorTurn(u8 i);                          // ��ɫ��ת
void OLED_DisplayTurn(u8 i);                        // ��ʾ��ת
void OLED_WR_Byte(u8 dat, u8 mode);                 // дһ���ֽ����ݻ����OLED
void OLED_DisPlay_On(void);                         // ��OLED��ʾ
void OLED_DisPlay_Off(void);                        // �ر�OLED��ʾ
void OLED_Refresh(void);                            // ˢ��OLED��ʾ����
void OLED_Clear(void);                              // ����
void OLED_Init(void);                               // OLED��ʼ��
void OELD_ChangeLuminance(u8 level);                // ��������
void OLED_BufferSave(void);                         // ���浱ǰ��ʾ������
void OLED_BufferRegain(void);                       // �ָ��������ʾ������
void OLED_ScrollDisplay(u8 num, u8 space, u8 mode); // OLED������ʾ

#endif
