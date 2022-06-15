/******************************************************************************
 * @brief    wifi任务(AT-command演示, 使用的模组是M169WI-FI)
 *
 * Copyright (c) 2020, <morro_luo@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs: 
 * Date           Author       Notes 
 * 2021-01-20     Morro        初版
 * 2021-03-03     Morro        增加URC使用案例
 ******************************************************************************/
#include "at_chat.h"
#include "wifi_uart.h"
#include <stdio.h>
#include <stdbool.h>
#include "wifi_task.h"
#include "MultiTimer.h"

#define LOG_TAG    "app_wifi_task"


#include "elog.h"

#define WLAN_SSID "CMCC-g7cN"
#define WLAN_PASSWORD "su7s9cbx"
/* Private function prototypes -----------------------------------------------*/
void wifi_open(void);
void wifi_close(void);
static void at_error(void);
void wifi_ready_handler(char *recvbuf, int size);
void wifi_connected_handler(char *recvbuf, int size);
void wifi_disconnected_handler(char *recvbuf, int size);

/* Private variables ---------------------------------------------------------*/
/* 
 * @brief   wifi管理数据
 */
static wifi_manager wifi_data;

/* 
 * @brief   定义AT控制器
 */
static at_obj_t at;

/* 
 * @brief   wifi 数据接收缓冲区
 */
static unsigned char wifi_recvbuf[512];

/* 
 * @brief   wifi URC接收缓冲区
 */
static unsigned char wifi_urcbuf[128];

/* 
 * @brief   wifi URC表
 */
static const utc_item_t urc_table[] = {
    "ready",             wifi_ready_handler,
    "WIFI CONNECTED:",   wifi_connected_handler,
    "WIFI DISCONNECTED", wifi_disconnected_handler,
};

/* 
 * @brief   AT适配器
 */
static const at_adapter_t  at_adapter = {
    .write    = wifi_uart_write,
    .read     = wifi_uart_read,
    .error    = at_error,
    .utc_tbl  = (utc_item_t *)urc_table,
    .urc_buf  = wifi_urcbuf,
    .recv_buf = wifi_recvbuf,
    .urc_tbl_count = sizeof(urc_table) / sizeof(urc_table[0]),
    .urc_bufsize   = sizeof(wifi_urcbuf),
    .recv_bufsize = sizeof(wifi_recvbuf)
};

/* Private functions ---------------------------------------------------------*/

/* 
 * @brief   wifi开机就绪事件
 */
void wifi_ready_handler(char *recvbuf, int size)
{
    log_d("WIFI ready...\r\n");
}

/* 
 * @brief   wifi连接事件
 */
static void wifi_connected_handler(char *recvbuf, int size)
{
    log_d("WIFI connection detected...\r\n");
}
/* 
 * @brief   wifi断开连接事件
 */
static void wifi_disconnected_handler(char *recvbuf, int size)
{
    log_d("WIFI disconnect detected...\r\n");
}

/* 
 * @brief   打开wifi
 */
void wifi_open(void)
{
    //GPIO_SetBits(GPIOA, GPIO_Pin_4);
    log_d("wifi open\r\n");
}
/* 
 * @brief   关闭wifi
 */
void wifi_close(void)
{
    //GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    log_d("wifi close\r\n");
}


/* 
 * @brief   WIFI重启任务状态机
 * @return  true - 退出状态机, false - 保持状态机,
 */
static int wifi_reset_work(at_env_t *e)
{
    at_obj_t *a = (at_obj_t *)e->params;
    switch (e->state) {
    case 0:                                //关闭WIFI电源
        wifi_close();
        e->reset_timer(a);
        e->state++;
        break;
    case 1:
        if (e->is_timeout(a, 2000))       //延时等待2s
            e->state++;
        break;
    case 2:
        wifi_open();                       //重启启动wifi
        e->state++;
        break;
    case 3:
        if (e->is_timeout(a, 5000))       //大约延时等待5s至wifi启动
            return true;  
        break;
    }
    return false;
}
/* 
 * @brief   wifi 通信异常处理
 */
static void at_error(void)
{
    log_d("wifi AT communication error\r\n");
    //执行重启作业
    at_do_work(&at, wifi_reset_work, &at);        
}

/* 
 * @brief    初始化回调
 */
static void at_init_callbatk(at_response_t *r)
{    
    if (r->ret == AT_RET_OK ) {
        log_d("wifi Initialization successfully...\r\n");
        
        /* 查询版本号*/
        wifi_query_version();
        
    } else 
        log_d("wifi Initialization failure...\r\n");
}

/* 
 * @brief    wifi初始化命令表
 */
static const char *wifi_init_cmds[] = {
    "AT+GPIO_WR=1,1\r\n",
    "AT+GPIO_WR=2,0\r\n",
    "AT+GPIO_WR=3,1\r\n",
    NULL
};

static MultiTimer wifi_timer;
static MultiTimer at_timer;//给at_chat模块提供时钟
uint32_t at_tick = 0;

uint32_t AT_GetTick(void)
{
  return at_tick;
}

static void wifiATTimerCallback(MultiTimer* timer, void *userData)
{
	at_tick++;
    MultiTimerStart(timer, 1, wifiATTimerCallback, userData);
}

