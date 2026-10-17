#ifndef __INT_MOTOR_H__
#define __INT_MOTOR_H__

#include "stm32f4xx_hal.h"

// 1圈的步数，每步0.225度，直线距离8mm
#define BASE_STEP 1600
//最大圈速
#define MAX_REV 28

// 电机运动方向
typedef enum {
  MOTOR_DIR_TO_MOTOR, // 电机方向
  MOTOR_DIR_TO_HAND,  // 手轮方向
} MotorDir;

// 电机状态
typedef enum {
  MOTOR_STATE_INIT,               // 就绪
  MOTOR_STATE_RUNNING_REVALUTION, // 按键移动
  MOTOR_STATE_RUNNING_HOMING,     // 回零
  MOTOR_STATE_RUNNING_POINT,      // 点动
  MOTOR_STATE_STOP,               // 停止
} MotorState;

// 电机初始速度的倍数
typedef enum {
  MOTOR_SPEED_1X = 1, // 倍数
  MOTOR_SPEED_2X,
  MOTOR_SPEED_3X,
  MOTOR_SPEED_4X,
  MOTOR_SPEED_5X,
  MOTOR_SPEED_6X,
  MOTOR_SPEED_7X,
  MOTOR_SPEED_8X,
  MOTOR_SPEED_9X
} BaseSpeedMultiple;

// 电机步数
typedef struct {
  uint32_t current_step; // 当前步数
  uint32_t target_step;  // 目标步数
  uint32_t acc_step;  //加速步数
  uint32_t dec_step;  // 减速步数
  uint32_t const_step;  // 减速步数
} MotorStep;


// 电机速度
typedef struct {
  uint32_t init_speed;  // 初始速度
  uint32_t target_speed;  // 最大速度
  double current_speed; // 当前速度
  uint32_t acc_speed;  //加速步数
  uint32_t dec_speed;  // 减速步数
} MotorSpeed;

// 电机参数
typedef struct {
  MotorStep step;   // 步数
  MotorDir dir;     // 方向
  MotorState state; // 状态
  MotorSpeed speed; // 速度
} MotorAttr;

void Int_Motor_Init(void);

void Int_Motor_Start_By_Revolution(MotorDir dir);
    
void Int_Motor_Start_By_Homing(void);

void Int_Motor_Start_By_Point(MotorDir dir);

void Int_Motor_Stop(void);
void Int_Motor_Refresh(void);

#endif /* __INT_MOTOR_H__ */
