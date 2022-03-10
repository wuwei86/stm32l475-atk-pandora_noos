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
#ifndef __BSP_H
#define __BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include <stdlib.h>

/* Private includes ----------------------------------------------------------*/


 #define printDebugMsg(format, ...) printf("File:[%s], Line:%d ,Function:[%s]: " format "\r\n",__FILE__, __LINE__,__FUNCTION__,##__VA_ARGS__);


/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
void SystemClock_Config(void);
void MX_USART1_UART_Init(void);
void MX_GPIO_Init(void);
void Error_Handler(void);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
