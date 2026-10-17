#include "Int_Motor.h"
#include "Int_CAN.h"
#include "Int_Encoder.h"
#include "gpio.h"
#include "stdio.h"
#include "tim.h"

// 点击参数
MotorAttr motor_attr;

/**
 * @brief 设置方向
 */
void __Int_Motor_Set_Dir(MotorDir dir)
{
  if (dir == MOTOR_DIR_TO_HAND) {
    HAL_GPIO_WritePin(MOTOR1_DIR_GPIO_Port, MOTOR1_DIR_Pin, GPIO_PIN_RESET);
  } else {
    HAL_GPIO_WritePin(MOTOR1_DIR_GPIO_Port, MOTOR1_DIR_Pin, GPIO_PIN_SET);
  }
  motor_attr.dir = dir;
}

/**
 * @brief 设置目标步数  按圈数设置
 */
void __Int_Motor_Set_Rev(uint8_t rev)
{
  if (rev == 0) {
    rev = 0;
  }
  if (rev > MAX_REV) {
    rev = MAX_REV;
  }
  motor_attr.step.target_step = BASE_STEP * rev;
  motor_attr.step.current_step = 0;

  // 设置速度
  // double vMax = BASE_STEP * MOTOR_SPEED_6X;
  // double vInit = motor_attr.speed.init_speed;
  // motor_attr.speed.target_speed = vMax;                    // 设置最大速度
  // motor_attr.speed.current_speed = vInit;                  // 当前速度为初始速度
  // motor_attr.speed.acc_speed = BASE_STEP * MOTOR_SPEED_2X; // 加速度
  // motor_attr.speed.dec_speed = BASE_STEP * MOTOR_SPEED_2X; // 减速度

  // // 计算加速步数，减速步数，
  // motor_attr.step.acc_step = (vMax * vMax - vInit * vInit) / (2 * motor_attr.speed.acc_speed);
  // motor_attr.step.dec_step = (vMax * vMax - vInit * vInit) / (2 * motor_attr.speed.dec_speed);
  // // 匀速步数
  // if (motor_attr.step.target_step >= (motor_attr.step.acc_step + motor_attr.step.dec_step)) {
  //   // 有匀速步数
  //   motor_attr.step.const_step = motor_attr.step.target_step - (motor_attr.step.acc_step + motor_attr.step.dec_step);
  // } else {
  //   // 没有匀速步数
  //   motor_attr.step.const_step = 0;
  //   // 需要重新计算加速步数和减速步数
  //   double temp = 2 * motor_attr.speed.acc_speed * motor_attr.speed.dec_speed * motor_attr.step.target_step;
  //   double vNewMax2 = vInit * vInit + temp / (motor_attr.speed.acc_speed + motor_attr.speed.dec_speed);
  //   motor_attr.step.acc_step = (vNewMax2 - vInit * vInit) / (2 * motor_attr.speed.acc_speed);
  //   motor_attr.step.dec_step = motor_attr.step.target_step - motor_attr.step.acc_step;
  // }

  printf(" target step = %d \n", motor_attr.step.target_step);
  // printf(" acc step = %d \n", motor_attr.step.acc_step);
  // printf(" const step = %d \n", motor_attr.step.const_step);
  // printf(" dec step = %d \n", motor_attr.step.dec_step);
}

/**
 * @brief 设置目标步数 按角度设置
 */
void __Int_Motor_Set_Angle(uint16_t angle) { motor_attr.step.target_step = angle * 1.0 * BASE_STEP / 360; }

/**
 * @brief 设置电机 速度
 */
void __Int_Motor_Set_Speed(uint16_t speed)
{
  uint16_t arr = 1000000 / speed;
  __HAL_TIM_SetAutoreload(&htim2, arr - 1);             // 设置重装载寄存器值
  __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, arr / 2); // 占空比
}

/**
 * @brief 是否零点位置
 *
 */
uint8_t __Int_Motor_Read_Homing_State(void) { return HAL_GPIO_ReadPin(X_ZERO_GPIO_Port, X_ZERO_Pin) == GPIO_PIN_RESET; }

/**
 * @brief 1圈1600步，1步0.225度， 移动距离8mm，
 *
 */
void Int_Motor_Init(void)
{
  motor_attr.state = MOTOR_STATE_INIT;
  // 初始速度
  motor_attr.speed.init_speed = BASE_STEP * MOTOR_SPEED_2X;

  // Int_Motor_Start_By_Homing();
}

/**
 * @brief 按圈移动
 *
 * @param dir 方向
 */
