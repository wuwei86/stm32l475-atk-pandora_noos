#include "delay.h"
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
 *	使用SysTick的普通计数模式对延迟进行管理
 *	包括delay_us,delay_ms
 *	正点原子@ALIENTEK
 *	技术论坛:www.openedv.com
 *	创建日期:2015/6/10
 *	版本：V1.1
 *	版权所有，盗版必究。
 *	Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	修改说明
 *	******************************************************************************/

//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

static u32 fac_us = 0;							//us延时倍乘数

#if SYSTEM_SUPPORT_OS
static u16 fac_ms = 0;				      //ms延时倍乘数,在os下,代表每个节拍的ms数

#if SYSTEM_SUPPORT_OS							//如果SYSTEM_SUPPORT_OS定义了,说明要支持OS了(不限于UCOS).
//当delay_us/delay_ms需要支持OS的时候需要三个与OS相关的宏定义和函数来支持
//首先是3个宏定义:
//delay_osrunning:用于表示OS当前是否正在运行,以决定是否可以使用相关函数
//delay_ostickspersec:用于表示OS设定的时钟节拍,delay_init将根据这个参数来初始哈systick
//delay_osintnesting:用于表示OS中断嵌套级别,因为中断里面不可以调度,delay_ms使用该参数来决定如何运行
//然后是3个函数:
//delay_osschedlock:用于锁定OS任务调度,禁止调度
//delay_osschedunlock:用于解锁OS任务调度,重新开启调度
//delay_ostimedly:用于OS延时,可以引起任务调度.

//本例程仅作UCOSII和UCOSIII的支持,其他OS,请自行参考着移植
//支持UCOSII
#ifdef 	OS_CRITICAL_METHOD						//OS_CRITICAL_METHOD定义了,说明要支持UCOSII				
#define delay_osrunning		OSRunning			//OS是否运行标记,0,不运行;1,在运行
#define delay_ostickspersec	OS_TICKS_PER_SEC	//OS时钟节拍,即每秒调度次数
#define delay_osintnesting 	OSIntNesting		//中断嵌套级别,即中断嵌套次数
#endif

//支持UCOSIII
#ifdef 	CPU_CFG_CRITICAL_METHOD					//CPU_CFG_CRITICAL_METHOD定义了,说明要支持UCOSIII	
#define delay_osrunning		OSRunning			//OS是否运行标记,0,不运行;1,在运行
#define delay_ostickspersec	OSCfg_TickRate_Hz	//OS时钟节拍,即每秒调度次数
#define delay_osintnesting 	OSIntNestingCtr		//中断嵌套级别,即中断嵌套次数
#endif


/**
 * @brief	us级延时时,关闭任务调度(防止打断us级延迟)
 *
 * @param   SYSCLK	系统时钟频率
 *
 * @return  void
 */
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			//使用UCOSIII
    OS_ERR err;
    OSSchedLock(&err);						//UCOSIII的方式,禁止调度，防止打断us延时
#else										//否则UCOSII
    OSSchedLock();							//UCOSII的方式,禁止调度，防止打断us延时
#endif
}
/**
 * @brief	us级延时时,恢复任务调度
 *
 * @param   SYSCLK	系统时钟频率
 *
 * @return  void
 */
void delay_osschedunlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			//使用UCOSIII
    OS_ERR err;
    OSSchedUnlock(&err);					//UCOSIII的方式,恢复调度
#else										//否则UCOSII
    OSSchedUnlock();						//UCOSII的方式,恢复调度
#endif
}
/**
 * @brief	调用OS自带的延时函数延时
 *
 * @param   ticks	延时的节拍数
 *
 * @return  void
 */
void delay_ostimedly(u32 ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
    OS_ERR err;
    OSTimeDly(ticks, OS_OPT_TIME_PERIODIC, &err); //UCOSIII延时采用周期模式
#else
    OSTimeDly(ticks);						    //UCOSII延时
#endif
}
/**
* @brief	systick中断服务函数,使用OS时用到
*
* @param   ticks	延时的节拍数
*
* @return  void
*/
void SysTick_Handler(void)
{
    HAL_IncTick();

    if(delay_osrunning == 1)					//OS开始跑了,才执行正常的调度处理
    {
        OSIntEnter();						//进入中断
        OSTimeTick();       				//调用ucos的时钟服务程序
        OSIntExit();       	 				//触发任务切换软中断
    }
}
#endif



#endif
/**
 * @brief	初始化延迟函数,SYSTICK的时钟固定为AHB时钟
 *
 * @param   SYSCLK	系统时钟频率
 *
 * @return  void
 */
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
    u32 reload;
#endif
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK
    fac_us = SYSCLK;						//不论是否使用OS,fac_us都需要使用

#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
    reload = SYSCLK;					  //每秒钟的计数次数 单位为K
    reload *= 1000000 / delay_ostickspersec;	//根据delay_ostickspersec设定溢出时间
    //reload为24位寄存器,最大值:16777216,在80M下，约209.7ms左右
    fac_ms = 1000 / delay_ostickspersec;		//代表OS可以延时的最少单位
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //开启SYSTICK中断
    SysTick->LOAD = reload; 					//每1/OS_TICKS_PER_SEC秒中断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //开启SYSTICK
#else
#endif
}


/**
 * @brief	延时微秒(us)函数
 *
 * @remark	nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)
 *
 * @param   nus		需要延时多少微秒
 *
 * @return  void
 */
void delay_us(u32 nus)
{
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;				//LOAD的值
    ticks = nus * fac_us; 						//需要的节拍数
    told = SysTick->VAL;        				//刚进入时的计数器值

    while(1)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {
            if(tnow < told)tcnt += told - tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
            else tcnt += reload - tnow + told;
			
            told = tnow;
            if(tcnt >= ticks)break;			//时间超过/等于要延迟的时间,则退出.
        }
    }
}


/**
 * @brief	延时毫秒(ms)函数
 *
 * @param   nms		需要延时多少毫秒
 *
 * @return  void
 */
void delay_ms(u16 nms)
{
    u32 i;

    for(i = 0; i < nms; i++) delay_us(1000);
}







