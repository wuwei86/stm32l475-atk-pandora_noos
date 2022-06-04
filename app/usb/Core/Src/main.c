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

#define LOG_TAG    "app_main"

#include "main.h"
#include "fatfs.h"
#include "usb_host.h"
#include "sd_diskio.h"
#include "elog.h"
#include "elog_flash.h"
#include "bsp.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include "w25qxx.h"
#include "delay.h"
#include "spi1.h"
#include "sd_card.h"
#include "sfud.h"
#include "easyflash.h"
#include "shell_port.h"
#include "cJSON.h"
#include "cJSON_Utils.h"   




/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/


/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
// void SystemClock_Config(void);
// static void MX_GPIO_Init(void);
// static void MX_USART1_UART_Init(void);
void MX_USB_HOST_Process(void);

// FATFS USBDISKFatFs;
// FIL USBHFile;

// FATFS SDDISKFatFs;
// FIL SDFile;

extern ApplicationTypeDef Appli_state;

/* GUILITE使用 -----------------------------------------------*/
#define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))

void gfx_draw_pixel(int x, int y, unsigned int rgb)
{
	LCD_Draw_ColorPoint(x, y, GL_RGB_32_to_16(rgb));
}

struct EXTERNAL_GFX_OP
{
	void (*draw_pixel)(int x, int y, unsigned int rgb);
	void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} my_gfx_op;
extern void startHelloWave(void* phy_fb, int width, int height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op);
/* GUILITE使用 -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
// int main(void)
// {
//   /* USER CODE BEGIN 1 */

//   /* USER CODE END 1 */

//   /* MCU Configuration--------------------------------------------------------*/

//   /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
//   HAL_Init();

//   /* USER CODE BEGIN Init */

//   /* USER CODE END Init */

//   /* Configure the system clock */
//   SystemClock_Config();

//   /* USER CODE BEGIN SysInit */

//   /* USER CODE END SysInit */

//   /* Initialize all configured peripherals */
//   MX_GPIO_Init();
//   MX_FATFS_Init();
//   MX_USB_HOST_Init();
//   MX_USART1_UART_Init();
//   /* USER CODE BEGIN 2 */

//   /* USER CODE END 2 */

//   /* Infinite loop */
//   /* USER CODE BEGIN WHILE */
//   while (1)
//   {
//     /* USER CODE END WHILE */
//     MX_USB_HOST_Process();

//     /* USER CODE BEGIN 3 */
//   }
//   /* USER CODE END 3 */
// }

