#include "Int_W5500.h"
#include "wizchip_conf.h"
#include "gpio.h"
#include "stdio.h"
#include "socket.h"

#define SOCKET_NUM 0
#define LOCAL_PORT 8888
#define SERVER_PORT 666

uint8_t GATEWAY[4] ={192,168,45,1};
uint8_t IP[4] ={192,168,45,148};
uint8_t SERVER_IP[4] ={192,168,45,48};
uint8_t SUB[4] ={255,255,255,0};
uint8_t MAC[6] = {110, 120, 130, 140, 150, 160}; 
 

/**
 * @brief W5500联网芯片-重置
 * 
 */
void __Int_W5500_Reset(void){
  HAL_GPIO_WritePin(W5500_RST_GPIO_Port,W5500_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(5);
  HAL_GPIO_WritePin(W5500_RST_GPIO_Port,W5500_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(5);
}

/**
 * @brief W5500以太网芯片-初始化
 * 
 */
void Int_W5500_Init(void) {
  //复位
  __Int_W5500_Reset();
  //注册回调函数
  wizchip_register_callback();

  setGAR(GATEWAY);//网关
  setSUBR(SUB);// 子网掩码
  setSIPR(IP);//ip
  setSHAR(MAC);// MAC地址

  HAL_Delay(3000);

  printf("W5500 Init Success!!! \n");
}

/**
 * @brief 发送数据
 */
void Int_W5500_SendData(uint8_t *data, uint16_t length) {

  while (1) {
    //获取寄存器状态
    uint8_t status = getSn_SR(SOCKET_NUM);
    if(status == SOCK_CLOSED){//是否为关闭状态
      //创建socket
      int8_t socket_status = socket(SOCKET_NUM, Sn_MR_TCP, LOCAL_PORT, 0);
      if(socket_status == SOCKET_NUM){
        printf("create socket success \n");
      }else{
         printf("create socket fail \n");
      }
    }
    if(status == SOCK_INIT){//是否初始化完成
      //创建socket
      int8_t socket_status = connect(SOCKET_NUM, SERVER_IP, SERVER_PORT);
      if(socket_status == SOCK_OK){
        printf("connect socket success \n");
      }else{
         printf("connect socket fail \n");
      }
    }
    if(status == SOCK_ESTABLISHED){//是否可以开始通讯传输数据
      break;
    }
    HAL_Delay(1000);
  }

  send(SOCKET_NUM, data, length);

  close(SOCKET_NUM);
}