static void wifiTimerCallback(MultiTimer* timer, void *userData)
{
	wifi_task();
    MultiTimerStart(timer, 10, wifiTimerCallback, userData);
}

/* 
 * @brief    wifi初始化
 */
void wifi_init(void)
{
    wifi_uart_init(115200);
    at_obj_init(&at, &at_adapter);

    //初始化数据
    wifi_data.connect_state = WIFI_STATE_BREAK;
    wifi_data.ssid = WLAN_SSID;
    wifi_data.password = WLAN_PASSWORD;

         
    //启动WIFI
    at_do_work(&at, wifi_reset_work, &at);  

    MultiTimerStart(&at_timer, 1, wifiATTimerCallback, "1ms at timer"); 

    MultiTimerStart(&wifi_timer, 10, wifiTimerCallback, "10ms wifi task");


    //wifi_query_version(); 

    wifi_run_cmd("AT");

    //wifi_run_cmd("AT+CWJAP=\"CMCC-g7cN\",\"su7s9cbx\"");
    wifi_connect(WLAN_SSID,WLAN_PASSWORD);

    //查询ip地址
    wifi_run_cmd("AT+CIFSR");
    //初始化wifi
    //at_send_multiline(&at, at_init_callbatk, wifi_init_cmds);  
    
    //GPIO测试
    //at_send_singlline(&at, NULL, "AT+GPIO_TEST_EN=1\r\n");  
    
}



/* 
 * @brief    wifi任务(10ms 轮询1次)
 */
void wifi_task(void)
{
    at_poll_task(&at);
}


/** 非标准AT例子----------------------------------------------------------------
 *  以查询版本号为例:
 * ->  AT+VER\r\n
 * <-  VERSION:M169-YH01
 *  
 */

//方式1, 使用at_do_cmd接口

/* 
 * @brief    自定义AT发送器
 */
static void at_ver_sender(at_env_t *e)
{
    e->printf(&at, "AT+CWJAP=CMCC-g7cN,su7s9cbx");//后面不能带\r\n
}
/* 
 * @brief    版本查询回调
 */
static void query_version_callback(at_response_t *r)
{
    if (r->ret == AT_RET_OK ) {
        log_d("wifi version info : \r\n%s\r\n", r->recvbuf);
    } else 
        log_d("wifi version query failure...\r\n");
}

/* 填充AT命令*/
static const at_cmd_t at_cmd_ver = {
    at_ver_sender,                   //自定义AT发送器
    "OK",                           //接收匹配前缀(可以不是最前面，可以是最后面的字符)
    query_version_callback,          //查询回调
    1, 
    3000                          //重试次数及超时时间
};

/* 
 * @brief    执行版本查询命令
 */
void wifi_query_version(void)
{
    //at_do_cmd(&at, NULL, &at_cmd_ver);
    at_send_singlline(&at,query_version_callback,"AT+GMR");
}

/******************************************************************************/

/* 
 * @brief    自定义AT发送器
 */
static void at_connect_sender(at_env_t *e)
{
    char buf[64] = { 0 }; 
    char *cmd = "AT+CWJAP=";
    memset(buf, 0, 64);
    sprintf(buf, "%s\"%s\",\"%s\"", cmd, wifi_data.ssid, wifi_data.password);
    log_d("buf is: %s",buf);
    e->printf(&at, buf);//后面不能带\r\n
}
/* 
 * @brief    版本查询回调
 */
static void wifi_connect_callback(at_response_t *r)
{
    if (r->ret == AT_RET_OK ) {
        log_d("wifi version info : \r\n%s\r\n", r->recvbuf);
        wifi_data.connect_state = WIFI_STATE_CONNECT;
    } else 
        log_d("wifi version query failure...\r\n");
}

/* 填充AT命令*/
static const at_cmd_t at_cmd_connect = {
    at_connect_sender,                   //自定义AT发送器
    "OK",                           //接收匹配前缀(可以不是最前面，可以是最后面的字符)
    wifi_connect_callback,          //查询回调
    1, 
    8000                          //重试次数及超时时间
};


/* 
 * @brief    执行版本查询命令
 */
void wifi_connect(const char *ssid, const char *password)
{
    // char buf[64] = { 0 }; 
    // char *cmd = "AT+CWJAP=";
    // memset(buf, 0, 64);
    // sprintf(buf, "%s\"%s\",\"%s\"", cmd, ssid, password);
    // log_d("buf is: %s",buf);
    log_d("run wifi_connect ssid is: %s password is: %s",wifi_data.ssid,wifi_data.password);
    at_do_cmd(&at, NULL, &at_cmd_connect);
    //at_send_singlline(&at,wifi_connect_callback,buf);
}

/* 
 * @brief    版本查询回调
 */
static void cmd_callback(at_response_t *r)
{
    if (r->ret == AT_RET_OK ) {
        log_d("wifi cmd info : \r\n%s\r\n", r->recvbuf);
    } else 
        log_d("wifi cmd failure...\r\n");
}

/* 
 * @brief    执行命令
 */
void wifi_run_cmd(const char *cmd)
{
    log_d("wifi_run_cmd is: %s",cmd);
    at_send_singlline(&at,cmd_callback,cmd);
}
