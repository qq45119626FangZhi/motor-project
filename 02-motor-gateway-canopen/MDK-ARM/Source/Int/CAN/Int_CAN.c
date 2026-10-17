#include "Int_CAN.h"
#include "can.h"

#define CAN_ID_MOTOR 1001
#define CAN_ID_GAtEWAY 1002

/**
 * @brief CAN通信初始化
 *
 */
void Int_CAN_Init(void)
{

  // 配置
  CAN_FilterTypeDef filter_config;
  filter_config.FilterActivation = CAN_FILTER_ENABLE;    // 使能
  filter_config.FilterBank = 0;                          // 0-13区
  filter_config.FilterFIFOAssignment = CAN_FILTER_FIFO0; // 0，使用邮箱编号
  filter_config.FilterIdHigh = CAN_ID_MOTOR << 5;            // 16位寄存器，使用完全匹配ID，高11位为地址位
  filter_config.FilterIdLow = 0;
  filter_config.FilterMaskIdHigh = 0; // 模糊标记匹配，此处不使用
  filter_config.FilterMaskIdLow = 0;
  filter_config.FilterMode = CAN_FILTERMODE_IDLIST;  // 标准帧
  filter_config.FilterScale = CAN_FILTERSCALE_32BIT; //
  // filter_config.SlaveStartFilterBank

  HAL_CAN_ConfigFilter(&hcan, &filter_config);

  // 启动
  HAL_CAN_Start(&hcan);
}

/**
 * @brief 发送数据
 *
 */
void Int_CAN_SendData(uint8_t *data, uint8_t length)
{

  while (1) {
    uint32_t tx_mail_free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);
    if (tx_mail_free_level > 0) {
      // 当邮箱空闲可发送数据
      break;
    }
  }

  CAN_TxHeaderTypeDef message_header;
  message_header.StdId = CAN_ID_GAtEWAY;
  message_header.DLC = length;
  message_header.IDE = CAN_ID_STD;   // 标准帧
  message_header.RTR = CAN_RTR_DATA; // 数据
  uint32_t tx_mailbox = 0;           // 发送邮箱位置
  HAL_CAN_AddTxMessage(&hcan, &message_header, data, &tx_mailbox);
  printf("tx_mailbox=%d\n", tx_mailbox);
}

/**
 * @brief 接收数据
 */
void Int_CAN_ReceiveData(CAN_RX_MESSAGE *message, uint8_t *mesage_length)
{
  while (1) {
    *mesage_length = HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0);
    if (*mesage_length > 0) {
      // 当邮箱空闲可发送数据
      break;
    }
  }

  CAN_RxHeaderTypeDef message_header;
  for (int i = 0; i < *mesage_length; i++) {
    HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &message_header, message[i].data);
    message[i].id = message_header.StdId;
    message[i].message_length = message_header.DLC;
  }
}
