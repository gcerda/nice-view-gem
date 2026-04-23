
#include <lvgl.h>
#include <zephyr/kernel.h>
#include "battery.h"
#include "../assets/custom_fonts.h"

LV_IMAGE_DECLARE(bolt);

static lv_obj_t *battery_label_obj = NULL;
static lv_obj_t *battery_body_obj = NULL;
static lv_obj_t *battery_tip_obj = NULL;
static lv_obj_t *battery_fill_obj = NULL;
static lv_obj_t *charge_img_obj = NULL;

static void init_battery_objects(lv_obj_t *parent) {
    if (!battery_body_obj) {
        battery_body_obj = lv_obj_create(parent);
        lv_obj_set_size(battery_body_obj, 10, 6);
        lv_obj_set_pos(battery_body_obj, 29, 5);
        lv_obj_set_style_bg_opa(battery_body_obj, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_color(battery_body_obj, LVGL_FOREGROUND, 0);
        lv_obj_set_style_border_width(battery_body_obj, 1, 0);
        lv_obj_set_style_radius(battery_body_obj, 0, 0);
        lv_obj_set_style_pad_all(battery_body_obj, 0, 0);
    }

    if (!battery_tip_obj) {
        battery_tip_obj = lv_obj_create(parent);
        lv_obj_set_size(battery_tip_obj, 2, 3);
        lv_obj_set_pos(battery_tip_obj, 39, 6);
        lv_obj_set_style_bg_color(battery_tip_obj, LVGL_FOREGROUND, 0);
        lv_obj_set_style_border_width(battery_tip_obj, 0, 0);
        lv_obj_set_style_radius(battery_tip_obj, 0, 0);
    }

    if (!battery_fill_obj) {
        battery_fill_obj = lv_obj_create(parent);
        lv_obj_set_pos(battery_fill_obj, 31, 7);
        lv_obj_set_style_bg_color(battery_fill_obj, LVGL_FOREGROUND, 0);
        lv_obj_set_style_border_width(battery_fill_obj, 0, 0);
        lv_obj_set_style_radius(battery_fill_obj, 0, 0);
    }

    if (!battery_label_obj) {
        battery_label_obj = lv_label_create(parent);
        lv_obj_set_pos(battery_label_obj, 44, 1);
        lv_obj_set_style_text_color(battery_label_obj, LVGL_FOREGROUND, 0);
        lv_obj_set_style_text_font(battery_label_obj, &pixel_operator_mono, 0);
        lv_obj_set_style_text_align(battery_label_obj, LV_TEXT_ALIGN_RIGHT, 0);
    }

    if (!charge_img_obj) {
        charge_img_obj = lv_image_create(parent);
        lv_image_set_src(charge_img_obj, &bolt);
        lv_obj_set_pos(charge_img_obj, 22, 2);
    }
}

static void draw_level(lv_obj_t *parent, const struct status_state *state) {
    char text[10] = {};
    sprintf(text, "%i%%", state->battery);
    init_battery_objects(parent);
    lv_label_set_text(battery_label_obj, text);

    uint8_t fill_width = (uint8_t)((state->battery * 8U) / 100U);
    if (state->battery > 0 && fill_width == 0) {
        fill_width = 1;
    }

    lv_obj_set_size(battery_fill_obj, fill_width, 2);
    lv_obj_add_flag(charge_img_obj, LV_OBJ_FLAG_HIDDEN);
}

void draw_battery_status(lv_obj_t *parent, const struct status_state *state) {
    draw_level(parent, state);

    if (state->charging) {
        lv_obj_clear_flag(charge_img_obj, LV_OBJ_FLAG_HIDDEN);
    }
}
