# SMART_MOTOR
## 定时器配置
### PWM定时器--TIM3
- PWM频率：10KHz
- 最大比较值：7200
- 通道：TIM3_CH3 ，TIM3_CH4
- 引脚：PB0（CH3），PB1（CH4）
- 电机映射： PB0->MOTOR_A PB1->MOTOR_B
### 编码器定时器--TIM2/TIM4
- 定时器：TIM2/TIM4
- 引脚：PA0/PA1/PB6/PB7
- 通道映射：
    - MOTOR_A：TIM2
        - PA0->EA-A
        - PA1->EA-B
    - MOTOR_B：TIM4
        - PB6->EB-A
        - PB7->EB-B

### 速度定时器
- 定时器：TIM1
- 周期：20ms
- 用于将编码器获得的值转化为转速

## 方向控制
#### 方向控制引脚：
- AIN1 PA3
- AIN2 PA4
- BIN1 PA5
- BIN2 PA6

## 电池检测
#### 电池检测引脚：
- 通道：ADC1-Channel2
- 引脚：PA2

## 电压电流检测
#### INA226引脚：
- SDA：PB13
- SCL：PB12

## OLED显示
#### OLED引脚：
- SCL：PB5
- SDA：PB8
- RES: PB9
- DC: PB14
- CS: PB15

## 总连接表

| 模块              | 名称       | 对应引脚                 | 简单备注                |
| --------------- | -------- | -------------------- | ------------------- |
| PWM 定时器 - TIM3  | MOTOR\_A | PB0（CH3）             | 控制电机 A 的 PWM 信号输出   |
| PWM 定时器 - TIM3  | MOTOR\_B | PB1（CH4）             | 控制电机 B 的 PWM 信号输出   |
| 编码器定时器 - TIM2   | EA-A     | PA0                  | 电机 A 编码器 A 相输入      |
| 编码器定时器 - TIM2   | EA-B     | PA1                  | 电机 A 编码器 B 相输入      |
| 编码器定时器 - TIM4   | EB-A     | PB6                  | 电机 B 编码器 A 相输入      |
| 编码器定时器 - TIM4   | EB-B     | PB7                  | 电机 B 编码器 B 相输入      |
| 方向控制            | AIN1     | PA3                  | 控制电机 A 方向的引脚 1      |
| 方向控制            | AIN2     | PA4                  | 控制电机 A 方向的引脚 2      |
| 方向控制            | BIN1     | PA5                  | 控制电机 B 方向的引脚 1      |
| 方向控制            | BIN2     | PA6                  | 控制电机 B 方向的引脚 2      |
| 电池检测            | 电池检测通道   | PA2（ADC1 - Channel2） | 用于检测电池电压            |
| 电压电流检测 - INA226 | SDA      | PB13                 | 与 INA226 通信的数据线     |
| 电压电流检测 - INA226 | SCL      | PB12                 | 与 INA226 通信的时钟线     |
| OLED 显示         | SCL      | PB5                  | OLED 显示屏时钟线         |
| OLED 显示         | SDA      | PB8                  | OLED 显示屏数据线         |
| OLED 显示         | RES      | PB9                  | OLED 显示屏复位引脚        |
| OLED 显示         | DC       | PB14                 | OLED 显示屏数据 / 命令控制引脚 |
| OLED 显示         | CS       | PB15                 | OLED 显示屏片选引脚        |
