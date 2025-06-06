# SMART_MOTOR
### PWM定时器--TIM3
| 参数          | 值/配置                      |
|---------------|-----------------------------|
| PWM频率       | 10KHz                      |
| 最大比较值    | 7200                       |
| 通道          | TIM3_CH3, TIM3_CH4         |
| 引脚          | PB0 (CH3), PB1 (CH4)       |
| 电机映射      | PB0 → MOTOR_A, PB1 → MOTOR_B |

### 编码器定时器--TIM2/TIM4
| 参数          | 值/配置                      |
|---------------|-----------------------------|
| 定时器        | TIM2 (MOTOR_A), TIM4 (MOTOR_B) |
| 引脚          | PA0, PA1, PB6, PB7         |
| 通道映射      | **MOTOR_A (TIM2):**<br>PA0 → EA-A<br>PA1 → EA-B<br>**MOTOR_B (TIM4):**<br>PB6 → EB-A<br>PB7 → EB-B |

### 速度定时器
| 参数          | 值/配置      |
|---------------|-------------|
| 定时器        | TIM1        |
| 周期          | 20ms        |
| 功能          | 编码器值→转速转换 |

---

## 方向控制
| 引脚   | 功能  | 映射       |
|--------|-------|-----------|
| PA3    | AIN1  | MOTOR_A方向1 |
| PA4    | AIN2  | MOTOR_A方向2 |
| PA5    | BIN1  | MOTOR_B方向1 |
| PA6    | BIN2  | MOTOR_B方向2 |

---

## 电池检测
| 参数             | 值/配置       |
|------------------|--------------|
| ADC通道          | ADC1-Channel2 |
| 引脚             | PA2          |

---

## 电压电流检测（INA226）
| 引脚   | 功能  |
|--------|-------|
| PB13   | SDA   |
| PB12   | SCL   |

---

## OLED显示
| 引脚   | 功能  |
|--------|-------|
| PB5    | SCL   |
| PB8    | SDA   |
| PB9    | RES   |
| PB14   | DC    |
| PB15   | CS    |

---

## WS2812 LED 灯带
### 通讯颜色ID：
| 值 (case) | 颜色常量                | 颜色名称 | 颜色说明                     |
|----------|-------------------------|----------|-----------------------------|
| 0        | `WS2812_BLACK`         | 黑色     | RGB(0, 0, 0)                |
| 1        | `WS2812_RED`           | 红色     | RGB(255, 0, 0)              |
| 2        | `WS2812_GREEN`         | 绿色     | RGB(0, 255, 0)              |
| 3        | `WS2812_BLUE`          | 蓝色     | RGB(0, 0, 255)              |
| 4        | `WS2812_YELLOW`        | 黄色     | RGB(255, 255, 0)            |
| 5        | `WS2812_PURPLE`        | 紫色     | RGB(128, 0, 128)            |
| 6        | `WS2812_CYAN`          | 青色     | RGB(0, 255, 255)            |

## 串口传输
| 串口    | 引脚功能 | 引脚  |
|---------|----------|-------|
| USART1  | TX       | PA9   |
|         | RX       | PA10  |
| USART3  | TX       | PB10  |
|         | RX       | PB11  |


## 总连接表

