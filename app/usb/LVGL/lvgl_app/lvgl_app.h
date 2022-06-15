/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LVGL_APP_H
#define __LVGL_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/




/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
void lv_show_img(void);
void lv_show_qrcode(void);
void lv_show_led(void);

extern lv_obj_t *main_win_contanier;
extern lv_obj_t *qrcode_win_contanier;
extern lv_obj_t * qr;

extern lv_obj_t * led_red;
extern lv_obj_t * led_green;
extern lv_obj_t * led_blue;
/* USER CODE END ET */



#ifdef __cplusplus
}
#endif

#endif /* __LVGL_APP_H */