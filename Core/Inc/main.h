/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

#include "stm32f1xx_ll_crc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_dma.h"

#include "stm32f1xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} ds_time_s;

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
#define NB_nRST_Pin GPIO_PIN_13
#define NB_nRST_GPIO_Port GPIOC
#define ALARM_LED_Pin GPIO_PIN_0
#define ALARM_LED_GPIO_Port GPIOA
#define SP706_WDI_Pin GPIO_PIN_1
#define SP706_WDI_GPIO_Port GPIOA
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define SCL_3_Pin GPIO_PIN_0
#define SCL_3_GPIO_Port GPIOB
#define SDA_3_Pin GPIO_PIN_1
#define SDA_3_GPIO_Port GPIOB
#define DS1302_CLK_Pin GPIO_PIN_13
#define DS1302_CLK_GPIO_Port GPIOB
#define DS1302_DAT_Pin GPIO_PIN_14
#define DS1302_DAT_GPIO_Port GPIOB
#define DS1302_OE_Pin GPIO_PIN_15
#define DS1302_OE_GPIO_Port GPIOB
#define SYSTEM_LED_Pin GPIO_PIN_15
#define SYSTEM_LED_GPIO_Port GPIOA
#define SCL_2_Pin GPIO_PIN_4
#define SCL_2_GPIO_Port GPIOB
#define SDA_2_Pin GPIO_PIN_5
#define SDA_2_GPIO_Port GPIOB
#define SCL_1_Pin GPIO_PIN_6
#define SCL_1_GPIO_Port GPIOB
#define SDA_1_Pin GPIO_PIN_7
#define SDA_1_GPIO_Port GPIOB
#define L3G_SCL_Pin GPIO_PIN_8
#define L3G_SCL_GPIO_Port GPIOB
#define L3G_SDA_Pin GPIO_PIN_9
#define L3G_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
