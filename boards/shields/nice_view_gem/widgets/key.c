#include <zephyr/kernel.h>
#include "key.h"
#include "../assets/custom_fonts.h"

static lv_obj_t *key_label_obj = NULL;

void draw_key_status(lv_obj_t *parent, const struct status_state *state) {
    if (!key_label_obj) {
        key_label_obj = lv_label_create(parent);
        lv_obj_set_pos(key_label_obj, 0, 19);
        lv_obj_set_width(key_label_obj, 68);
        lv_obj_set_style_text_color(key_label_obj, LVGL_FOREGROUND, 0);
        lv_obj_set_style_text_font(key_label_obj, &pixel_operator_mono, 0);
        lv_obj_set_style_text_align(key_label_obj, LV_TEXT_ALIGN_CENTER, 0);
    }

    lv_label_set_text(key_label_obj, state->key_text);
}
