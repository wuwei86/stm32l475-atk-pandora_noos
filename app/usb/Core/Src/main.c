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
#include "elog_file.h"
#include "bsp.h"
#include "lcd.h"
#include "key.h"
#include "timer.h"
#include "led.h"
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

#include "mb.h"
#include "mbport.h"
#include "port.h"

#include "user_mb_app.h"
#include "MultiTimer.h"
#include "multi_button.h"


#include <lvgl.h>
#include "lv_port_disp.h"
#include "lv_example_get_started.h"
#include "lv_example_anim.h"
#include "lv_example_widgets.h"
#include "lv_example_libs.h"

#include "lvgl_app.h"
#include "lvgl_button.h"


#include "wifi_uart.h"
#include "wifi_task.h"


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
#define LVGL_TICK 	5
MultiTimer lvgl_timer;
void lvglTimerCallback(MultiTimer* timer, void *userData)
{
    //log_d("[%012ld] Timer:%p callback-> %s.\r\n", PlatformTicksGetFunc(), timer, (char*)userData);
    lv_tick_inc(LVGL_TICK);
		lv_timer_handler();
    MultiTimerStart(&lvgl_timer, 5, lvglTimerCallback, userData);
}

static void lvgl_init( void ) 
{
    lv_init();
    lv_port_disp_init();        // ??????????????????
    // lv_port_indev_init();       // ?????????????????????
    // lv_port_fs_init();          // ???????????????????????????

    //lv_example_get_started_1();
    //lv_example_keyboard_1();
    //lv_example_img_2();
    //lv_example_qrcode_1();
    //lv_show_img();
    lv_show_led();
    lvgl_Button_init();
    MultiTimerStart(&lvgl_timer, 5, lvglTimerCallback, "lvgl timer");
  //   while(1) 
  //   {
	// 	// ????????? lv_tick_inc ????????? lv_timer_handler
	// 	lv_tick_inc(LVGL_TICK);
	// 	lv_timer_handler();
  //   MultiTimerYield();
	// 	HAL_Delay(LVGL_TICK);
	// }
}

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

/* GUILITE?????? -----------------------------------------------*/
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
/* GUILITE?????? -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void FreeModbus_Init(void)
{
  eMBErrorCode eStatus;
  eStatus = eMBInit(MB_RTU, 0x01, 0, 115200, MB_PAR_NONE);
  log_d("eMBInit is: %d",eStatus);
  eStatus = eMBEnable(  );
  log_d("eMBEnable is: %d",eStatus);
}
/* USER CODE END 0 */



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
              f_lseek(&SDFile, 0);//????????????????????????????????????????????????????????????
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
          f_lseek(&SDFile, 0);//????????????????????????????????????????????????????????????
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
                f_lseek(&USBHFile, 0);//????????????????????????????????????????????????????????????
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
      Appli_state = APPLICATION_DISCONNECT;//?????????????????????????????????????????????
      break;

    case APPLICATION_DISCONNECT:
      //f_mount(NULL, (TCHAR const*)"", 0);//??????u???
      //f_mount(NULL, (TCHAR const*)USBHPath, 1);//??????u???
      //f_mount(NULL, (TCHAR const*)SDPath, 1);//??????SDCARD

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

  /* ??????????????????????????????????????? */
  //??????????????????
  elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
  //?????????????????????????????????TAG
  elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG);
  elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG);
  elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG);
  //?????????????????????????????????????????????????????????????????????
  elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_TIME | ELOG_FMT_P_INFO | ELOG_FMT_T_INFO));
  //??????????????????
  elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);

#ifdef ELOG_FLASH_ENABLE
  /* initialize EasyLogger Flash plugin */
  elog_flash_init();
#endif

#ifdef ELOG_FILE_ENABLE
  /* initialize EasyLogger Flash plugin */
  elog_file_init();
#endif

  /* ??????elog */
  elog_start();
  test_elog();

  printDebugMsg("hello###123456789");
  printDebugMsg("hello1111");
}

//??????????????????
const u8 TEXT_Buffer[] = {"wwww STM32L4 QSPI TEST"};
#define SIZE sizeof(TEXT_Buffer)
	
#define FLASH_Buf_Address		0x00803000			//?????? ????????????????????????1012K???
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

      //??????
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
  //???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
  c_old_boot_times = ef_get_env("boot_times");
  assert_param(c_old_boot_times);
  i_boot_times = atol(c_old_boot_times);
  log_d("read i_boot_times %d times\n\r", i_boot_times);

  ef_set_and_save_env("read_t","333");//????????????????????????????????????????????????????????????ef_set_env????????????????????????????????????????????????????????????????????????????????????????????????
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
  //???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
  //default_env_set?????????????????????
  ef_set_env("boot_times", c_new_boot_times);
  ef_save_env();//????????????????????????????????????Flash???
  //ef_read_env_value();
  //ef_print_env();
  //ef_env_set_default();//???????????????????????????????????????????????????
  //size_t size = ef_get_env_write_bytes();//?????????????????????????????????Flash???????????????
  //log_d("ef_get_env_write_bytes is: %d",size);
  ef_set_and_save_env("device_id","123");//????????????????????????????????????????????????????????????ef_set_env??????????????????????????????????????????????????????????????????????????????????????????
  //ef_set_and_save_env("device_id123",123);
  //ef_print_env();
}

void easyflash_user_init(void)
{
  //easyflash_init?????????sfud_init
  //if (easyflash_init() == EF_NO_ERR)
  //{
    /* test Env demo */
  log_d("easyflash_init success");
  test_env(); 
  //} 
}

//easylogger easyfalsh?????????????????????,?????????????????????flash?????????
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
  //SCB->VTOR = FLASH_BASE | 0x19000; //??????FLASH?????????????????????
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  delay_init(80); 			//??????80M
  uint32_t clk = 0;
  clk = HAL_RCC_GetSysClockFreq();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  LED_Init();
  KEY_Init();
  TIM3_Init(5000 - 1, 8000 - 1);//???????????????8000???80M = 80000000/8000 = 10000 ???????????????5000 ???5000/10000 = 0.5s = 500ms
  MX_FATFS_Init();
  MX_USB_HOST_Init();
  MX_USART1_UART_Init();
  //MX_USART2_UART_Init();
  //wifi_uart_init(115200);
  
  //MX_USART3_UART_Init();
  //uint8_t tmp = 0;
  //HAL_UART_Receive_IT(&huart2, &tmp, 1);
  //HAL_UART_Transmit(&huart2,(const uint8_t*)LOG_VERSION_NUM, 5, 0xffff);
  //HAL_UART_Transmit(&huart2,(const uint8_t*)"AT\r\n", 5, 0xffff);
  //HAL_UART_Transmit_IT(&huart2,(const uint8_t*)LOG_VERSION_NUM, 5);
  

  printf(LOG_PROJECT_VERSION_MSG);//??????????????????
  W25QXX_Init();
  LCD_Init();	
  SD_Init();

  //??????sd?????????????????????????????????????????????
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

  FreeModbus_Init();

  extern void MultiTimer_test(void);
  MultiTimer_test();

  extern void MultiButton_callback();
  MultiButton_callback();

  wifi_init();

  //lvgl_init();

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

     ( void )eMBPoll();
 
    usSRegHoldBuf[3]++;
		if(usSRegHoldBuf[3] > 100)
    {
      usSRegHoldBuf[3] = 0;
    }

    MultiTimerYield();

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

