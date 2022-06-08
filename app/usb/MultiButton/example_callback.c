#include "multi_button.h"
#include "MultiTimer.h"
#include "key.h"
#include "sys.h"

#define LOG_TAG    "app_multibutton_callback"


#include "elog.h"

enum Button_IDs {
	btn0_id,
	btn1_id,
	btn2_id,
	btn_wake_up,
};

struct Button btn0;
struct Button btn1;

MultiTimer btn_timer;

void btnTimerCallback(MultiTimer* timer, void *userData)
{
	button_ticks();
    MultiTimerStart(timer, 5, btnTimerCallback, userData);
}

uint8_t read_button_GPIO(uint8_t button_id)
{
	uint8_t readkey = 0xff;
	// you can share the GPIO read function with multiple Buttons
	switch(button_id)
	{
		case btn0_id:
			readkey = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_10);
			break;

		case btn1_id:
			readkey = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_9);
			break;

		case btn2_id:
			readkey = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_8);
			break;

		case btn_wake_up:
			readkey = HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_12);
			break;

		// default:
		// 	return 0;
		// 	break;
	}
	return readkey;
}

void BTN0_CALLBAKC_Handler(void* btn)
{
	uint8_t btn_event_val;
	btn_event_val = get_button_event((struct Button *)btn);

	switch (btn_event_val)
	{
		case PRESS_DOWN:
			log_d("BTN0 PRESS_DOWN");
			break;
		case PRESS_UP:
			log_d("BTN0 PRESS_UP");
			break;
		case PRESS_REPEAT:
			log_d("BTN0 PRESS_REPEAT");
			break;
		case SINGLE_CLICK:
			log_d("BTN0 SINGLE_CLICK");
			break;
		case DOUBLE_CLICK:
			log_d("BTN0 DOUBLE_CLICK");
			break;
		case LONG_PRESS_START:
			log_d("BTN0 LONG_PRESS_START");
			break;
		case LONG_PRESS_HOLD:
			log_d("BTN0 LONG_PRESS_HOLD");
			break;
		default:
			break;
	}

}

void BTN1_CALLBAKC_Handler(void* btn)
{
	uint8_t btn_event_val;
	btn_event_val = get_button_event((struct Button *)btn);
	
	switch (btn_event_val)
	{
		case PRESS_DOWN:
			log_d("BTN1 PRESS_DOWN");
			break;
		case PRESS_UP:
			log_d("BTN1 PRESS_UP");
			break;
		case PRESS_REPEAT:
			log_d("BTN1 PRESS_REPEAT");
			break;
		case SINGLE_CLICK:
			log_d("BTN1 SINGLE_CLICK");
			break;
		case DOUBLE_CLICK:
			log_d("BTN1 DOUBLE_CLICK");
			break;
		case LONG_PRESS_START:
			log_d("BTN1 LONG_PRESS_START");
			break;
		case LONG_PRESS_HOLD:
			log_d("BTN1 LONG_PRESS_HOLD");
			break;
		default:
			break;
	}
}


void MultiButton_callback()
{
	button_init(&btn0, read_button_GPIO, 0, btn0_id);
	button_init(&btn1, read_button_GPIO, 0, btn1_id);

	button_attach(&btn0, PRESS_DOWN,       BTN0_CALLBAKC_Handler);
	button_attach(&btn0, PRESS_UP,         BTN0_CALLBAKC_Handler);
	button_attach(&btn0, PRESS_REPEAT,     BTN0_CALLBAKC_Handler);
	button_attach(&btn0, SINGLE_CLICK,     BTN0_CALLBAKC_Handler);
	button_attach(&btn0, DOUBLE_CLICK,     BTN0_CALLBAKC_Handler);
	button_attach(&btn0, LONG_PRESS_START, BTN0_CALLBAKC_Handler);
	button_attach(&btn0, LONG_PRESS_HOLD,  BTN0_CALLBAKC_Handler);

	button_attach(&btn1, PRESS_DOWN,       BTN1_CALLBAKC_Handler);
	button_attach(&btn1, PRESS_UP,         BTN1_CALLBAKC_Handler);
	button_attach(&btn1, PRESS_REPEAT,     BTN1_CALLBAKC_Handler);
	button_attach(&btn1, SINGLE_CLICK,     BTN1_CALLBAKC_Handler);
	button_attach(&btn1, DOUBLE_CLICK,     BTN1_CALLBAKC_Handler);
	button_attach(&btn1, LONG_PRESS_START, BTN1_CALLBAKC_Handler);
	button_attach(&btn1, LONG_PRESS_HOLD,  BTN1_CALLBAKC_Handler);

	button_start(&btn0);
	button_start(&btn1);

	//make the timer invoking the button_ticks() interval 5ms.
	//This function is implemented by yourself.
	 MultiTimerStart(&btn_timer, 5, btnTimerCallback, "5ms BTN CYCLE timer");
}