void SD_Mount(void)
{
  FRESULT res;                                          /* FatFs function common result code */
  uint32_t byteswriten;                   /* File write/read counts */
  uint32_t bytesread;
  uint8_t wtext[] = "The site is STM32cube.com working with FatFs\0"; /* File write buffer */
  uint8_t rtext[100] = {0}; 

  log_d("SDPath is: %s",SDPath);
  res = f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
  if(res != FR_OK)
  {
      log_d("SDPath f_mount is fail error code is :%d",res);
      /* FatFs Initialization Error */
      Error_Handler();
  }
  else
  {
      log_d("SDPath f_mount is success");
      /* Create and Open a new text file object with write access */
      res = f_open(&SDFile, "1:ww.TXT", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
      log_d("SDPath f_open res is :%d",res);
      if(res != FR_OK)
      {
          log_d("SDPath f_open is fail###res is :%d",res);
          /* 'STM32.TXT' file Open for write Error */
          Error_Handler();
      }
      else
      {
          log_d("SDPath f_open is success");
          /* Write data to the text file */
          res = f_write(&SDFile, wtext, sizeof(wtext)+1, (void *)&byteswriten);
          if((byteswriten == 0) || (res != FR_OK))
          {
              log_d("SDPath f_write is fail###res is :%d",res);
              /* 'STM32.TXT' file Write or EOF Error */
              Error_Handler();
          }
          else
          {
              log_d("SDPath f_write is success###res is :%d###byteswriten is: %d",res,byteswriten);
              /* Close the open text file */
              f_lseek(&SDFile, 0);//读取文件的时候必须要加上这一句，否则报错
              res = f_read(&SDFile,rtext,100,&bytesread);
              log_d("f_read is rtext is: %s",rtext);
              log_d("f_read is fail error code is: %d",res);
              /* Close the open text file */
              res = f_close(&SDFile);
              log_d("f_close is fail error code is: %d",res);
          }
      }

      log_d("#####SDPath f_mount is success");
      /* Create and Open a new text file object with write access */
      res = f_open(&SDFile, "1:tt.log", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
      log_d("SDPath f_open res is :%d",res);
      if(res != FR_OK)
      {
          printf("SDPath f_open is fail###res is :%d",res);
          /* 'STM32.TXT' file Open for write Error */
          Error_Handler();
      }
      else
      {
          f_lseek(&SDFile, 0);//读取文件的时候必须要加上这一句，否则报错
          res = f_read(&SDFile,rtext,100,&bytesread);
          log_d("f_read is rtext is: %s",rtext);
          log_d("f_read is fail error code is: %d",res);
          /* Close the open text file */
          res = f_close(&SDFile);
          log_d("f_close is fail error code is: %d",res);
      }

  }
}

/* USER CODE BEGIN 4 */
static void MSC_Application(void)
{
    FRESULT res;                                          /* FatFs function common result code */
    uint32_t byteswriten;                   /* File write/read counts */
    uint32_t bytesread;
    uint8_t wtext[] = "The site is STM32cube.com working with FatFs\0"; /* File write buffer */
    uint8_t rtext[100] = {0};                                   /* File read buffer */
    log_d("USBHPath is: %s",USBHPath);
    
    /* Register the file system object to the FatFs module */
    if(f_mount(&USBHFatFS, (TCHAR const*)USBHPath, 1) != FR_OK)
    {
        /* FatFs Initialization Error */
        Error_Handler();
    }
    else
    {
        /* Create and Open a new text file object with write access */
        if(f_open(&USBHFile, "0:aa.TXT", FA_CREATE_ALWAYS | FA_WRITE | FA_READ) != FR_OK)
        {
            log_d("USB f_open is fail");
            /* 'STM32.TXT' file Open for write Error */
            Error_Handler();
        }
        else
        {
            log_d("USB f_open is success");
            /* Write data to the text file */
            res = f_write(&USBHFile, wtext, sizeof(wtext)+1, (void *)&byteswriten);

            if((byteswriten == 0) || (res != FR_OK))
            {
                log_d("USB f_write is fail");
                /* 'STM32.TXT' file Write or EOF Error */
                Error_Handler();
            }
            else
            {
                log_d("USB f_write is success");
                f_lseek(&USBHFile, 0);//读取文件的时候必须要加上这一句，否则报错
                res = f_read(&USBHFile,rtext,100,&bytesread);
                log_d("f_read is rtext is: %s",rtext);
                log_d("f_read is fail error code is: %d",res);
                /* Close the open text file */
                res = f_close(&USBHFile);
                log_d("f_close is fail error code is: %d",res);
            }
        }

        res = f_open(&USBHFile, "0:bb.TXT", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
        if(res != FR_OK)
        {
            log_e("f_open is fail error code is: %d",res);
            res = f_close(&USBHFile);
            log_d("f_close is fail error code is: %d",res);
        }
        else
        {
            res = f_write(&USBHFile, wtext, sizeof(wtext)+1, (void *)&byteswriten);
            if(res == FR_OK)
            {
                log_d("f_write is success");
            }
            else
            {
                log_e("f_write is fail error code is: %d",res);
            }
            res = f_close(&USBHFile);
            log_d("f_close is fail error code is: %d",res);
        }
    }
}

static void user_usb_process(void)
{
  switch(Appli_state)
  {
    case APPLICATION_READY:
      MSC_Application();
      Appli_state = APPLICATION_DISCONNECT;//联上去后立即断开并卸载文件系统
      break;

    case APPLICATION_DISCONNECT:
      //f_mount(NULL, (TCHAR const*)"", 0);//卸载u盘
      //f_mount(NULL, (TCHAR const*)USBHPath, 1);//卸载u盘
      //f_mount(NULL, (TCHAR const*)SDPath, 1);//卸载SDCARD

      break;
    default:          
      break;         
  }
}

void test_elog(void) 
 {
    /* test log output for all level */
    log_a("Hello EasyLogger!");
    log_e("Hello EasyLogger!");
    log_w("Hello EasyLogger!");
    log_i("Hello EasyLogger!");
    log_d("Hello EasyLogger!");
    log_v("Hello EasyLogger!");
//    elog_raw("Hello EasyLogger!");
}

void elog_user_init(void)
 {
  //elog_init();
  elog_set_text_color_enabled(true);

  /* 设置每个级别的日志输出格式 */
  //输出所有内容
  elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
  //输出日志级别信息和日志TAG
  elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG);
  elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG);
  elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG);
  //除了时间、进程信息、线程信息之外，其余全部输出
  elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_TIME | ELOG_FMT_P_INFO | ELOG_FMT_T_INFO));
  //输出所有内容
  elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);

