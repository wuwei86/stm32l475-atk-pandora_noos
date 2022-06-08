/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "bsp.h"
#include "portserial.h"
// #include "rtdevice.h"
// #include "board.h"

/* ----------------------- Static variables ---------------------------------*/
//ALIGN(RT_ALIGN_SIZE)
/* software simulation serial transmit IRQ handler thread stack */
// static uint8_t serial_soft_trans_irq_stack[512];
// /* software simulation serial transmit IRQ handler thread */
// static struct rt_thread thread_serial_soft_trans_irq;
// /* serial event */
// static struct rt_event event_serial;
// /* modbus slave serial device */
// static struct rt_serial_device *serial;

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)



/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity)
{
    /* USER CODE BEGIN USART3_Init 0 */

    /* USER CODE END USART3_Init 0 */

    /* USER CODE BEGIN USART3_Init 1 */

    /* USER CODE END USART3_Init 1 */
    huart3.Instance = USART3;
    huart3.Init.BaudRate = ulBaudRate;
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

    return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    ENTER_CRITICAL_SECTION();
   
    if(xRxEnable == TRUE) 
    {
        __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
        RS485_RX_EN;
    } 
    else 
    {
        __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
        RS485_TX_EN;
    }

    if(xTxEnable == TRUE) 
    {
        __HAL_UART_ENABLE_IT(&huart3, UART_IT_TC);
        RS485_TX_EN;
    } 
    else 
    {
        __HAL_UART_DISABLE_IT(&huart3, UART_IT_TC);
        RS485_RX_EN;
    }

    EXIT_CRITICAL_SECTION();
}

void vMBPortClose(void)
{
    HAL_UART_MspDeInit(&huart3);
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    //serial->parent.write(&(serial->parent), 0, &ucByte, 1);
    //while(huart3.gState != HAL_UART_STATE_READY);
    HAL_UART_Transmit(&huart3,(const uint8_t *)&ucByte, 1,0xffff);
    //USART3->TDR = ucByte;
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
     static uint8_t temp = 0;
	// HAL_UART_Receive_IT(&huart3, &temp, 1);
	
    //*pucByte = (USART3->RDR & (uint16_t)0x00FF);
    HAL_UART_Receive(&huart3, &temp, 1,0xffff);
    *pucByte = temp;
    return TRUE;
}

/*
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
    pxMBFrameCBTransmitterEmpty();
}

/*
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
    pxMBFrameCBByteReceived();
}


