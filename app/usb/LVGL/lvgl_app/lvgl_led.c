#include "../../lv_examples.h"
#if LV_USE_LED && LV_BUILD_EXAMPLES

lv_obj_t * led_red = NULL;
lv_obj_t * led_green = NULL;
lv_obj_t * led_blue = NULL;
/**
 * Create LED's with different brightness and color
 */
void lv_show_led(void)
{
    /*Create a LED and switch it OFF*/
    led_red  = lv_led_create(lv_scr_act());
    lv_obj_align(led_red, LV_ALIGN_CENTER, -80, 0);
    lv_led_set_color(led_red, lv_palette_main(LV_PALETTE_RED));
    lv_led_set_brightness(led_red, 10);
    lv_led_on(led_red);

    /*Copy the previous LED and set a brightness*/
    led_green  = lv_led_create(lv_scr_act());
    lv_obj_align(led_green, LV_ALIGN_CENTER, 0, 0);
    lv_led_set_brightness(led_green, 150);
    lv_led_set_color(led_green, lv_palette_main(LV_PALETTE_GREEN));
    lv_led_on(led_green);

    /*Copy the previous LED and switch it ON*/
    led_blue  = lv_led_create(lv_scr_act());
    lv_obj_align(led_blue, LV_ALIGN_CENTER, 80, 0);
    lv_led_set_color(led_blue, lv_palette_main(LV_PALETTE_BLUE));
    lv_led_on(led_blue);
}

#endif