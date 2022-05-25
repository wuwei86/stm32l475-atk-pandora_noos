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

#define LOG_TAG    "main"

#include "main.h"
#include "fatfs.h"
#include "usb_host.h"
#include "elog.h"
#include "bsp.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include "w25qxx.h"
#include "delay.h"
#include "sfud.h"

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

FATFS USBDISKFatFs;
FIL MyFile;
extern ApplicationTypeDef Appli_state;

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



/* USER CODE BEGIN 4 */
static void MSC_Application(void)
{
    FRESULT res;                                          /* FatFs function common result code */
    uint32_t byteswriten;                   /* File write/read counts */
    uint8_t wtext[] = "The site is STM32cube.com working with FatFs"; /* File write buffer */
//  uint8_t rtext[100];                                   /* File read buffer */

    /* Register the file system object to the FatFs module */
    if(f_mount(&USBDISKFatFs, (TCHAR const*)USBHPath, 0) != FR_OK)
    {
        /* FatFs Initialization Error */
        Error_Handler();
    }
    else
    {
        /* Create and Open a new text file object with write access */
        if(f_open(&MyFile, "aa.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
        {
            /* 'STM32.TXT' file Open for write Error */
            Error_Handler();
        }
        else
        {
            /* Write data to the text file */
            res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswriten);

            if((byteswriten == 0) || (res != FR_OK))
            {
                /* 'STM32.TXT' file Write or EOF Error */
                Error_Handler();
            }
            else
            {
                /* Close the open text file */
                f_close(&MyFile);
            }
        }
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
    elog_init();
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

    /* 启动elog */
    elog_start();
    test_elog();

    printDebugMsg("hello");
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
  }   
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  elog_user_init();
  LCD_Init();				//
  log_d("system clk is %d",clk);

  W25QXX_Init();
  //W25QXX_Write((u8*)TEXT_Buffer, FLASH_Buf_Address, SIZE);		//´ÓÖ¸¶¨µØÖ·´¦¿ªÊ¼,Ð´ÈëSIZE³¤¶ÈµÄÊý¾Ý

	//W25QXX_Read(datatemp, FLASH_Buf_Address, SIZE);					//´ÓÖ¸¶¨µØÖ·´¦¿ªÊ¼,¶Á³öSIZE¸ö×Ö½Ú
  log_d("datatemp is %s",datatemp);
  sfud_user_init();
  

	my_gfx_op.draw_pixel = gfx_draw_pixel;
	my_gfx_op.fill_rect = NULL;//gfx_fill_rect;
	startHelloWave(NULL, 240, 240, 2, &my_gfx_op);

  //Display_ALIENTEK_LOGO(0, 0);

	//printf("hello");
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    MX_USB_HOST_Process();
    

     switch(Appli_state)
      {
        case APPLICATION_READY:
          MSC_Application();
          Appli_state = APPLICATION_DISCONNECT;
          break;

        case APPLICATION_DISCONNECT:
          f_mount(NULL, (TCHAR const*)"", 0);

          break;
        default:
          break;
      }

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

