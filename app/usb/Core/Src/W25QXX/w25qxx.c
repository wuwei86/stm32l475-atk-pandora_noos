#include "w25qxx.h"
#include "qspi.h"
#include "delay.h"
//#include "usart.h"


#define LOG_TAG    "W25QXX"

#include "elog.h"
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
 *	W25QXX QSPI驱动代码
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

/***********************************************************************

			SPI FLASH(W25Q128)资源分配
					
	序号	 偏移地址			大小		 存储内容
	 1		0x00000000		0x00200000		预留给RT Thread 使用(2M) 
	 2		0x00200000		0x00603000		预留给中文字库使用（6156K）
	 3		0x00803000		0x000FD000		预留给用户使用（1012K）
	 4		0x00900000		0x00700000		预留给 FATFS 使用（7M）
	 
	在测试本例程的时候，不要随意擦除扇区数据
	 
************************************************************************/

/**
	4Kbytes为一个Sector
	16个扇区为1个Block
	W25Q128容量为16M字节,共有256个Block,4096个Sector
*/
u16 W25QXX_TYPE = W25Q128;	//默认是W25Q128



/**
 * @brief	初始化SPI FLASH的IO口
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Init(void)
{
    QSPI_Init();					//初始化QSPI
    log_d("W25QXX_TYPE is 0x%x",W25QXX_TYPE);
    //必须执行2次读取id的操作才能读取出来，不知道什么原因
    W25QXX_TYPE = W25QXX_ReadID();	//读取FLASH ID.
    log_d("W25QXX_TYPE is 0x%x",W25QXX_TYPE);
    W25QXX_TYPE = W25QXX_ReadID();	//读取FLASH ID.
    log_d("W25QXX_TYPE is 0x%x",W25QXX_TYPE);
}


/**
 * @brief	读取W25QXX的状态寄存器，W25QXX一共有3个状态寄存器
 * 			状态寄存器1：
 * 			BIT7  6   5   4   3   2   1   0
 * 			SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 * 			SPR:默认0,状态寄存器保护位,配合WP使用
 * 			TB,BP2,BP1,BP0:FLASH区域写保护设置
 * 			WEL:写使能锁定
 * 			BUSY:忙标记位(1,忙;0,空闲)
 * 			默认:0x00
 * 			状态寄存器2：
 * 			BIT7  6   5   4   3   2   1   0
 * 			SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 * 			状态寄存器3：
 * 			BIT7      6    5    4   3   2   1   0
 * 			HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 *
 * @param   regno	状态寄存器号，范:1~3
 *
 * @return  u8		状态寄存器值
 */
u8 W25QXX_ReadSR(u8 regno)
{
    u8 byte = 0, command = 0;

    switch(regno)
    {
        case 1:
            command = W25X_ReadStatusReg1;  //读状态寄存器1指令
            break;

        case 2:
            command = W25X_ReadStatusReg2;  //读状态寄存器2指令
            break;

        case 3:
            command = W25X_ReadStatusReg3;  //读状态寄存器3指令
            break;

        default:
            command = W25X_ReadStatusReg1;
            break;
    }

    QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(&byte, 1);
    return byte;
}

/**
 * @brief	写W25QXX状态寄存器
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Write_SR(u8 regno, u8 sr)
{
    u8 command = 0;

    switch(regno)
    {
        case 1:
            command = W25X_WriteStatusReg1;  //写状态寄存器1指令
            break;

        case 2:
            command = W25X_WriteStatusReg2;  //写状态寄存器2指令
            break;

        case 3:
            command = W25X_WriteStatusReg3;  //写状态寄存器3指令
            break;

        default:
            command = W25X_WriteStatusReg1;
            break;
    }

    QSPI_Send_CMD(command, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);
    QSPI_Transmit(&sr, 1);
}


/**
 * @brief	W25QXX写使能,将S1寄存器的WEL置位
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Write_Enable(void)
{
    QSPI_Send_CMD(W25X_WriteEnable, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

/**
 * @brief	W25QXX写禁止,将WEL清零
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Write_Disable(void)
{
    QSPI_Send_CMD(W25X_WriteDisable, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

/**
 * @brief	读取W25QXX芯片ID
 *
 * @param   void
 *
 * @return  u8	返回值如下
 * 				0XEF13,表示芯片型号为W25Q80
 * 				0XEF14,表示芯片型号为W25Q16
 * 				0XEF15,表示芯片型号为W25Q32
 * 				0XEF16,表示芯片型号为W25Q64
 * 				0XEF17,表示芯片型号为W25Q128
 * 				0XEF18,表示芯片型号为W25Q256
 */

