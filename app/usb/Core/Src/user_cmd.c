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

/*************************************************************************************/
//easylogger easyflash测试指令 
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
    printf("%s\r\n", agrv[1]);//打印第一个参数"1024"
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



/*************************************************************************************/
//sd usb fat测试指令

//sd_card
//没有文件就创建
int sd_write_file(int argc, char *agrv[])
{
    FRESULT res;                                        
    uint32_t byteswriten;

    log_d("agrv1 is %s",agrv[1]);//参数1,文件名(例子：1:aa.txt)sd卡前面是1:，usb前面0:
    //log_d("agrv1 len is %d",strlen(agrv[1]));//参数1长度

    log_d("agrv2 is %s",agrv[2]);//参数2,写入的数据（字符串）
    //log_d("agrv2 len is %d",strlen(agrv[2]));//参数2长度

    //log_d("argc is %d",argc);//加上函数和参数，总计的个数
    
    res = f_open(&SDFile, (const TCHAR*)agrv[1], FA_CREATE_ALWAYS | FA_WRITE);
    if(res != FR_OK)
    {
        log_e("f_open is fail error code is: %d",res);
        res = f_close(&SDFile);
        log_d("f_close is fail error code is: %d",res);
    }
    else
    {
        res = f_write(&SDFile, agrv[2], strlen(agrv[1]), (void *)&byteswriten);
        if(res == FR_OK)
        {
            log_d("f_write is success### write data is: %s###byteswriten is: %d",agrv[2],byteswriten);
        }
        else
        {
            log_e("f_write is fail error code is: %d",res);
        }
        res = f_close(&SDFile);
        log_d("f_close is fail error code is: %d",res);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), sd_write_file, sd_write_file, sd_write_file);

//读取文件
int sd_read_file(int argc, char *agrv[])
{
    uint32_t size = 0;
    uint8_t readbuf[128] = {0};
    FRESULT res;                                        
    uint32_t bytesread = 0;//读指针的位置

    log_d("agrv1 is %s",agrv[1]);//参数1,文件名
    //log_d("agrv1 len is %d",strlen(agrv[1]));//参数1长度

    log_d("agrv2 is %s",agrv[2]);//参数2,读取文件的长度
    //log_d("agrv2 len is %d",strlen(agrv[2]));//参数2长度

    res = f_open(&SDFile, (const TCHAR*)agrv[1], FA_READ);
    if(res != FR_OK)
    {
        log_e("f_open is fail error code is: %d",res);
        res = f_close(&SDFile);
        log_d("f_close is fail error code is: %d",res);
    }
    else
    {
        Str2Int((uint8_t*)agrv[2],&size);//字符串转数字
        log_d("read size is: %d Pointer to number of bytes read is: %d",size,bytesread);
        f_lseek(&SDFile, 0);//读取文件的时候必须要加上这一句，否则报错
        res = f_read(&SDFile,readbuf,size,&bytesread);
        if(res == FR_OK)
        {
            log_d("readbuf is: %s",readbuf);
        }
        else
        {
            log_e("f_read is fail error code is: %d",res);
        }
        res = f_close(&SDFile);
        log_d("f_close is fail error code is: %d",res);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), sd_read_file, sd_read_file, sd_read_file);

//usb//由于lettershell的问题，写入的数据长度最多只能8
int usb_write_file(int argc, char *agrv[])
{
    FRESULT res;                                        
    uint32_t byteswriten;

    log_d("agrv1 is %s",agrv[1]);//参数1,文件名(例子：1:aa.txt)sd卡前面是1:，usb前面0:
    //log_d("agrv1 len is %d",strlen(agrv[1]));//参数1长度

    log_d("agrv2 is %s",agrv[2]);//参数2,写入的数据（字符串）
    //log_d("agrv2 len is %d",strlen(agrv[2]));//参数2长度

    //log_d("argc is %d",argc);//加上函数和参数，总计的个数
    
    res = f_open(&USBHFile, (const TCHAR*)agrv[1], FA_CREATE_ALWAYS | FA_WRITE);
    if(res != FR_OK)
    {
        log_e("f_open is fail error code is: %d",res);
        res = f_close(&USBHFile);
        log_d("f_close is fail error code is: %d",res);
    }
    else
    {
         res = f_write(&USBHFile, agrv[2], strlen(agrv[1]), (void *)&byteswriten);
        if(res == FR_OK)
        {
            log_d("f_write is success### write data is: %s###byteswriten is: %d",agrv[2],byteswriten);
        }
        else
        {
            log_e("f_write is fail error code is: %d",res);
        }
        res = f_close(&USBHFile);
        log_d("f_close is fail error code is: %d",res);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), usb_write_file, usb_write_file, usb_write_file);

//读取文件
int usb_read_file(int argc, char *agrv[])
{
    uint32_t size = 0;
    uint8_t readbuf[128] = {0};
    FRESULT res;                                        
    uint32_t bytesread;//读指针的位置

    log_d("agrv1 is %s",agrv[1]);//参数1,文件名
    //log_d("agrv1 len is %d",strlen(agrv[1]));//参数1长度

    log_d("agrv2 is %s",agrv[2]);//参数2,读取文件的长度
    //log_d("agrv2 len is %d",strlen(agrv[2]));//参数2长度

    Str2Int((uint8_t*)agrv[2],&size);//字符串转数字
    if(size > 128)
    {
        log_e("read size is too lager");
        return 0;
    }

    res = f_open(&USBHFile, (const TCHAR*)agrv[1], FA_READ);
    if(res != FR_OK)
    {
        log_e("f_open is fail error code is: %d",res);
        res = f_close(&USBHFile);
        log_d("f_close is fail error code is: %d",res);
    }
    else
    {
        log_d("read size is: %d Pointer to number of bytes read is: %d",size,bytesread);
        f_lseek(&USBHFile, 0);//读取文件的时候必须要加上这一句，否则报错
        res = f_read(&USBHFile,readbuf,size,&bytesread);
        if(res == FR_OK)
        {
            log_d("readbuf is: %s",readbuf);
        }
        else
        {
            log_e("f_read is fail error code is: %d",res);
        }
        res = f_close(&USBHFile);
        log_d("f_close is fail error code is: %d",res);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), usb_read_file, usb_read_file, usb_read_file);

//读取文件
int test_log_file(int argc, char *agrv[])
{
    log_d("##f_close is fail error code is: 1");
    log_d("###f_close is fail error code is: 2");
    log_d("###f_close is fail error code is: 3");
    log_d("##f_close is fail error code is: 4");
    log_d("f_close is fail error code is: 5");
    log_d("f_close is fail error code is: 6");
    log_d("f_close is fail error code is: 7");
    log_d("f_close is fail error code is: 8");
    log_d("f_close is fail error code is: 9");

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), test_log_file, test_log_file, test_log_file);
