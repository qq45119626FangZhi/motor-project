#include "App_Main.h"
#include "Int_CAN.h"
#include "Int_MQTT.h"
#include "cJSON.h"


typedef enum { TASK_PRIORITY_0, TASK_PRIORITY_1, TASK_PRIORITY_2, TASK_PRIORITY_3, TASK_PRIORITY_4 } TASK_PRIORITY;

void mqtt_task(void *arg);
void gateway_to_motor_task(void *arg);
void motor_to_gateway_task(void *arg);

SemaphoreHandle_t mqtt_semaphore_handle;
SemaphoreHandle_t can_semaphore_handle;

uint16_t distance = 0;
uint16_t speed = 0;

void App_Main(void)
{
  printf("motor gateway project start \n");

  Int_CAN_Init();

  // 创建信号量
  mqtt_semaphore_handle = xSemaphoreCreateBinary();
  can_semaphore_handle = xSemaphoreCreateBinary();

  // 创建任务
  xTaskCreate(mqtt_task, "mqtt_task", 512, NULL, TASK_PRIORITY_4, NULL);
  xTaskCreate(gateway_to_motor_task, "gateway_to_motor_task", 512, NULL, TASK_PRIORITY_4, NULL);
  xTaskCreate(motor_to_gateway_task, "motor_to_gateway_task", 512, NULL, TASK_PRIORITY_4, NULL);
  vTaskStartScheduler();
}

/**
 * @brief MQTT初始化启动
 */
void mqtt_task(void *arg)
{
  Int_MQTT_Init();

  while (1) {
    Int_MQTT_ReceiveData();
  }
}

/**
 * @brief 解析json数据
 */
void json_data_handler(char *json_string){
  cJSON *json_object = cJSON_Parse(json_string);
  distance = cJSON_GetObjectItem(json_object, "target_distance")->valueint;
  speed = cJSON_GetObjectItem(json_object, "max_speed")->valueint;
  cJSON_Delete(json_object);

  if(speed == 0 ||  distance == 0){
    printf("json_data_handler params is NULL \n");
    return;
  }
  //通知gateway_to_motor_task任务
  xSemaphoreGive(mqtt_semaphore_handle);
}


/**
 * @brief 网关接收到信号，获取数据，通过CAN，发送给电机
 *
 * @param arg
 */
void gateway_to_motor_task(void *arg)
{

  while (1) {
    BaseType_t state = xSemaphoreTake(mqtt_semaphore_handle, portMAX_DELAY);
    if (state == pdTRUE) {
      uint8_t data[4] = {0};
      data[0] = distance >> 8;
      data[1] = distance & 0xFF;
      data[2] = speed >> 8;
      data[3] = speed & 0xFF;
      // 发送数据
      Int_CAN_SendData(data, 4);
      vTaskDelay(200);

      // 通知 can 开始接收 电机返回的 电机数据
      xSemaphoreGive(can_semaphore_handle);
    }
  }
}

/**
 * @brief 向控制台发送数据
 * 
 * @param dis 
 * @param state 
 */
void __send_json_to_control(uint16_t dis, uint16_t state){
  cJSON *json_object = cJSON_CreateObject();
  cJSON_AddNumberToObject(json_object, "cur_distance", dis);
  cJSON_AddStringToObject(json_object, "motor_status", state==1?"on":"off");
  char *json_string = cJSON_PrintUnformatted(json_object);
  Int_MQTT_SendData((uint8_t *)json_string);

  cJSON_free(json_string);
  cJSON_Delete(json_object);
}

void motor_to_gateway_task(void *arg)
{
  while (1) {
    BaseType_t state = xSemaphoreTake(can_semaphore_handle, portMAX_DELAY);
    if (state == pdTRUE) {
      while (1) {
        //接收单击回传的数据，发送给控制台显示
        CAN_RX_MESSAGE message[3] = {0};
        uint8_t message_size = 8;
        Int_CAN_ReceiveData(message, &message_size);
        if (message_size > 0) {
          for (uint8_t i = 0; i < message_size; i++) {
            uint8_t *data = message[i].data;
            uint16_t dis = (data[0] << 8) | data[1];
            uint16_t state1 = (data[2] << 8) | data[3];
            if (state1 == 1) {
              printf("running: %d mm\n", dis);
              __send_json_to_control(dis, state1);
            }
            if (state1 == 2) {
              printf("stop: %d mm\n", dis);
              __send_json_to_control(dis, state1);
              goto end_while;
            }
          }
        }
      }
      end_while:;
    }
  }
}