#ifdef ELOG_FLASH_ENABLE
  /* initialize EasyLogger Flash plugin */
  elog_flash_init();
#endif

#ifdef ELOG_FILE_ENABLE
  /* initialize EasyLogger Flash plugin */
  elog_file_init();
#endif

  /* 启动elog */
  elog_start();
  test_elog();

  printDebugMsg("hello###123456789");
  printDebugMsg("hello1111");
}

//写入缓存数据
const u8 TEXT_Buffer[] = {"wwww STM32L4 QSPI TEST"};
#define SIZE sizeof(TEXT_Buffer)
	
#define FLASH_Buf_Address		0x00803000			//地址 预留给用户使用（1012K）
u8 datatemp[SIZE];

void sfud_user_init(void)
{
  sfud_err result = SFUD_SUCCESS;
  const sfud_flash *flash = sfud_get_device(SFUD_NM25_DEVICE_INDEX);

  if (sfud_init() == SFUD_SUCCESS)
  {
      /* enable qspi fast read mode, set four data lines width */
      //sfud_qspi_fast_read_enable(sfud_get_device(SFUD_W25_DEVICE_INDEX), 4);
      //sfud_demo(0, sizeof(sfud_demo_test_buf), sfud_demo_test_buf);
      log_d("sfud_init() is SUCCESS");
      result = sfud_read(flash, FLASH_Buf_Address, SIZE, datatemp);
      if (result == SFUD_SUCCESS)
      {
        log_d("read flash data is %s",datatemp);   
      }

      //擦除
      result = sfud_erase(flash, FLASH_Buf_Address,SIZE);
      if (result == SFUD_SUCCESS)
      {
        log_d("erase flash data is %d",result);   
      }

      result = sfud_read(flash, FLASH_Buf_Address, SIZE, datatemp);
      if (result == SFUD_SUCCESS)
      {
        log_d("read flash data is %s",datatemp);   
      }

      result = sfud_write(flash, FLASH_Buf_Address, SIZE, TEXT_Buffer);
      if (result == SFUD_SUCCESS)
      {
        log_d("write flash data is %s",TEXT_Buffer);   
      }

      result = sfud_read(flash, FLASH_Buf_Address, SIZE, datatemp);
      if (result == SFUD_SUCCESS)
      {
        log_d("read flash data is %s",datatemp);   
      }

      result = sfud_write(flash, FLASH_Buf_Address+0xff, SIZE, TEXT_Buffer);
      if (result == SFUD_SUCCESS)
      {
        log_d("write flash data is %s",TEXT_Buffer);   
      }

      result = sfud_read(flash, FLASH_Buf_Address, SIZE, datatemp);
      if (result == SFUD_SUCCESS)
      {
        log_d("read flash data is %s",datatemp);   
      }

      result = sfud_read(flash, FLASH_Buf_Address+0xff, SIZE, datatemp);
      if (result == SFUD_SUCCESS)
      {
        log_d("read flash data is %s",datatemp);   
      }
  }   
}

