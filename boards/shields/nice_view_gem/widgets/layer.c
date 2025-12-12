#include <zephyr/kernel.h>
#include "layer.h"
#include "../assets/custom_fonts.h"

// Persistent label for layer status
static lv_obj_t *layer_label_obj = NULL;

void draw_layer_status(lv_obj_t *parent, const struct status_state *state) {
    char text[10] = {};
    if (state->layer_label == NULL) {
        sprintf(text, "Layer %i", state->layer_index);
    } else {
        strncpy(text, state->layer_label, 9);
        to_uppercase(text);
    }

    if (!layer_label_obj) {
        layer_label_obj = lv_label_create(parent);
        lv_obj_set_pos(layer_label_obj, 0, 146 + BUFFER_OFFSET_BOTTOM);
        lv_obj_set_width(layer_label_obj, 68);
        lv_obj_set_style_text_color(layer_label_obj, LVGL_FOREGROUND, 0);
        lv_obj_set_style_text_font(layer_label_obj, &pixel_operator_mono, 0);
        lv_obj_set_style_text_align(layer_label_obj, LV_TEXT_ALIGN_CENTER, 0);
    }
    lv_label_set_text(layer_label_obj, text);
}