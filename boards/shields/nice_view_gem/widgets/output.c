
#include <lvgl.h>
#include <zephyr/kernel.h>
#include "output.h"
#include "../assets/custom_fonts.h"

LV_IMAGE_DECLARE(bt_no_signal);
LV_IMAGE_DECLARE(bt_unbonded);
LV_IMAGE_DECLARE(bt);
LV_IMAGE_DECLARE(usb);


#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
static void draw_usb_connected(lv_obj_t *parent) {
    lv_obj_t *img = lv_image_create(parent);
    lv_image_set_src(img, &usb);
    lv_obj_set_pos(img, 45, 2);
}

static void draw_ble_unbonded(lv_obj_t *parent) {
    lv_obj_t *img = lv_image_create(parent);
    lv_image_set_src(img, &bt_unbonded);
    lv_obj_set_pos(img, 44, 0);
}
#endif

static void draw_ble_disconnected(lv_obj_t *parent) {
    lv_obj_t *img = lv_image_create(parent);
    lv_image_set_src(img, &bt_no_signal);
    lv_obj_set_pos(img, 49, 0);
}

static void draw_ble_connected(lv_obj_t *parent) {
    lv_obj_t *img = lv_image_create(parent);
    lv_image_set_src(img, &bt);
    lv_obj_set_pos(img, 49, 0);
}

void draw_output_status(lv_obj_t *parent, const struct status_state *state) {
    // Add a white rectangle background for the signal area
    lv_obj_t *rect = lv_obj_create(parent);
    lv_obj_set_size(rect, 24, 15);
    lv_obj_set_pos(rect, 43, 0);
    lv_obj_set_style_bg_color(rect, lv_color_white(), 0);
    lv_obj_set_style_border_width(rect, 0, 0);
    lv_obj_set_style_radius(rect, 0, 0);

    // Add the "SIG" label at the same position as before
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "SIG");
    lv_obj_set_pos(label, 0, 1);
    lv_obj_set_style_text_color(label, lv_color_black(), 0);
    lv_obj_set_style_text_font(label, &pixel_operator_mono, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);

#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    switch (state->selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        draw_usb_connected(parent);
        break;

    case ZMK_TRANSPORT_BLE:
        if (state->active_profile_bonded) {
            if (state->active_profile_connected) {
                draw_ble_connected(parent);
            } else {
                draw_ble_disconnected(parent);
            }
        } else {
            draw_ble_unbonded(parent);
        }
        break;
    }
#else
    if (state->connected) {
        draw_ble_connected(parent);
    } else {
        draw_ble_disconnected(parent);
    }
#endif
}