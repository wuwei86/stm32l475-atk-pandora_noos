#include <stdio.h>
#include "bsp.h"
#include "led.h"
#include "MultiTimer.h"
#include <lvgl.h>
#include "lv_port_disp.h"
#include "lvgl_button.h"
#include "lvgl_app.h"

#define LOG_TAG    "app_multitimer_test"


#include "elog.h"

MultiTimer timer1;
MultiTimer timer2;
MultiTimer timer3;

uint64_t PlatformTicksGetFunc(void)
{
    return (uint64_t)HAL_GetTick();
}

void exampleTimer1Callback(MultiTimer* timer, void *userData)
{
    //log_d("[%012ld] Timer:%p callback-> %s.\r\n", PlatformTicksGetFunc(), timer, (char*)userData);
    LED_R_TogglePin;
    lv_led_toggle(led_red);
    lv_led_toggle(led_green);
    lv_led_toggle(led_blue);
    MultiTimerStart(timer, 500, exampleTimer1Callback, userData);
}

void exampleTimer2Callback(MultiTimer* timer, void *userData)
{
    //log_d("[%012ld] Timer:%p callback-> %s.\r\n", PlatformTicksGetFunc(), timer, (char*)userData);
}

void exampleTimer3Callback(MultiTimer* timer, void *userData)
{
    //log_d("[%012ld] Timer:%p callback-> %s.\r\n", PlatformTicksGetFunc(), timer, (char*)userData);
    MultiTimerStart(timer, 4567, exampleTimer3Callback, userData);
}

void MultiTimer_test(void)
{
    MultiTimerInstall(PlatformTicksGetFunc);

    MultiTimerStart(&timer1, 500, exampleTimer1Callback, "500ms CYCLE timer");
    //MultiTimerStart(&timer2, 5000, exampleTimer2Callback, "5000ms ONCE timer");
    //MultiTimerStart(&timer3, 3456, exampleTimer3Callback, "3456ms delay start, 4567ms CYCLE timer");

    // while (1) {
    //     MultiTimerYield();
    // }
}
