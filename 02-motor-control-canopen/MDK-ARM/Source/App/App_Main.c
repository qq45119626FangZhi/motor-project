#include "App_Main.h"
#include "Com_DWT.h"
#include "Int_CAN.h"
#include "Int_Encoder.h"
#include "Int_Key.h"
#include "Int_Motor.h"
#include "Int_CAN_OPEN.h"

extern uint16_t encoder_1_z_count;

void test_encoder(void)
{


    // Com_DWT_Init();
  // Int_Encoder_Init();
  // Int_Motor_Init();
  // Int_Motor_Start_By_Revolution(MOTOR_DIR_TO_HAND);

  // while (1) {
  //   printf("ab count = %d \n", __HAL_TIM_GetCounter(&htim1));
  //   printf("z count = %d \n", encoder_1_z_count);

  //   HAL_Delay(1000);
  // }
}

/**
 * @brief 程序入口
 *
 */
void App_main(void)
{
  printf("motor App_main start \n");
  // test_encoder();
  Int_CAN_OPEN_Init();


  // Int_CAN_Init();
  // //初始化电机
  // Int_Motor_Init();
  // KEY_TYPE kt = KEY_NONE;
  // while (1) {
  //   // 动态扫描按键
  //   kt = Int_Key_Scan();
  //   if (kt != KEY_NONE) {
  //     if (kt == KEY1) {
  //       Int_Motor_Start_By_Revolution(MOTOR_DIR_TO_MOTOR);
  //     }
  //     if (kt == KEY2) {
  //       Int_Motor_Start_By_Revolution(MOTOR_DIR_TO_HAND);
  //     }
  //     if (kt == KEY5) {
  //       //回零
  //       Int_Motor_Start_By_Homing();
  //     }
  //     if (kt == KEY3) {
  //       Int_Motor_Start_By_Point(MOTOR_DIR_TO_MOTOR);
  //     }
  //     if (kt == KEY4) {
  //       Int_Motor_Start_By_Point(MOTOR_DIR_TO_HAND);
  //     }
  //     kt = KEY_NONE;
  //   }
  //   Int_Motor_Refresh();
  //   HAL_Delay(100);
  // }
}
