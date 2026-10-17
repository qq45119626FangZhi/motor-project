#ifndef __INT_MQTT_H__
#define __INT_MQTT_H__

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "MQTTClient.h"

void Int_MQTT_Init(void);
void Int_MQTT_SendData(uint8_t *data);
void Int_MQTT_ReceiveData(void);

#endif /* __INT_MQTT_H__ */
