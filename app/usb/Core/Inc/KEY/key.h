#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	本程序只供学习使用，未经作者许可，不得用于其它任何用途
 *	ALIENTEK Pandora STM32L475 IOT开发板
 *	KEY驱动代码
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2018/10/27
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	初始版本
 *	******************************************************************************/

/*
	KEY0 	- PD10
	KEY1 	- PD9
	KEY2 	- PD8
	WK_UP 	- PC13
*/
#define KEY0        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_10)
#define KEY1        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9)
#define KEY2        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8)
#define WK_UP       HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)

#define KEY0_PRES 		1
#define KEY1_PRES		2
#define KEY2_PRES		3
#define WKUP_PRES   	4

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
