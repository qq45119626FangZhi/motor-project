#include "Int_MQTT.h"
#include "Int_W5500.h"


MQTTClient mqtt_client;
Network mqtt_network;

#define BUFFER_SIZE 1024
uint8_t send_buffer[BUFFER_SIZE] = {0};
uint8_t read_buffer[BUFFER_SIZE] = {0};

uint8_t MQTT_SERVER_IP[4] = {192, 168, 45, 48};
#define MQTT_SERVER_PORT 1883
#define SOCKET_NUM 0
#define MQTT_RECEIVE_TOPIC_NAME "console_to_gateway"
#define MQTT_SEND_TOPIC_NAME "gateway_to_console"



void mqtt_message_callback(MessageData* data);

/**
 * @brief MQTT初始化
 *
 */
void Int_MQTT_Init(void)
{
  // 初始化W5500
  Int_W5500_Init();

  // 初始化网络
  NewNetwork(&mqtt_network, SOCKET_NUM);
  int state = ConnectNetwork(&mqtt_network, MQTT_SERVER_IP, MQTT_SERVER_PORT);
  if (state == SOCK_ERROR) {
    printf("ConnectNetwork fail \n");
    return;
  }
  printf("ConnectNetwork success \n");

  // 创建网络客户端
  MQTTClientInit(&mqtt_client, &mqtt_network, 1000, send_buffer, BUFFER_SIZE, read_buffer, BUFFER_SIZE);
  // 连接网络
  MQTTPacket_connectData options = MQTTPacket_connectData_initializer;
  options.clientID.cstring = "mqtt_fz"; // 客户端id编号
  options.keepAliveInterval = 0;      // 保活周期，0表示不设置
  options.willFlag = 0;               // 遗嘱，退出连接，通知的信号
  state = MQTTConnect(&mqtt_client, &options);
  if (state == FAILURE) {
    printf("MQTTConnect fail \n");
    return;
  }
  printf("MQTTConnect success \n");

  // 订阅主题
  state = MQTTSubscribe(&mqtt_client, MQTT_RECEIVE_TOPIC_NAME, QOS0, mqtt_message_callback);
  if (state == FAILURE) {
    printf("MQTTSubscribe fail \n");
    return;
  }
   printf("MQTTSubscribe success \n");
}

/**
 * @brief 发送数据
 *
 */
void Int_MQTT_SendData(uint8_t *data)
{
  MQTTMessage message;
  message.payload = data;
  message.payloadlen = strlen((char *)data);
  message.qos = QOS0;
  message.dup = 0;
  message.retained = 0;
  int state = MQTTPublish(&mqtt_client, MQTT_SEND_TOPIC_NAME, &message);
  if (state == FAILURE) {
    printf("MQTTPublish fail %s \n", data);
    return;
  }
  //  printf("MQTTPublish success \n");
}

/**
 * @brief 接收数据
 *
 */
void Int_MQTT_ReceiveData(void) { MQTTYield(&mqtt_client, 1000); }

/**
 * @brief 若实现函数，由其它文件实现具体json解析
 * 
 * @param json_string 
 */
__weak void json_data_handler(char *json_string){

}

/**
 * @brief mqtt接收消息，回调函数
 * 
 * @param data 
 */
void mqtt_message_callback(MessageData* data) {
    //printf("receive mqtt message : %s \n", data->message->payload);
    json_data_handler(data->message->payload);
}

