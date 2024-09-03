/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cdc_io.h"
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
void delay_us(uint16_t us);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define INTERNAL_LED_Pin GPIO_PIN_13
#define INTERNAL_LED_GPIO_Port GPIOC
#define MOT_DIR_Pin GPIO_PIN_0
#define MOT_DIR_GPIO_Port GPIOA
#define MOT_STP_Pin GPIO_PIN_1
#define MOT_STP_GPIO_Port GPIOA
#define MOT_SLP_Pin GPIO_PIN_2
#define MOT_SLP_GPIO_Port GPIOA
#define MOT_RST_Pin GPIO_PIN_3
#define MOT_RST_GPIO_Port GPIOA
#define MOT_MS3_Pin GPIO_PIN_4
#define MOT_MS3_GPIO_Port GPIOA
#define MOT_MS2_Pin GPIO_PIN_5
#define MOT_MS2_GPIO_Port GPIOA
#define MOT_MS1_Pin GPIO_PIN_6
#define MOT_MS1_GPIO_Port GPIOA
#define MOT_EN_Pin GPIO_PIN_7
#define MOT_EN_GPIO_Port GPIOA
#define PAPER_SENS_Pin GPIO_PIN_1
#define PAPER_SENS_GPIO_Port GPIOB
#define PRNT_CLK_Pin GPIO_PIN_4
#define PRNT_CLK_GPIO_Port GPIOB
#define PRNT_LAT_Pin GPIO_PIN_5
#define PRNT_LAT_GPIO_Port GPIOB
#define PRNT_STB1_Pin GPIO_PIN_6
#define PRNT_STB1_GPIO_Port GPIOB
#define PRNT_STB3_Pin GPIO_PIN_7
#define PRNT_STB3_GPIO_Port GPIOB
#define PRNT_STB2_Pin GPIO_PIN_8
#define PRNT_STB2_GPIO_Port GPIOB
#define PRNT_DAT_Pin GPIO_PIN_9
#define PRNT_DAT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
