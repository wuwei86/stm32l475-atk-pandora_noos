#include "stm32l4xx_hal.h"
#include "bsp.h"
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "portserial.h"
#include "led.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
      Error_Handler();
    }
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
      Error_Handler();
    }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{
  uint8_t tmp = 0;
  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  //注意：中断开启和关闭操作不能在HAL_UART_MspInit回调中调用，否则不生效
  //因为：HAL_UART_Init调用流程是HAL_UART_MspInit--->UART_SetConfig(配置串口参数)
  //如果在UART_SetConfig之前配置中断则无效，必须要等待UART_SetConfig后面配置中断使能
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */
}


//编写中断服务函数
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
	//由于调用一次中断，进入中断回调函数后，中断就结束了，所以还要开启中断
	//HAL_UART_Receive_IT(&huart1,rdata,sizeof(rdata));//使能接收中断
}


//编写中断服务函数
void USART3_IRQHandler(void)
{
  if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE))			// 接收非空中断标记被置位
  {
      __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_RXNE);			// 清除中断标记
      prvvUARTRxISR();										// 通知modbus有数据到达
  }

  if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TXE))				// 发送为空中断标记被置位
  {
      __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_TXE);			// 清除中断标记
      prvvUARTTxReadyISR();									// 通知modbus数据可以发松
  }
}


/* 中断回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* 判断是哪个串口触发的中断 */
    if(huart ->Instance == USART1)
    {
        extern Shell shell;
        extern uint8_t recv_buf;
        //printf("recv:%#x\r\n",recv_buf);
        //调用shell处理数据的接口
        shellHandler(&shell, recv_buf);
        //使能串口中断接收
        HAL_UART_Receive_IT(&huart1, (uint8_t*)&recv_buf, 1);
    }
    else if(huart ->Instance == USART2)
    {
     
    }
    else if(huart ->Instance == USART3)
    {
     
    }
}

/* 中断回调函数 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	extern UART_HandleTypeDef huart1;

    /* 判断是哪个串口触发的中断 */
    if(huart ->Instance == USART1)
    {
       
    }
    else if(huart ->Instance == USART3)
    {
      //prvvUARTTxReadyISR();	

    }
}

extern TIM_HandleTypeDef TIM3_Handler;
extern TIM_HandleTypeDef TIM4_Handler;

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

extern void prvvTIMERExpiredISR(void);

void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM4_Handler);
    // if(__HAL_TIM_GET_FLAG(&TIM4_Handler, TIM_FLAG_UPDATE))			// 更新中断标记被置位
    // {
    //     __HAL_TIM_CLEAR_FLAG(&TIM4_Handler, TIM_FLAG_UPDATE);		// 清除中断标记
    //     prvvTIMERExpiredISR();								// 通知modbus3.5个字符等待时间到
    // }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        LED_B_TogglePin;        //LED_B翻转
    }
    else if (htim->Instance == TIM4)
    {
        // if(__HAL_TIM_GET_FLAG(&TIM4_Handler, TIM_FLAG_UPDATE))			// 更新中断标记被置位
        // {
        //     __HAL_TIM_CLEAR_FLAG(&TIM4_Handler, TIM_FLAG_UPDATE);		// 清除中断标记
        //     prvvTIMERExpiredISR();								// 通知modbus3.5个字符等待时间到
        // }
        prvvTIMERExpiredISR();								// 通知modbus3.5个字符等待时间到
    }
}
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);

  /*Configure GPIO pins : PE7 PE8 PE9 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

int fputc(int ch, FILE *stream)
{
    /* 堵塞判断串口是否发送完成 */
    while((USART1->ISR & 0X40) == 0);

    /* 串口发送完成，将该字符发送 */
    USART1->TDR = (uint8_t) ch;

    return ch;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}