void Int_Motor_Start_By_Revolution(MotorDir dir)
{
  // printf("check1: %d\n", motor_attr.state);
  // 如果不是初始化，停止状态，不往下执行，
  if (motor_attr.state != MOTOR_STATE_INIT && motor_attr.state != MOTOR_STATE_STOP) {
    return;
  }
  // 判断绝对零点位置，当方向往手轮方向（绝对零点方向），不再移动，反向不拦截
  if (dir == MOTOR_DIR_TO_HAND && __Int_Motor_Read_Homing_State()) {
    return;
  }

  motor_attr.state = MOTOR_STATE_RUNNING_REVALUTION;
  // 设置方向
  __Int_Motor_Set_Dir(dir);
  // 设置圈数
  __Int_Motor_Set_Rev(3);
  // 设置速度，
  __Int_Motor_Set_Speed(motor_attr.speed.init_speed);
  // 开启使能
  HAL_GPIO_WritePin(MOTOR_ALL_EN_GPIO_Port, MOTOR_ALL_EN_Pin, GPIO_PIN_SET);

  // 启动编码器
  Int_Encoder_Start();
  HAL_TIM_Base_Start_IT(&htim3);

  // 启动定时器
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);

  printf("Int_Motor_Start_By_Revalution start\n");
}

/**
 * @brief 停止电机
 *
 */
void Int_Motor_Stop(void)
{
  // // 关闭定时器
  HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
  // 关闭使能
  HAL_GPIO_WritePin(MOTOR_ALL_EN_GPIO_Port, MOTOR_ALL_EN_Pin, GPIO_PIN_RESET);
  // 状态重置
  motor_attr.step.current_step = 0;
  motor_attr.state = MOTOR_STATE_STOP;
  printf("motor stop\n");
}

/**
 * @brief 编码器计时器校验步数
 *
 */
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//   if (htim->Instance == TIM3) {
//     // 获取编码器的值
//     uint16_t cnt = __HAL_TIM_GetCounter(&htim1);

//     // if (motor_attr.dir == MOTOR_DIR_TO_HAND) {
//     //   // 转换为实际电机的前进步数
//     //   uint16_t encoder_step = cnt * 1600 / 4096;

//     // if (encoder_step > motor_attr.step.current_step) {
//     //   if ((encoder_step - motor_attr.step.current_step) > 5) {
//     //     motor_attr.step.current_step = encoder_step;
//     //   }
//     // } else {
//     //   if ((motor_attr.step.current_step - encoder_step) > 5) {
//     //     motor_attr.step.current_step = encoder_step;
//     //   }
//     // }
//     //  printf("%d,%d,%d\n",cnt, encoder_step, motor_attr.step.current_step);
//     //     if (motor_attr.step.current_step >= motor_attr.step.target_step) {
//     //       Int_Motor_Stop();
//     //     }
//     //   } else {

//     //   }
//   }

// }

/**
 * @brief TIM2定时器 输出PWM，比较中断
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) {
    // 按圈数移动
    if (motor_attr.state == MOTOR_STATE_RUNNING_REVALUTION) {
      motor_attr.step.current_step++;
      if (motor_attr.step.current_step >= motor_attr.step.target_step) {
        Int_Motor_Stop();
      }
      // else {
      //   if (motor_attr.step.current_step < motor_attr.step.acc_step) {
      //     // 加速
      //     motor_attr.speed.current_speed += (motor_attr.speed.acc_speed / motor_attr.speed.current_speed);
      //     __Int_Motor_Set_Speed(motor_attr.speed.current_speed);
      //   } else {
      //     if (motor_attr.step.const_step == 0) {
      //       // 减速
      //       motor_attr.speed.current_speed -= (motor_attr.speed.dec_speed  / motor_attr.speed.current_speed);
      //       __Int_Motor_Set_Speed(motor_attr.speed.current_speed);
      //     } else {
      //       if (motor_attr.step.current_step > (motor_attr.step.acc_step + motor_attr.step.const_step)) {
      //         // 减速
      //         motor_attr.speed.current_speed -= (motor_attr.speed.dec_speed / motor_attr.speed.current_speed);
      //         __Int_Motor_Set_Speed(motor_attr.speed.current_speed);
      //       }
      //     }
      //   }
      //   // if (motor_attr.step.current_step % 1600 == 0) {
      //   //   printf("cur_speed %d\n", motor_attr.speed.current_speed);
      //   // }
      // }
    } else if (motor_attr.state == MOTOR_STATE_RUNNING_POINT) { // 点动
      // 按键3向电机移动抬起，和按键4向手轮移动抬起
      if (((HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_SET) && (motor_attr.dir == MOTOR_DIR_TO_MOTOR)) ||
          ((HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_SET) && (motor_attr.dir == MOTOR_DIR_TO_HAND))) {
        Int_Motor_Stop();
      }
    }
  }
}

// 编码器每转一圈的计数器
uint16_t encoder_1_z_count = 0;
/**
 * @brief X_ZERO_Pin 的中断回调函数，光电限位开关，低电平触发
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == X_ZERO_Pin) {
    printf("HAL_GPIO_EXTI_Callback\n");
    Int_Motor_Stop();
  } else if (GPIO_Pin == ENCODER_1_Z_Pin) {
    encoder_1_z_count++;
  }
}

/**
 * @brief 回零
 *
 */