static void test_env(void) 
{
  uint32_t i_boot_times = NULL;
  uint32_t i_read_time = NULL;

  char *c_old_boot_times, c_new_boot_times[11] = {0};
  char *c_read_times;

  /* get the boot count number from Env */
  //通过环境变量的名字来获取其对应的值。（注意：此处的环境变量指代的已加载到内存中的环境变量）
  c_old_boot_times = ef_get_env("boot_times");
  assert_param(c_old_boot_times);
  i_boot_times = atol(c_old_boot_times);
  log_d("read i_boot_times %d times\n\r", i_boot_times);

  ef_set_and_save_env("read_t","333");//设置环境变量成功后立刻保存。设置功能参考ef_set_env方法。（注意：此处的环境变量指代的任意创建，不需要加载到内存中）
  c_read_times = ef_get_env("read_t");
  assert_param(c_read_times);
  i_read_time = atol(c_read_times);
  log_d("read times %s", c_read_times);


  /* boot count +1 */
  i_boot_times ++;
  log_d("The system now boot %d times\n\r", i_boot_times);
  /* interger to string */
  sprintf(c_new_boot_times,"%ld", i_boot_times);
  log_d("c_new_boot_times is: %s", c_new_boot_times);
  /* set and store the boot count number to Env */
  //使用此方法可以实现对环境变量的增加、修改及删除功能。（注意：此处的环境变量指代的已加载到内存中的环境变量）
  //default_env_set表中的环境变量
  ef_set_env("boot_times", c_new_boot_times);
  ef_save_env();//保存内存中的环境变量表到Flash中
  //ef_read_env_value();
  //ef_print_env();
  ef_env_set_default();//将内存中的环境变量表重置为默认值。
  //size_t size = ef_get_env_write_bytes();//获取当前环境变量写入到Flash的字节大小
  //log_d("ef_get_env_write_bytes is: %d",size);
  ef_set_and_save_env("device_id","123");//设置环境变量成功后立刻保存。设置功能参考ef_set_env方法。（注意：此处的环境变量指代的已加载到内存中的环境变量）
  //ef_print_env();
}

void easyflash_user_init(void)
{
  //easyflash_init会调用sfud_init
  //if (easyflash_init() == EF_NO_ERR)
  //{
    /* test Env demo */
  log_d("easyflash_init success");
  test_env(); 
  //} 
}

//easylogger easyfalsh同时进行初始化,实现日志保存在flash的功能
void elogger_eflash_user_init(void)
{
  if ((easyflash_init() == EF_NO_ERR)&&(elog_init() == ELOG_NO_ERR)) 
  {
    elog_user_init(); 
    easyflash_user_init();
  }
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  //SCB->VTOR = FLASH_BASE | 0x19000; //内部FLASH的向量表重定位
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  delay_init(80); 			//时钟80M
  uint32_t clk = 0;
  clk = HAL_RCC_GetSysClockFreq();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FATFS_Init();
  MX_USB_HOST_Init();
  MX_USART1_UART_Init();
  printf(LOG_PROJECT_VERSION_MSG);//打印工程信息
  W25QXX_Init();
  LCD_Init();	
  SD_Init();

  //挂载sd卡并测试文件的打开及其写入关闭
  SD_Mount();
  
  //elog_user_init();
  //easyflash_user_init();
  elogger_eflash_user_init();
  
  log_d("system clk is %d",clk);

  userShellInit();
  
  //W25QXX_Write((u8*)TEXT_Buffer, FLASH_Buf_Address, SIZE);		

  W25QXX_Read(datatemp, FLASH_Buf_Address, SIZE);					
  log_d("datatemp is:   %s",datatemp);
  elog_hexdump("datatemp",16,datatemp,SIZE);
  //sfud_user_init();


  extern int CJSON_CDECL cjson_main();
  cjson_main();


  //挂载sd卡并测试文件的打开及其写入关闭
  //SD_Mount();

  //my_gfx_op.draw_pixel = gfx_draw_pixel;
  //my_gfx_op.fill_rect = NULL;//gfx_fill_rect;
  //startHelloWave(NULL, 240, 240, 2, &my_gfx_op);

  //Display_ALIENTEK_LOGO(0, 0);

  
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    MX_USB_HOST_Process();
    

    user_usb_process();

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}




#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

