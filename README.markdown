# SMART_MOTOR
## ��ʱ������
### PWM��ʱ��--TIM3
- PWMƵ�ʣ�10KHz
- ���Ƚ�ֵ��7200
- ͨ����TIM3_CH3 ��TIM3_CH4
- ���ţ�PB0��CH3����PB1��CH4��
- ���ӳ�䣺 PB0->MOTOR_A PB1->MOTOR_B
### ��������ʱ��--TIM2/TIM4
- ��ʱ����TIM2/TIM4
- ���ţ�PA0/PA1/PB6/PB7
- ͨ��ӳ�䣺
    - MOTOR_A��TIM2
        - PA0->EA-A
        - PA1->EA-B
    - MOTOR_B��TIM4
        - PB6->EB-A
        - PB7->EB-B

### �ٶȶ�ʱ��
- ��ʱ����TIM1
- ���ڣ�20ms
- ���ڽ���������õ�ֵת��Ϊת��

## �������
#### ����������ţ�
- AIN1 PA3
- AIN2 PA4
- BIN1 PA5
- BIN2 PA6

## ��ؼ��
#### ��ؼ�����ţ�
- ͨ����ADC1-Channel2
- ���ţ�PA2

## ��ѹ�������
#### INA226���ţ�
- SDA��PB13
- SCL��PB12

## OLED��ʾ
#### OLED���ţ�
- SCL��PB5
- SDA��PB8
- RES: PB9
- DC: PB14
- CS: PB15

## WS2812 LED �ƴ�
#### WS2812���ţ�
- DAT��PA8

## ���ڴ���
#### �������ţ�
- USART1
    - TX: PA9
    - RX: PA10
- USART3
    - TX: PB10
    - RX: PB11
#### �������ݰ���USART3����
- ���ݰ���ʽ��
    - LINK_FrameHeader1
    - LINK_FrameHeader2
    - LINK_CMD
    - LINK_DATA_H
    - LINK_DATA_L
    - LINK_CheckSum

## �����ӱ�

| ģ��              | ����       | ��Ӧ����                 | ��ע                |
| --------------- | -------- | -------------------- | ------------------- |
| PWM ��ʱ�� - TIM3  | PWM\_A | PB0��CH3��             | ���Ƶ�� A �� PWM �ź����   |
| PWM ��ʱ�� - TIM3  | PWM\_B | PB1��CH4��             | ���Ƶ�� B �� PWM �ź����   |
| ��������ʱ�� - TIM2   | EA-A     | PA0                  | ��� A ������ A ������      |
| ��������ʱ�� - TIM2   | EA-B     | PA1                  | ��� A ������ B ������      |
| ��������ʱ�� - TIM4   | EB-A     | PB6                  | ��� B ������ A ������      |
| ��������ʱ�� - TIM4   | EB-B     | PB7                  | ��� B ������ B ������      |
| �������            | AIN1     | PA3                  | ���Ƶ�� A ��������� 1      |
| �������            | AIN2     | PA4                  | ���Ƶ�� A ��������� 2      |
| �������            | BIN1     | PA5                  | ���Ƶ�� B ��������� 1      |
| �������            | BIN2     | PA6                  | ���Ƶ�� B ��������� 2      |
| ��ؼ��            | ADC  | PA2��ADC1 - CH2�� | ���ڼ���ص�ѹ            |
| ��ѹ������� - INA226 | SDA      | PB13                 | �� INA226 ͨ�ŵ�������     |
| ��ѹ������� - INA226 | SCL      | PB12                 | �� INA226 ͨ�ŵ�ʱ����     |
| OLED ��ʾ         | SCL      | PB5                  | OLED ��ʾ��ʱ����         |
| OLED ��ʾ         | SDA      | PB8                  | OLED ��ʾ��������         |
| OLED ��ʾ         | RES      | PB9                  | OLED ��ʾ����λ����        |
| OLED ��ʾ         | DC       | PB14                 | OLED ��ʾ������ / ����������� |
| OLED ��ʾ         | CS       | PB15                 | OLED ��ʾ��Ƭѡ����        |
| WS2812 LED �ƴ�   | DAT      | PA8                   | ���� WS2812 LED �ƴ�������   |
| ���ڴ��� - USART1  | TX       | PA9                  | USART1 �ķ�������         |
| ���ڴ��� - USART1  | RX       | PA10                 | USART1 �Ľ�������         |
| ��������           | KEY1     | PA7                  | ����1�Ľ�������        |
| ��������           | KEY2     | PA11                 | ����2�Ľ�������         |
| ��������           | KEY3     | PA12                 | ����3�Ľ�������         |
| ������             | BEEP     | PB3                  | �������ķ�������         |
