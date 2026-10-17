#ifndef __INT_W5500_H__
#define __INT_W5500_H__

#include "stm32f1xx_hal.h"

void Int_W5500_Init(void);
void Int_W5500_SendData(uint8_t *data, uint16_t length);


#endif /* __INT_W5500_H__ */
