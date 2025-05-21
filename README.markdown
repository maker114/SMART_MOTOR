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

| 电机   | 定时器 | 通道引脚 | 编码器信号 | 计数器位数 | 工作模式       |
|--------|--------|----------|------------|------------|----------------|
| MOTOR_A| TIM2   | PA0      | EA-A       | 16位       | TI12模式       |
|        |        | PA1      | EA-B       |            | (双通道编码器) |
| MOTOR_B| TIM4   | PB6      | EB-A       | 16位       | TI12模式       |
|        |        | PB7      | EB-B       |            | (双通道编码器) |

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