| 模块              | 名称       | 对应引脚                 | 备注                |
| --------------- | -------- | -------------------- | ------------------- |
| PWM 定时器 - TIM3  | PWM\_A | PB0（CH3）             | 控制电机 A 的 PWM 信号输出   |
| PWM 定时器 - TIM3  | PWM\_B | PB1（CH4）             | 控制电机 B 的 PWM 信号输出   |
| 编码器定时器 - TIM2   | EA-A     | PA0                  | 电机 A 编码器 A 相输入      |
| 编码器定时器 - TIM2   | EA-B     | PA1                  | 电机 A 编码器 B 相输入      |
| 编码器定时器 - TIM4   | EB-A     | PB6                  | 电机 B 编码器 A 相输入      |
| 编码器定时器 - TIM4   | EB-B     | PB7                  | 电机 B 编码器 B 相输入      |
| 方向控制            | AIN1     | PA3                  | 控制电机 A 方向的引脚 1      |
| 方向控制            | AIN2     | PA4                  | 控制电机 A 方向的引脚 2      |
| 方向控制            | BIN1     | PA5                  | 控制电机 B 方向的引脚 1      |
| 方向控制            | BIN2     | PA6                  | 控制电机 B 方向的引脚 2      |
| 电池检测            | ADC  | PA2（ADC1 - CH2） | 用于检测电池电压            |
| 电压电流检测 - INA226 | SDA      | PB13                 | 与 INA226 通信的数据线     |
| 电压电流检测 - INA226 | SCL      | PB12                 | 与 INA226 通信的时钟线     |
| OLED 显示         | SCL      | PB5                  | OLED 显示屏时钟线         |
| OLED 显示         | SDA      | PB8                  | OLED 显示屏数据线         |
| OLED 显示         | RES      | PB9                  | OLED 显示屏复位引脚        |
| OLED 显示         | DC       | PB14                 | OLED 显示屏数据 / 命令控制引脚 |
| OLED 显示         | CS       | PB15                 | OLED 显示屏片选引脚        |
| WS2812 LED 灯带   | DAT      | PA8                   | 控制 WS2812 LED 灯带的引脚   |
| 串口传输 - USART1  | TX       | PA9                  | USART1 的发送引脚         |
| 串口传输 - USART1  | RX       | PA10                 | USART1 的接收引脚         |
| 按键输入           | KEY1     | PA7                  | 按键1的接收引脚        |
| 按键输入           | KEY2     | PA11                 | 按键2的接收引脚         |
| 按键输入           | KEY3     | PA12                 | 按键3的接收引脚         |
| 蜂鸣器             | BEEP     | PB3                  | 蜂鸣器的发送引脚         |

## 串口数据包格式
### 数据包格式：
| 字段名称            | 描述               |
|---------------------|--------------------|
| `LINK_FrameHeader1` | 帧头1              |
| `LINK_FrameHeader2` | 帧头2              |
| `LINK_CMD`          | 命令字段           |
| `LINK_DATA_H`       | 数据高字节         |
| `LINK_DATA_L`       | 数据低字节         |
| `LINK_CheckSum`     | 校验和字段         |
| `SUM`               | 字段总数（枚举值） |
### 发送命令：
| 常量名称                          | 值   | 描述                                      |
|-----------------------------------|------|------------------------------------------|
| `LINK_CMD_IN_NULL`                | 0x00 | Null command (no operation)              |
| `LINK_CMD_IN_SET_LED`             | 0x01 | 设置LED，低字节LED编号，高字节LED状态     |
| `LINK_CMD_IN_SET_MotorAPWM`       | 0x12 | 设置电机A PWM（仅手动模式有效）           |
| `LINK_CMD_IN_SET_MotorBPWM`       | 0x22 | 设置电机B PWM（仅手动模式有效）           |
| `LINK_CMD_IN_SET_MotorAGoalSpeed` | 0x13 | 设置电机A 目标速度（仅自动模式有效）      |
| `LINK_CMD_IN_SET_MotorBGoalSpeed` | 0x23 | 设置电机B 目标速度（仅自动模式有效）      |
| `LINK_CMD_IN_SET_MotorMode`       | 0x04 | 设置电机模式，1为手动模式，0为自动模式    |
| `LINK_CMD_IN_SET_Orientation`     | 0x05 | 设置模块安装方向，1为逆时针，0为顺时针    |
| `LINK_CMD_IN_SET_SwitchOLED`      | 0x06 | 设置OLED开关，1为开，0为关                |
| `LINK_CMD_IN_SET_MotorStatue`     | 0x07 | 设置电机状态，1为启动，0为停止            |
| `LINK_CMD_IN_ASK_ADC`             | 0x08 | 查询电池电量                              |
| `LINK_CMD_IN_ASK_MotorASpeed`     | 0x09 | 查询电机A速度                             |
| `LINK_CMD_IN_ASK_MotorBSpeed`     | 0x0A | 查询电机B速度                             |