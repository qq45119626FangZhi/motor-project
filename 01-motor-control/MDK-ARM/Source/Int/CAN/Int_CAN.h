#ifndef __INT_CAN_H__
#define __INT_CAN_H__

#include "stm32f4xx_hal.h"
#include <stdio.h>

typedef struct{
  uint32_t id;
  uint32_t message_length;
  uint8_t data[9];
}CAN_RX_MESSAGE;

void Int_CAN_Init(void);
void Int_CAN_SendData(uint8_t *data, uint8_t length);
void Int_CAN_ReceiveData(CAN_RX_MESSAGE *message, uint8_t *mesage_length);


#endif /* __INT_CAN_H__ */
