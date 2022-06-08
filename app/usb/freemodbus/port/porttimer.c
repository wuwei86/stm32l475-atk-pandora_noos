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
 * File: $Id: porttimer.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "bsp.h"

TIM_HandleTypeDef TIM4_Handler;      //定时器句柄

#define MODBUS_TIM	TIM4_Handler



/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)//需要实现50us定时器
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    MODBUS_TIM.Instance = TIM4;
    MODBUS_TIM.Init.Prescaler = 4000 - 1;//80MHz/20kHz=4000
    MODBUS_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
    MODBUS_TIM.Init.Period = usTim1Timerout50us - 1;//wbPeriod;
    MODBUS_TIM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&MODBUS_TIM) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&MODBUS_TIM, &sClockSourceConfig) != HAL_OK)
    {
        return FALSE;
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&MODBUS_TIM, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_TIM_CLEAR_FLAG(&MODBUS_TIM, TIM_FLAG_UPDATE);// 先清除一下定时器的中断标记,防止使能中断后直接触发中断
    __HAL_TIM_ENABLE_IT(&MODBUS_TIM, TIM_IT_UPDATE);// 使能定时器更新中断
    return TRUE;
}

void vMBPortTimersEnable()
{
    __HAL_TIM_SET_COUNTER(&MODBUS_TIM, 0);
	HAL_TIM_Base_Start_IT(&MODBUS_TIM);
}

void vMBPortTimersDisable()
{
    HAL_TIM_Base_Stop_IT(&MODBUS_TIM);
}

void prvvTIMERExpiredISR(void)
{
    (void) pxMBPortCBTimerExpired();
}


