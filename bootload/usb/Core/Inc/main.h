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
#include "stm32l4xx_hal.h"

#define LOG_VERSION_NUM                "1.0.0"
#define COMPILE_TIME                    __DATE__","__TIME__
#define PROJECT_NAME                   "stm32l475-atk-pandora_noos_boot"


#define LOG_PROJECT_VERSION_MSG         "\r\n\r\n\r\n" \
                                        "/***********************************/" \
                                        "\r\n**"PROJECT_NAME"\r\n" \
                                        "**Build:       "__DATE__" "__TIME__"\r\n" \
                                        "**Version:     "LOG_VERSION_NUM"\r\n" \
                                        "**Copyright:   (c) 2022 by wuwei\r\n" \
                                        "/***********************************/" \
                                        "\r\n\r\n\r\n" \

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
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
