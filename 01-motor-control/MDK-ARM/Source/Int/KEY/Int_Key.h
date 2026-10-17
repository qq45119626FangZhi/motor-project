#ifndef __INT_KEY_H__
#define __INT_KEY_H__

#include "stm32f4xx_hal.h"

typedef enum{
  KEY_NONE,
  KEY1,
  KEY2,
  KEY3,
  KEY3_DOWN,
  KEY3_UP,
  KEY4,
  KEY5,
}KEY_TYPE;

KEY_TYPE Int_Key_Scan(void);
void Int_Key_Scan1(void (* callback_down)(uint8_t), uint8_t dir, void (* callback_up)());
void Int_Key_Scan2(void (* callback_down)(uint8_t), uint8_t dir, void (* callback_up)());

#endif /* __INT_KEY_H__ */
