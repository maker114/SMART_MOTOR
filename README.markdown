# README

## 定时器配置
| 定时器名 | 功能           | 通道号     | 引脚                     |
|:---------|:---------------|:-----------|:-------------------------|
| TIM1     | 速度计算定时器 | 无         | 无（内部定时器）         |
| TIM2     | 电机A编码器计数| CH1, CH2   | PA0 (CH1), PA1 (CH2)     |
| TIM3     | 电机PWM驱动    | CH3, CH4   | PB0 (CH3), PB1 (CH4)     |
| TIM4     | 电机B编码器计数| CH1, CH2   | PB6 (CH1), PB7 (CH2)     |

## WS2812 颜色映射
  | 值 | 常量              | 颜色   | RGB值           |
  |----|-------------------|--------|-----------------|
  | 0  | `WS2812_BLACK`   | 黑色   | (0, 0, 0)       |
  | 1  | `WS2812_RED`     | 红色   | (255, 0, 0)     |
  | 2  | `WS2812_GREEN`   | 绿色   | (0, 255, 0)     |
  | 3  | `WS2812_BLUE`    | 蓝色   | (0, 0, 255)     |
  | 4  | `WS2812_YELLOW`  | 黄色   | (255, 255, 0)   |
  | 5  | `WS2812_PURPLE`  | 紫色   | (128, 0, 128)   |
  | 6  | `WS2812_CYAN`    | 青色   | (0, 255, 255)   |

## 电机宏定义部分
| 分类     | 宏定义名称                    | 值   | 描述            |
| :----- | :----------------------- | :-- | :------------ |
| 积分限幅   | `MOTORA_I_MAX`           | 自定义 | 电机A积分限幅       |
| 积分限幅   | `MOTORB_I_MAX`           | 自定义 | 电机B积分限幅       |
| PID参数  | `MOTORA_KP`              | 自定义 | 电机A PID-KP值   |
| PID参数  | `MOTORA_KI`              | 自定义 | 电机A PID-KI值   |
| PID参数  | `MOTORA_KD`              | 自定义 | 电机A PID-KD值   |
| PID参数  | `MOTORB_KP`              | 自定义 | 电机B PID-KP值   |
| PID参数  | `MOTORB_KI`              | 自定义 | 电机B PID-KI值   |
| PID参数  | `MOTORB_KD`              | 自定义 | 电机B PID-KD值   |
| 电机编号   | `MOTOR_A`                | 0   | 电机A编号         |
| 电机编号   | `MOTOR_B`                | 1   | 电机B编号         |
| 运行状态编号 | `MOTOR_STOP`             | 0   | 电机停止状态编号      |
| 运行状态编号 | `MOTOR_FORWARD`          | 1   | 电机正向运行状态编号    |
| 运行状态编号 | `MOTOR_BACKWARD`         | 2   | 电机反向运行状态编号    |
| 安装方向   | `MOTOR_Clockwise`        | 0   | 顺时针旋转为正向      |
| 安装方向   | `MOTOR_Anticlockwise`    | 1   | 逆时针旋转为正向      |
| 操作模式   | `MOTOR_AUTO`             | 0   | 自动模式（使用内部PID） |
| 操作模式   | `MOTOR_MANUAL`           | 1   | 手动模式（使用外部PID） |
| PWM限幅  | `MOTOR_PWM_MAX`          | 自定义 | PWM最大限幅（补偿后）  |
| PWM限幅  | `MOTOR_PWM_MIN`          | 自定义 | PWM最小限幅（补偿后）  |
| PWM限幅  | `MOTOR_PWM_Compensate_A` | 自定义 | A通道PWM补偿值     |
| PWM限幅  | `MOTOR_PWM_Compensate_B` | 自定义 | B通道PWM补偿值     |
| 引脚定义   | `Direction_TIM_A`        | 自定义 | A通道引脚时钟       |
| 引脚定义   | `Direction_Group_A`      | 自定义 | A通道引脚组        |
| 引脚定义   | `Direction_AIN1`         | 自定义 | A通道方向引脚1      |
| 引脚定义   | `Direction_AIN2`         | 自定义 | A通道方向引脚2      |
| 引脚定义   | `Direction_TIM_B`        | 自定义 | B通道引脚时钟       |
| 引脚定义   | `Direction_Group_B`      | 自定义 | B通道引脚组        |
| 引脚定义   | `Direction_BIN1`         | 自定义 | B通道方向引脚1      |
| 引脚定义   | `Direction_BIN2`         | 自定义 | B通道方向引脚2      |



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