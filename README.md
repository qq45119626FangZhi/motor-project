# motor-project
搭建一套 上位机(Android/html) + STM32 网关 + STM32 电机控制器 三层分布式运动控制系统， 实现步进电机丝杆远程运动控制、位置闭环校验与状态监控。软件采用四层模块化分层架构开发。

项目名称：步进电机机电一体化控制系统
项目描述：整套系统由电机控制板、网关板、控制页面组成，实现远程控制步进电机丝杆直线运动，具备电机状态监测、运动参数配置、
        闭环位置校验功能。软件采用四层模块化架构：驱动层、公共层、硬件接口层、应用层。

项目01-motor-control和02-motor-control-canopen(使用canopen框架实现)，电机控制

1\下机位电机控制板（STM32F407ZGT6，HAL 库）
   - 驱动层：实现 USART、I2C、CAN、TIM (PWM)、GPIO 外设底层驱动；
   - 公共层：实现微秒延时、cJSON数据封装；
   - 硬件接口层：封装外设，向上提供编码器读取、CAN 通信、脉冲输出统一 API；
   - 应用层：完成加减速运动规划；定时器 PWM 输出脉冲驱动步进电机；I2C + 编码器外部中断采集位置做闭环校验；
          解析网关 CAN 指令，上报电机运行状态。

项目01-motor-gateway和02-motor-gateway-canopen(使用canopen框架实现)，电机网关   

2. 中间层网关板（STM32F103C8T6，HAL+FreeRTOS）
   - 驱动层：USART、CAN、SPI、W5500 底层驱动；
   - 公共层：微秒延时、cJSON 参数解析；
   - 硬件接口层：封装 CAN 总线、W5500 网络通信接口；
   - 应用层：创建 3 个同优先级任务，使用二值信号量做任务同步；通过 CAN 和电机板交互指令；
            SPI 驱动 W5500，基于 TCP 实现 MQTT 通信，与本地 MQTT 服务器交互，转发网页控制指令与电机状态。

项目Android-motor_console_stepper和H5-motor_console_stepper

3. 上机位android app控制页面：MQTT 客户端，下发电机控制指令，实时展示电机运行状态。