u16 W25QXX_ReadID(void)
{
    u8 temp[2];
    u16 deviceid;
    QSPI_Send_CMD(W25X_ManufactDeviceID, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(temp, 2);
    deviceid = (temp[0] << 8) | temp[1];
    return deviceid;
}


/**
 * @brief	读取SPI FLASH数据
 *
 * @param   pBuffer			数据存储区
 * @param   ReadAddr		开始读取的地址(最大32bit)
 * @param   NumByteToRead	要读取的字节数(最大65535)
 *
 * @return  void
 */
void W25QXX_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    QSPI_Send_CMD(W25X_FastReadData, ReadAddr, 8, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Receive(pBuffer, NumByteToRead);
}


/**
 * @brief	在指定地址开始写入最大256字节的数据
 *
 * @param   pBuffer			数据存储区
 * @param   WriteAddr		开始写入的地址(最大32bit)
 * @param   NumByteToWrite	要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
 *
 * @return  void
 */
void W25QXX_Write_Page(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    W25QXX_Write_Enable();					//写使能
    QSPI_Send_CMD(W25X_PageProgram, WriteAddr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    QSPI_Transmit(pBuffer, NumByteToWrite);
    W25QXX_Wait_Busy();					   //等待写入结束
}

/**
 * @brief	无检验写SPI FLASH
 * 			必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
 * 			具有自动换页功能
 * 			在指定地址开始写入指定长度的数据,但是要确保地址不越界!
 *
 * @param   pBuffer			数据存储区
 * @param   WriteAddr		开始写入的地址(最大32bit)
 * @param   NumByteToWrite	要写入的字节数(最大65535)
 *
 * @return  void
 */
void W25QXX_Write_NoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 pageremain;
    pageremain = 256 - WriteAddr % 256; //单页剩余的字节数

    if(NumByteToWrite <= pageremain)pageremain = NumByteToWrite; //不大于256个字节

    while(1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);

        if(NumByteToWrite == pageremain)break; //写入结束了

        else //NumByteToWrite>pageremain
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;			 //减去已经写入了的字节数

            if(NumByteToWrite > 256)pageremain = 256; //一次可以写入256个字节

            else pageremain = NumByteToWrite; 	 //不够256个字节了
        }
    }
}


/**
 * @brief	写SPI FLASH
 * 			在指定地址开始写入指定长度的数据
 * 			该函数带擦除操作!
 *
 * @param   pBuffer			数据存储区
 * @param   WriteAddr		开始写入的地址(最大32bit)
 * @param   NumByteToWrite	要写入的字节数(最大65535)
 *
 * @return  void
 */
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 * W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secpos = WriteAddr / 4096; //扇区地址
    secoff = WriteAddr % 4096; //在扇区内的偏移
    secremain = 4096 - secoff; //扇区剩余空间大小

    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
    if(NumByteToWrite <= secremain)secremain = NumByteToWrite; //不大于4096个字节

    while(1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); //读出整个扇区的内容

        for(i = 0; i < secremain; i++) //校验数据
        {
            if(W25QXX_BUF[secoff + i] != 0XFF)break; //需要擦除
        }

        if(i < secremain) //需要擦除
        {
            W25QXX_Erase_Sector(secpos);//擦除这个扇区

            for(i = 0; i < secremain; i++)	 //复制
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }

            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); //写入整个扇区

        }

        else W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); //写已经擦除了的,直接写入扇区剩余区间.

        if(NumByteToWrite == secremain)break; //写入结束了

        else//写入未结束
        {
            secpos++;//扇区地址增1
            secoff = 0; //偏移位置为0

            pBuffer += secremain; //指针偏移
            WriteAddr += secremain; //写地址偏移
            NumByteToWrite -= secremain;				//字节数递减

            if(NumByteToWrite > 4096)secremain = 4096;	//下一个扇区还是写不完

            else secremain = NumByteToWrite;			//下一个扇区可以写完了
        }
    };
}

/**
 * @brief	擦除整个芯片,等待时间超长...
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable();					//SET WEL
    W25QXX_Wait_Busy();
    QSPI_Send_CMD(W25X_ChipErase, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
    W25QXX_Wait_Busy();						//等待芯片擦除结束
}


/**
 * @brief	擦除一个扇区，擦除一个扇区的最少时间:150ms
 *
 * @param   Dst_Addr	扇区地址 根据实际容量设置
 *
 * @return  void
 */
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
    //printf("fe:%x\r\n",Dst_Addr);			//监视flash擦除情况,测试用
    Dst_Addr *= 4096;
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    QSPI_Send_CMD(W25X_SectorErase, Dst_Addr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE);
    W25QXX_Wait_Busy();   				    //等待擦除完成
}

/**
 * @brief	等待空闲
 *
 * @param   void
 *
 * @return  void
 */
void W25QXX_Wait_Busy(void)
{
    while((W25QXX_ReadSR(1) & 0x01) == 0x01); // 等待BUSY位清空
}







