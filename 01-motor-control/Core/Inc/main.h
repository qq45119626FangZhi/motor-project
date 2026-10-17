/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOE
#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOE
#define KEY3_Pin GPIO_PIN_4
#define KEY3_GPIO_Port GPIOE
#define KEY4_Pin GPIO_PIN_5
#define KEY4_GPIO_Port GPIOE
#define KEY5_Pin GPIO_PIN_6
#define KEY5_GPIO_Port GPIOE
#define MOTOR1_DIR_Pin GPIO_PIN_4
#define MOTOR1_DIR_GPIO_Port GPIOA
#define MOTOR_ALL_EN_Pin GPIO_PIN_0
#define MOTOR_ALL_EN_GPIO_Port GPIOG
#define X_ZERO_Pin GPIO_PIN_2
#define X_ZERO_GPIO_Port GPIOG
#define X_ZERO_EXTI_IRQn EXTI2_IRQn
#define ENCODER_1_MODE_Pin GPIO_PIN_7
#define ENCODER_1_MODE_GPIO_Port GPIOG
#define ENCODER_1_Z_Pin GPIO_PIN_9
#define ENCODER_1_Z_GPIO_Port GPIOC
#define ENCODER_1_Z_EXTI_IRQn EXTI9_5_IRQn
#define ENCODER_1_EN_Pin GPIO_PIN_10
#define ENCODER_1_EN_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
