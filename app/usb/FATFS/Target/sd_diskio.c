/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sd_diskio.c (based on usbh_diskio_template.c v2.0.2)
  * @brief   SDCARD Disk I/O driver
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
/* USER CODE BEGIN firstSection */
/* can be used to modify / undefine following code or add new definitions */
/* USER CODE END firstSection */

/* Includes ------------------------------------------------------------------*/
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#define LOG_TAG    "app_sd_diskio"


#include "elog.h"

DSTATUS SDCARD_initialize(BYTE lun);
DSTATUS SDCARD_status(BYTE lun);
DRESULT SDCARD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count);
DRESULT SDCARD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count);
DRESULT SDCARD_ioctl(BYTE lun, BYTE cmd, void *buff);

const Diskio_drvTypeDef  SDCARD_Driver =
{
  SDCARD_initialize,
  SDCARD_status,
  SDCARD_read,
  SDCARD_write,
  SDCARD_ioctl,
};

/**
  * @brief  Initializes a Drive
  * @param  lun : lun id
  * @retval DSTATUS: Operation status
  */
DSTATUS SDCARD_initialize(BYTE lun)
{
  /* CAUTION : USB Host library has to be initialized in the application */
  uint8_t res = RES_OK ;
  res = SD_Init();	//SD卡初始化 
  //log_d("SD_Init is %d",res);
  return ((DRESULT)res);
}

/**
  * @brief  Gets Disk Status
  * @param  lun : lun id
  * @retval DSTATUS: Operation status
  */
DSTATUS SDCARD_status(BYTE lun)
{
  DSTATUS res = RES_OK;

  return res;
}

/* USER CODE BEGIN beforeReadSection */
/* can be used to modify previous code / undefine following code / add new code */
/* USER CODE END beforeReadSection */

/**
  * @brief  Reads Sector(s)
  * @param  lun : lun id
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT SDCARD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
  uint8_t res = RES_OK;
  res = SD_ReadDisk(buff,sector,count);	
  while(res)
  {
    SD_Init();	
    res=SD_ReadDisk(buff,sector,count);	
  }
  //log_d("SD_ReadDisk is %d",res);
  return ((DRESULT)res);
}

/* USER CODE BEGIN beforeWriteSection */
/* can be used to modify previous code / undefine following code / add new code */
/* USER CODE END beforeWriteSection */

/**
  * @brief  Writes Sector(s)
  * @param  lun : lun id
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */

DRESULT SDCARD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    uint8_t res = RES_OK;
    res = SD_WriteDisk((u8*)buff,sector,count);
    while(res)
    {
      SD_Init();	
      res=SD_WriteDisk((u8*)buff,sector,count);	
    }
    //log_d("SD_WriteDisk is %d",res);
    return ((DRESULT)res);
}


/* USER CODE BEGIN beforeIoctlSection */
/* can be used to modify previous code / undefine following code / add new code */
/* USER CODE END beforeIoctlSection */

/**
  * @brief  I/O control operation
  * @param  lun : lun id
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */

DRESULT SDCARD_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_OK;

  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC:
    res = RES_OK;
    break;

  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    *(DWORD*)buff = SD_GetSectorCount();
		res = RES_OK;
    break;

  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    *(DWORD*)buff = 512; 
		res = RES_OK;
    break;

    /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(WORD*)buff = 8;
		res = RES_OK;
    break;

  default:
    res = RES_PARERR;
  }
  return res;
}
