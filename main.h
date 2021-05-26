/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define R4_Pin GPIO_PIN_2
#define R4_GPIO_Port GPIOE
#define R3_Pin GPIO_PIN_3
#define R3_GPIO_Port GPIOE
#define R2_Pin GPIO_PIN_4
#define R2_GPIO_Port GPIOE
#define R1_Pin GPIO_PIN_5
#define R1_GPIO_Port GPIOE
#define R0_Pin GPIO_PIN_6
#define R0_GPIO_Port GPIOE
#define duoji1_Pin GPIO_PIN_2
#define duoji1_GPIO_Port GPIOA
#define duoji2_Pin GPIO_PIN_3
#define duoji2_GPIO_Port GPIOA
#define DI_Pin GPIO_PIN_4
#define DI_GPIO_Port GPIOA
#define DO_Pin GPIO_PIN_4
#define DO_GPIO_Port GPIOC
#define CS_Pin GPIO_PIN_0
#define CS_GPIO_Port GPIOB
#define CLK_Pin GPIO_PIN_7
#define CLK_GPIO_Port GPIOE
#define R6_Pin GPIO_PIN_12
#define R6_GPIO_Port GPIOB
#define R7_Pin GPIO_PIN_13
#define R7_GPIO_Port GPIOB
#define R8_Pin GPIO_PIN_14
#define R8_GPIO_Port GPIOB
#define R9_Pin GPIO_PIN_15
#define R9_GPIO_Port GPIOB
#define R5_Pin GPIO_PIN_8
#define R5_GPIO_Port GPIOD
#define duoji3_Pin GPIO_PIN_9
#define duoji3_GPIO_Port GPIOC
#define BF2_Pin GPIO_PIN_12
#define BF2_GPIO_Port GPIOA
#define AF1_Pin GPIO_PIN_10
#define AF1_GPIO_Port GPIOC
#define AF2_Pin GPIO_PIN_12
#define AF2_GPIO_Port GPIOC
#define AR1_Pin GPIO_PIN_1
#define AR1_GPIO_Port GPIOD
#define AR2_Pin GPIO_PIN_3
#define AR2_GPIO_Port GPIOD
#define BR2_Pin GPIO_PIN_5
#define BR2_GPIO_Port GPIOD
#define BR1_Pin GPIO_PIN_7
#define BR1_GPIO_Port GPIOD
#define BF1_Pin GPIO_PIN_5
#define BF1_GPIO_Port GPIOB
#define STBY_Pin GPIO_PIN_1
#define STBY_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
