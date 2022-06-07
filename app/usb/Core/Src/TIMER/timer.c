#include "timer.h"
#include "sys.h"
#include "led.h"
#include "stm32l4xx_hal.h"

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
 *	定时器中断驱动代码
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


TIM_HandleTypeDef TIM3_Handler;      //定时器句柄


/**
 * @brief	通用定时器3中断初始化函数
 *			定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
 *			Ft=定时器工作频率,单位:Mhz
 *			这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK:80Mhz)
 *
 * @param   arr		自动重装值。
 * @param   psc		时钟预分频数
 *
 * @return  void
 */
void TIM3_Init(u16 arr, u16 psc)
{
    TIM3_Handler.Instance = TIM3;                          	//通用定时器3
    TIM3_Handler.Init.Prescaler = psc;                     	//分频系数
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;    	//向上计数器
    TIM3_Handler.Init.Period = arr;                        	//自动装载值
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM3_Handler);

    HAL_TIM_Base_Start_IT(&TIM3_Handler);					//使能定时器3和定时器3更新中断：TIM_IT_UPDATE
}


