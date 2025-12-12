
#include <lvgl.h>
#include <zephyr/kernel.h>
#include "battery.h"
#include "../assets/custom_fonts.h"

LV_IMAGE_DECLARE(bolt);


static void draw_level(lv_obj_t *parent, const struct status_state *state) {
    // Battery percentage label (right-aligned)
    lv_obj_t *label = lv_label_create(parent);
    char text[10] = {};
    sprintf(text, "%i%%", state->battery);
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, 26, 19);
    lv_obj_set_style_text_color(label, lv_color_black(), 0);
    lv_obj_set_style_text_font(label, &pixel_operator_mono, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
}

static void draw_charging_level(lv_obj_t *parent, const struct status_state *state) {
    // Battery percentage label (right-aligned)
    lv_obj_t *label = lv_label_create(parent);
    char text[10] = {};
    sprintf(text, "%i%%", state->battery);
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, 26, 19);
    lv_obj_set_style_text_color(label, lv_color_black(), 0);
    lv_obj_set_style_text_font(label, &pixel_operator_mono, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
    // Charging bolt icon
    lv_obj_t *img = lv_image_create(parent);
    lv_image_set_src(img, &bolt);
    lv_obj_set_pos(img, 62, 21);
}

void draw_battery_status(lv_obj_t *parent, const struct status_state *state) {
    // "BAT" label (left-aligned)
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "BAT");
    lv_obj_set_pos(label, 0, 19);
    lv_obj_set_style_text_color(label, lv_color_black(), 0);
    lv_obj_set_style_text_font(label, &pixel_operator_mono, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);

    if (state->charging) {
        draw_charging_level(parent, state);
    } else {
        draw_level(parent, state);
    }
}