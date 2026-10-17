
#include "Int_CAN_OPEN.h"

CANopenNodeSTM32 node;
extern CO_t *CO;
#define OD_VELOCITY_REAL_VALUE 0x606C


// !CANopenNode初始化,来自CANopenNode官方readme
static void CANopenNode_init(void)
{
  node.CANHandle = &hcan1;
  node.HWInitFunction = MX_CAN1_Init;
  node.timerHandle = &htim11;
  node.desiredNodeID = 3;
  node.baudrate = 500;
  canopen_app_init(&node);
}

void Int_CANOPEN_Set_TPDO(OD_t *od, uint16_t index, int32_t value)
{
  ODR_t odRet;
  OD_entry_t *entry;

  entry = OD_find(od, index);
  if (entry == NULL) {
    printf("CANopen set speed error, entry not found\r\n");
    return;
  } else {
    printf("CANopen set speed entry found\r\n");
  }

  // 在字典中写入目标速度
  CO_LOCK_OD(CO->CANmodule);
  odRet = OD_set_i32(entry, 0x00, value, false);
  CO_UNLOCK_OD(CO->CANmodule);

  if (odRet != ODR_OK) {
    printf("CANopen set speed error, error code: %d\r\n", odRet);
    return;
  } else {
    printf("CANopen send speed OK\r\n");
  }
  // 通知协议栈发送TPDO(新增)
  CO_TPDOsendRequest(&CO->TPDO[0]);
}

void Int_CAN_OPEN_Init(void) {
  // !CANopenNode初始化
  CANopenNode_init();
  Int_CANOPEN_Set_TPDO(OD, OD_VELOCITY_REAL_VALUE, 0x20);
  HAL_Delay(100);
  while (1) {
    canopen_app_process();
    HAL_Delay(1);
  }
}

// !定时器溢出中断回调
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM11) {
    canopen_app_interrupt();
  }
}