
#include <lvgl.h>
#include <zephyr/kernel.h>
#include "util.h"
#include <ctype.h>

void to_uppercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

// LVGL 9: rotate_canvas is deprecated, use lv_obj_set_style_transform_angle or lv_image_set_rotation on image objects
void rotate_canvas(lv_obj_t *obj, int16_t angle_tenths) {
    // angle_tenths: 900 means 90.0 degrees
    // LVGL 9 expects angle in 0.1 degree units
    lv_obj_set_style_transform_angle(obj, angle_tenths, 0);
}

void fill_background(lv_obj_t *obj) {
    // Set background color using style
    lv_obj_set_style_bg_color(obj, LVGL_BACKGROUND, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
}

void init_label_dsc(lv_draw_label_dsc_t *label_dsc, lv_color_t color, const lv_font_t *font,
                    lv_text_align_t align) {
    lv_draw_label_dsc_init(label_dsc);
    label_dsc->color = color;
    label_dsc->font = font;
    label_dsc->align = align;
}

void init_rect_dsc(lv_draw_rect_dsc_t *rect_dsc, lv_color_t bg_color) {
    lv_draw_rect_dsc_init(rect_dsc);
    rect_dsc->bg_color = bg_color;
}

void init_line_dsc(lv_draw_line_dsc_t *line_dsc, lv_color_t color, uint8_t width) {
    lv_draw_line_dsc_init(line_dsc);
    line_dsc->color = color;
    line_dsc->width = width;
}

void draw_line_on_canvas(lv_obj_t *canvas, const lv_point_t points[], uint32_t point_cnt,
                         lv_draw_line_dsc_t *line_dsc) {
    lv_layer_t layer;

    lv_canvas_init_layer(canvas, &layer);
    lv_draw_line(&layer, line_dsc, points);
    lv_canvas_finish_layer(canvas, &layer);
}
