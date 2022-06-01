/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/

#define LOG_TAG    "app_user_cmd"

#include "main.h"
#include "fatfs.h"
#include "usb_host.h"
#include "elog.h"
#include "elog_flash.h"
#include "bsp.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include "w25qxx.h"
#include "delay.h"
#include "sfud.h"
#include "easyflash.h"
#include "shell_port.h"
#include "cJSON.h"
#include "cJSON_Utils.h"


/* 系统重启 */
int reboot(int argc, char *agrv[])
{
	//printf("%dparameter(s)\r\n", argc);
    printf("agrv is: %s\r\n",*agrv);
	// for (char i = 1; i < argc; i++)
    // {
    //     //printf("%s\r\n", agrv[i]);
    // }
    HAL_NVIC_SystemReset();
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), reboot, reboot, reboot);

int write_log(int argc, char *agrv[])
{
    log_d("COMPILE_TIME: %s--%s###LOG_VERSION_NUM: %s###PROJECT_NAME: %s.\n", __DATE__, __TIME__, LOG_VERSION_NUM,PROJECT_NAME);
    /* write all buffered log to flash */
    elog_flash_flush();
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), write_log, write_log, write_log);

//读取最近的日志，指令后面需要带长度 read_recent_log 1024
int read_recent_log(int argc, char *agrv[])
{
    uint32_t size = 0;
    printf("agrv is: %s\r\n",*agrv);//打印出函数名“output_recent_log”
    printf("%s\r\n", agrv[1]);//打印第一个参数
    Str2Int((uint8_t*)agrv[1],&size);//字符串转数字
    elog_flash_output_recent(size);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), read_recent_log, read_recent_log, read_recent_log);


int read_all_log(int argc, char *agrv[])
{
    elog_flash_output_all();
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), read_all_log, read_all_log, read_all_log);


int clean_log(int argc, char *agrv[])
{
    elog_flash_clean();
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), clean_log, clean_log, clean_log);



