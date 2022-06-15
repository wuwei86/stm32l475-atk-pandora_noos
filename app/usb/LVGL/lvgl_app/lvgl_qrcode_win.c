#include "../../lv_examples.h"
#if LV_USE_QRCODE && LV_BUILD_EXAMPLES

lv_obj_t *qrcode_win_contanier = NULL;
lv_obj_t * qr = NULL;
/**
 * Create a QR Code
 */
void lv_show_qrcode(void)
{
    qrcode_win_contanier = lv_obj_create(lv_scr_act());  // 基于屏幕创建一个和屏幕大小一样的容器win1_contanier
    if (qrcode_win_contanier != NULL)
    {
        //lv_obj_set_style_bg_color(qrcode_win_contanier, lv_palette_main(LV_PALETTE_YELLOW), 0);
        lv_obj_set_size(qrcode_win_contanier, 240, 240);
        lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
        lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

        qr = (lv_obj_t *)lv_qrcode_create(qrcode_win_contanier, 150, fg_color, bg_color);
    // 
        /*Set data*/
        const char * data = "https://www.hao123.com";
        lv_qrcode_update(qr, data, strlen(data));
        lv_obj_center(qr);

        /*Add a border with bg_color*/
        lv_obj_set_style_border_color(qr, bg_color, 0);
        lv_obj_set_style_border_width(qr, 5, 0);

        
    }
}

// static void btn_event_cb(lv_event_t * e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     lv_obj_t * btn = lv_event_get_target(e);
//     if(code == LV_EVENT_CLICKED) {
//         static uint8_t cnt = 0;
//         cnt++;

//         /*Get the first child of the button which is the label and change its text*/
//         lv_obj_t * label = lv_obj_get_child(btn, 0);
//         lv_label_set_text_fmt(label, "Button: %d", cnt);
//     }
// }

// /**
//  * Create a button with a label and react on click event.
//  */
// void lv_show_qrcode(void)
// {
//     qrcode_win_contanier = lv_obj_create(lv_scr_act());  // 基于屏幕创建一个和屏幕大小一样的容器win1_contanier
//     if (qrcode_win_contanier != NULL)
//     {
//         //lv_obj_set_style_bg_color(qrcode_win_contanier, lv_palette_main(LV_PALETTE_YELLOW), 0);
//         lv_obj_set_size(qrcode_win_contanier, 240, 240);
//         lv_obj_t * btn = lv_btn_create(qrcode_win_contanier);     /*Add a button the current screen*/
//         lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
//         lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
//         lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

//         lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
//         lv_label_set_text(label, "Button");                     /*Set the labels text*/
//         lv_obj_center(label);
//     }
// }

#endif