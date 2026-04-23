
#include <lvgl.h>
#include <zephyr/kernel.h>
#include "output.h"
#include "../assets/custom_fonts.h"

LV_IMAGE_DECLARE(bt_no_signal);
LV_IMAGE_DECLARE(bt_unbonded);
LV_IMAGE_DECLARE(bt);
LV_IMAGE_DECLARE(usb);

static lv_obj_t *output_img_obj = NULL;

static void set_output_icon(lv_obj_t *parent, const lv_image_dsc_t *src, lv_coord_t x, lv_coord_t y) {
    if (!output_img_obj) {
        output_img_obj = lv_image_create(parent);
    }

    lv_image_set_src(output_img_obj, src);
    lv_obj_set_pos(output_img_obj, x, y);
}

#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
static void draw_usb_connected(lv_obj_t *parent) { set_output_icon(parent, &usb, 0, 3); }

static void draw_ble_unbonded(lv_obj_t *parent) {
    set_output_icon(parent, &bt_unbonded, 0, 1);
}
#endif

static void draw_ble_disconnected(lv_obj_t *parent) {
    set_output_icon(parent, &bt_no_signal, 0, 1);
}

static void draw_ble_connected(lv_obj_t *parent) {
    set_output_icon(parent, &bt, 0, 1);
}

void draw_output_status(lv_obj_t *parent, const struct status_state *state) {
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
