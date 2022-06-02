#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "stm32l4xx_hal_spi.h"



void SPI1_Init(void);
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI1_ReadWriteByte(u8 TxData);
u8 SPI1_WriteByte(u8 TxData);


void SPI3_Init(void);
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI3_ReadWriteByte(u8 TxData);
u8 SPI3_WriteByte(u8 *TxData,u16 size);

#endif
