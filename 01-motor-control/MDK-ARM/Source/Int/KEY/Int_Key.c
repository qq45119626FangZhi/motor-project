#include "Int_Key.h"
#include "gpio.h"
#include "stdio.h"

// 按键按下的状态，低电平表示按下，工作模式上拉输入
#define KEY1_PRESS_STATE (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
#define KEY2_PRESS_STATE (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
#define KEY3_PRESS_STATE (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
#define KEY4_PRESS_STATE (HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_RESET)
#define KEY5_PRESS_STATE (HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin) == GPIO_PIN_RESET)

/**
 * @brief 动态扫描按键
 *
 */
KEY_TYPE Int_Key_Scan(void)
{
  // 低电平表示按下
  if (KEY1_PRESS_STATE || KEY2_PRESS_STATE || KEY3_PRESS_STATE || KEY4_PRESS_STATE || KEY5_PRESS_STATE) {
    HAL_Delay(10); // 消抖

    if (KEY1_PRESS_STATE) {
      //按键按下
      while (KEY1_PRESS_STATE);
      printf("key1\n");
      // 按键抬起
      return KEY1;
    }
    if (KEY2_PRESS_STATE) {
      while (KEY2_PRESS_STATE);
      printf("key2\n");
      return KEY2;
    }
    if (KEY5_PRESS_STATE) {
      while (KEY5_PRESS_STATE);
      printf("key5\n");
      return KEY5;
    }
    if (KEY3_PRESS_STATE) {
      // while (KEY3_PRESS_STATE);
      // printf("key3\n");
      return KEY3;
    }
    if (KEY4_PRESS_STATE) {
      // while (KEY4_PRESS_STATE);
      // printf("key4\n");
      return KEY4;
    }
  }
  return KEY_NONE;
}

void Int_Key_Scan1(void (*callback_down)(uint8_t), uint8_t dir, void (*callback_up)())
{
  if (KEY3_PRESS_STATE) {
    HAL_Delay(10);
    if (KEY3_PRESS_STATE) {
      callback_down(dir);
      while (KEY3_PRESS_STATE);
      callback_up();
    }
  }
}

void Int_Key_Scan2(void (*callback_down)(uint8_t), uint8_t dir, void (*callback_up)())
{
  if (KEY4_PRESS_STATE) {
    HAL_Delay(10);
    if (KEY4_PRESS_STATE) {
      callback_down(dir);
      while (KEY4_PRESS_STATE);
      callback_up();
    }
  }
}