void Int_Motor_Start_By_Homing(void)
{
  if (motor_attr.state != MOTOR_STATE_INIT && motor_attr.state != MOTOR_STATE_STOP) {
    return;
  }
  if (__Int_Motor_Read_Homing_State()) {
    return;
  }

  motor_attr.state = MOTOR_STATE_RUNNING_HOMING;
  __Int_Motor_Set_Dir(MOTOR_DIR_TO_HAND);
  __Int_Motor_Set_Speed(BASE_STEP * MOTOR_SPEED_4X);

  HAL_GPIO_WritePin(MOTOR_ALL_EN_GPIO_Port, MOTOR_ALL_EN_Pin, GPIO_PIN_SET);
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);

  printf("Int_Motor_Start_By_Homing start\n");
}

/**
 * @brief 点动启动，按键按下启动，抬起关闭
 *
 * @param dir 方向
 */
void Int_Motor_Start_By_Point(MotorDir dir)
{
  if (motor_attr.state != MOTOR_STATE_INIT && motor_attr.state != MOTOR_STATE_STOP) {
    return;
  }
  if (dir == MOTOR_DIR_TO_HAND && __Int_Motor_Read_Homing_State()) {
    return;
  }

  motor_attr.state = MOTOR_STATE_RUNNING_POINT;
  __Int_Motor_Set_Dir(dir);
  __Int_Motor_Set_Speed(BASE_STEP * MOTOR_SPEED_3X);

  HAL_GPIO_WritePin(MOTOR_ALL_EN_GPIO_Port, MOTOR_ALL_EN_Pin, GPIO_PIN_SET);
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);

  printf("Int_Motor_Start_By_Point start\n");
}

/**
 * @brief 按远程遥控指令移动
 *
 * @param dir 方向
 */
void __Int_Motor_Start_By_Remote(MotorDir dir, uint16_t distance, uint16_t speed)
{
  // 如果不是初始化，停止状态，不往下执行，
  if (motor_attr.state != MOTOR_STATE_INIT && motor_attr.state != MOTOR_STATE_STOP) {
    return;
  }
  // 判断绝对零点位置，当方向往手轮方向（绝对零点方向），不再移动，反向不拦截
  if (dir == MOTOR_DIR_TO_HAND && __Int_Motor_Read_Homing_State()) {
    return;
  }

  motor_attr.state = MOTOR_STATE_RUNNING_REVALUTION;
  // 设置方向
  __Int_Motor_Set_Dir(dir);
  // 设置圈数
  __Int_Motor_Set_Rev(distance / 8);
  // 设置速度，
  __Int_Motor_Set_Speed(speed);
  // 开启使能
  HAL_GPIO_WritePin(MOTOR_ALL_EN_GPIO_Port, MOTOR_ALL_EN_Pin, GPIO_PIN_SET);
  // 启动定时器
  HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);

  printf("Int_Motor_Start_By_Remote start\n");
}

/**
 * @brief 周期性刷新
 *
 */
void Int_Motor_Refresh(void)
{
  if (motor_attr.state == MOTOR_STATE_INIT) {
    CAN_RX_MESSAGE message[3] = {0}; // 一次性最大接收3条数据
    uint8_t message_size = 0;
    Int_CAN_ReceiveData(message, &message_size);
    // printf("message_size=%d\n", message_size);
    if (message_size > 0) {
      // 取1条消息
      uint8_t *data = message[0].data;
      uint16_t distance = (data[0] << 8) | data[1];
      uint16_t speed = (data[2] << 8) | data[3];
      __Int_Motor_Start_By_Remote(MOTOR_DIR_TO_MOTOR, distance, speed);
    }
  }

  if (motor_attr.state == MOTOR_STATE_RUNNING_REVALUTION) {
    uint8_t data[4] = {0};
    uint16_t distance = motor_attr.step.current_step * 8 / BASE_STEP;
    data[0] = distance >> 8;
    data[1] = distance & 0xFF;
    data[2] = 0;
    data[3] = 1;
    Int_CAN_SendData(data, 4);
  }

  if (motor_attr.state == MOTOR_STATE_STOP) {
    uint8_t data[4] = {0};
    uint16_t distance = motor_attr.step.target_step * 8 / BASE_STEP;
    data[0] = distance >> 8;
    data[1] = distance & 0xFF;
    data[2] = 0;
    data[3] = 2;
    Int_CAN_SendData(data, 4);
    motor_attr.state = MOTOR_STATE_INIT;
  }
}
