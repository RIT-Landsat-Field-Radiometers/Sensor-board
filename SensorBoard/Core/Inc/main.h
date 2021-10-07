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
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal.h"

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
#define TP13_Pin GPIO_PIN_13
#define TP13_GPIO_Port GPIOC
#define CAL_2_Pin GPIO_PIN_0
#define CAL_2_GPIO_Port GPIOC
#define CAL_1_Pin GPIO_PIN_1
#define CAL_1_GPIO_Port GPIOC
#define CAL_0_Pin GPIO_PIN_2
#define CAL_0_GPIO_Port GPIOC
#define CAL_CS_Pin GPIO_PIN_3
#define CAL_CS_GPIO_Port GPIOC
#define HEAT_PWM_Pin GPIO_PIN_0
#define HEAT_PWM_GPIO_Port GPIOA
#define CHILL_PWM_Pin GPIO_PIN_1
#define CHILL_PWM_GPIO_Port GPIOA
#define TP9_Pin GPIO_PIN_4
#define TP9_GPIO_Port GPIOC
#define ADC_CS_Pin GPIO_PIN_1
#define ADC_CS_GPIO_Port GPIOB
#define ADC_Start_Pin GPIO_PIN_8
#define ADC_Start_GPIO_Port GPIOE
#define ADC_RST_Pin GPIO_PIN_11
#define ADC_RST_GPIO_Port GPIOE
#define ADC_DRDY_Pin GPIO_PIN_14
#define ADC_DRDY_GPIO_Port GPIOE
#define TP33_Pin GPIO_PIN_15
#define TP33_GPIO_Port GPIOE
#define SDMMC1_CD_Pin GPIO_PIN_10
#define SDMMC1_CD_GPIO_Port GPIOB
#define TP22_Pin GPIO_PIN_12
#define TP22_GPIO_Port GPIOB
#define CALIB_MODE_Pin GPIO_PIN_13
#define CALIB_MODE_GPIO_Port GPIOB
#define TP23_Pin GPIO_PIN_15
#define TP23_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_8
#define LED_B_GPIO_Port GPIOD
#define LED_G_Pin GPIO_PIN_9
#define LED_G_GPIO_Port GPIOD
#define LED_R_Pin GPIO_PIN_10
#define LED_R_GPIO_Port GPIOD
#define CAN_MODE_Pin GPIO_PIN_8
#define CAN_MODE_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
