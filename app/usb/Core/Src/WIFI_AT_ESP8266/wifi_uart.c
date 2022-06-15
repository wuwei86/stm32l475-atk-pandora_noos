/******************************************************************************
 * @brief    wifi串口通信驱动
 *
 * Copyright (c) 2021, <morro_luo@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs: 
 * Date           Author       Notes 
 * 2021-01-20     Morro        初始版本
 ******************************************************************************/
#include "stm32l4xx_hal.h"
#include "bsp.h"
#include "ringbuffer.h"
#include "wifi_uart.h"
#include <stdio.h>
#include <stdlib.h>
#include "sys.h"
#include "stm32l4xx_hal_uart.h"

#define LOG_TAG    "app_wifi_uart"


#include "elog.h"

#if (TTY_RXBUF_SIZE & (TTY_RXBUF_SIZE - 1)) != 0 
    #error "TTY_RXBUF_SIZE must be power of 2!"
#endif

#if (TTY_TXBUF_SIZE & (TTY_TXBUF_SIZE - 1)) != 0 
    #error "TTY_RXBUF_SIZE must be power of 2!"
#endif

static uint8_t rxbuf[WIFI_RXBUF_SIZE];         /*接收缓冲区 */
static uint8_t txbuf[WIFI_TXBUF_SIZE];         /*发送缓冲区 */
static ring_buf_t rbsend, rbrecv;              /*收发缓冲区管理*/

/*
 * @brief	    wifi串口初始化
 * @param[in]   baudrate - 波特率
 * @return 	    none
 */
void wifi_uart_init(int baud_rate)
{
    ring_buf_init(&rbsend, txbuf, sizeof(txbuf));/*初始化环形缓冲区 */
    ring_buf_init(&rbrecv, rxbuf, sizeof(rxbuf)); 
    
    MX_USART2_UART_Init();
}

/*
 * @brief	    向串口发送缓冲区内写入数据并启动发送
 * @param[in]   buf       -  数据缓存
 * @param[in]   len       -  数据长度
 * @return 	    实际写入长度(如果此时缓冲区满,则返回len)
 */
unsigned int wifi_uart_write(const void *buf, unsigned int len)
{   
    unsigned int ret;
    ret = ring_buf_put(&rbsend, (unsigned char *)buf, len); 
    //log_d("run wifi_uart_write"); 
    //USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
    
    return ret; 
}

/*
 * @brief	    读取串口接收缓冲区的数据
 * @param[in]   buf       -  数据缓存
 * @param[in]   len       -  数据长度
 * @return 	    (实际读取长度)如果接收缓冲区的有效数据大于len则返回len否则返回缓冲
 *              区有效数据的长度
 */
unsigned int wifi_uart_read(void *buf, unsigned int len)
{
    return ring_buf_get(&rbrecv, (unsigned char *)buf, len);
}

/*接收缓冲区空*/
bool wifi_uart_rx_empty(void)
{
    return ring_buf_len(&rbrecv) == 0;
}

/*
 * @brief	    串口2收发中断
 * @param[in]   none
 * @return 	    none
 */
void USART2_IRQHandler(void)
{
  static uint32_t revlen = 0;
  uint8_t data;

  if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE))			// 接收非空中断标记被置位
  {
    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);			// 清除中断标记
    HAL_UART_Receive(&huart2, &data, 1,0xffff);
    ring_buf_put(&rbrecv, &data, 1);           /*将数据放入接收缓冲区*/ 
    __nop();
  }

  if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE))				// 发送为空中断标记被置位
  {
    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TXE);			// 清除中断标记
    if (ring_buf_get(&rbsend, &data, 1))                      /*从缓冲区中取出数据---*/
    {
        //USART_SendData(USART3, data);  
        HAL_UART_Transmit(&huart2,(const uint8_t*)&data, 1, 0xffff);
    }        
    else
    {
        //USART_ITConfig(USART3, USART_IT_TXE, DISABLE);   
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE); 
    }  
  }
}
