#ifndef __INT_ENCODER_H__
#define __INT_ENCODER_H__

#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "tim.h"

void Int_Encoder_Init(void);

void Int_Encoder_Start(void);

void Int_Encoder_Stop(void);

#endif /* __INT_ENCODER_H__ */

