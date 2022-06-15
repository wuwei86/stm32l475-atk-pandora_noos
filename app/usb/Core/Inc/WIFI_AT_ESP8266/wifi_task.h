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

#ifndef	_WIFI_TASK_H_
#define	_WIFI_TASK_H_
#include "sys.h"

/*WIFI应用管理模块 ---------------------------------------------------------------*/
typedef struct 
{
    uint8_t connect_state;//链接状态
    char *ssid;//wifi热点
    char *password;//密码
}wifi_manager;

typedef enum
{
  WIFI_STATE_BREAK       = 0x00,
  WIFI_STATE_CONNECT    = 0x01
} WIFI_StateTypeDef;

/*接口声明 --------------------------------------------------------------------*/
void wifi_query_version(void);  

void wifi_run_cmd(const char *cmd);

void wifi_init(void);

void wifi_connect(const char *ssid, const char *password);

void wifi_task(void);

uint32_t AT_GetTick(void);

#endif