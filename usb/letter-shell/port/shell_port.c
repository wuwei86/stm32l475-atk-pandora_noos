/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "shell.h"
#include "stm32l4xx_hal.h"
#include "bsp.h"



//#define     SHELL_UART      huart1

Shell shell;
char shellBuffer[512];
uint8_t recv_buf = 0;
/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return unsigned short 写入实际长度
 */
unsigned short userShellWrite(char *data, unsigned short len)
{
    //return uart_write_bytes(SHELL_UART, (const char *)data, len);
    HAL_UART_Transmit(&huart1,(const uint8_t*)data,len,0xffff);
    return len;
}


/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return unsigned short 读取实际长度
 */
signed char userShellRead(char *data, unsigned short len)
{
    //return uart_read_bytes(SHELL_UART, (uint8_t *)data, len, portMAX_DELAY);
    //HAL_UART_Receive(&huart1,(uint8_t*)data,len,0xffff);
    return 0;
}


/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    // uart_config_t uartConfig = {
    //     .baud_rate = 115200,
    //     .data_bits = UART_WORDLENGTH_8B,
    //     .parity    = UART_PARITY_NONE,
    //     .stop_bits = UART_STOPBITS_1,
    //     .flow_ctrl = UART_HWCONTROL_NONE
    // };
    // uart_param_config(SHELL_UART, &uartConfig);
    // uart_driver_install(SHELL_UART, 256 * 2, 0, 0, NULL, 0);
    //使能串口中断接收
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&recv_buf, 1);
    shell.write = userShellWrite;
    //shell.read = userShellRead;
    shellInit(&shell, shellBuffer, 512);

    //xTaskCreate(shellTask, "shell", 2048, &shell, 10, NULL);
}

/* 中断回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	extern UART_HandleTypeDef huart1;

    /* 判断是哪个串口触发的中断 */
    if(huart ->Instance == USART1)
    {
        //printf("recv:%#x\r\n",recv_buf);
        //调用shell处理数据的接口
        shellHandler(&shell, recv_buf);
        //使能串口中断接收
        HAL_UART_Receive_IT(&huart1, (uint8_t*)&recv_buf, 1);
    }
}

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

