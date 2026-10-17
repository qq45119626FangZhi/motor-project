
#include "Int_CAN_OPEN.h"

CANopenNodeSTM32 node;

void Int_CAN_OPEN_Init(void) {

}

void canopen_task(void *parameter) {

  node.CANHandle = &hcan;
  node.HWInitFunction = MX_CAN_Init;
  node.timerHandle = &htim1;
  node.desiredNodeID = 2;
  node.baudrate = 500;
  canopen_app_init(&node);

  while (1)
  {
    canopen_app_process();

    static uint32_t last_tick = 0;
    if (HAL_GetTick() - last_tick >= 1000)
    {
        last_tick = HAL_GetTick();
        printf("0x%02x\n", OD_RAM.x606C_velocityRealValue);
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

// !定时器溢出中断回调
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) {
    canopen_app_interrupt();
  }
}
