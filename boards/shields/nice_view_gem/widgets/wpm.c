
#include <lvgl.h>
#include <math.h>
#include <zephyr/kernel.h>
#include "wpm.h"
#include "../assets/custom_fonts.h"

LV_IMAGE_DECLARE(gauge);
LV_IMAGE_DECLARE(grid);


static void draw_gauge(lv_obj_t *parent, const struct status_state *state) {
    lv_obj_t *img = lv_image_create(parent);
    lv_image_set_src(img, &gauge);
    lv_obj_set_pos(img, 16, 44 + BUFFER_OFFSET_MIDDLE);
}

static void draw_needle(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_line_dsc_t line_dsc;
    init_line_dsc(&line_dsc, LVGL_FOREGROUND, 1);

    int centerX = 33;
    int centerY = 67 + BUFFER_OFFSET_MIDDLE;
    int offset = 13;
    int value = state->wpm[9];

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_WPM_FIXED_RANGE)
    float max = CONFIG_NICE_VIEW_GEM_WPM_FIXED_RANGE_MAX;
#else
    float max = 0;
    for (int i = 0; i < 10; i++) {
        if (state->wpm[i] > max) {
            max = state->wpm[i];
        }
    }
#endif
    if (max == 0)
        max = 100;
    if (value < 0)
        value = 0;
    if (value > max)
        value = max;

    float radius = 25.45585;
    float angleDeg = 225 + ((float)value / max) * 90;
    float angleRad = angleDeg * (3.14159 / 180.0f);

    int needleStartX = centerX + (int)(offset * cos(angleRad));
    int needleStartY = centerY + (int)(offset * sin(angleRad));
    int needleEndX = centerX + (int)(radius * cos(angleRad));
    int needleEndY = centerY + (int)(radius * sin(angleRad));

    lv_point_t points[2] = {{needleStartX, needleStartY}, {needleEndX, needleEndY}};
    lv_canvas_draw_line(canvas, points, 2, &line_dsc);
}


static void draw_grid(lv_obj_t *parent) {
    lv_obj_t *img = lv_image_create(parent);
    lv_image_set_src(img, &grid);
    lv_obj_set_pos(img, 0, 65 + BUFFER_OFFSET_MIDDLE);
}

static void draw_graph(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_line_dsc_t line_dsc;
    init_line_dsc(&line_dsc, LVGL_FOREGROUND, 2);
    lv_point_t points[10];

    int baselineY = 97 + BUFFER_OFFSET_MIDDLE;

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_WPM_FIXED_RANGE)
    int max = CONFIG_NICE_VIEW_GEM_WPM_FIXED_RANGE_MAX;
    if (max == 0) {
        max = 100;
    }

    int value = 0;
    for (int i = 0; i < 10; i++) {
        value = state->wpm[i];
        if (value > max) {
            value = max;
        }
        points[i].x = 0 + i * 7.4;
        points[i].y = baselineY - (value * 32 / max);
    }
#else
    int max = 0;
    int min = 256;

    for (int i = 0; i < 10; i++) {
        if (state->wpm[i] > max) {
            max = state->wpm[i];
        }
        if (state->wpm[i] < min) {
            min = state->wpm[i];
        }
    }

    int range = max - min;
    if (range == 0) {
        range = 1;
    }

    for (int i = 0; i < 10; i++) {
        points[i].x = 0 + i * 7.4;
        points[i].y = baselineY - (state->wpm[i] - min) * 32 / range;
    }
#endif

    lv_canvas_draw_line(canvas, points, 10, &line_dsc);
}


static void draw_label(lv_obj_t *parent, const struct status_state *state) {
    // "WPM" label (left-aligned)
    lv_obj_t *label_left = lv_label_create(parent);
    lv_label_set_text(label_left, "WPM");
    lv_obj_set_pos(label_left, 0, 101 + BUFFER_OFFSET_MIDDLE);
    lv_obj_set_style_text_color(label_left, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_left, &pixel_operator_mono, 0);
    lv_obj_set_style_text_align(label_left, LV_TEXT_ALIGN_LEFT, 0);

    // WPM value label (right-aligned)
    lv_obj_t *label_right = lv_label_create(parent);
    char wpm_text[6] = {};
    snprintf(wpm_text, sizeof(wpm_text), "%d", state->wpm[9]);
    lv_label_set_text(label_right, wpm_text);
    lv_obj_set_pos(label_right, 26, 101 + BUFFER_OFFSET_MIDDLE);
    lv_obj_set_style_text_color(label_right, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_right, &pixel_operator_mono, 0);
    lv_obj_set_style_text_align(label_right, LV_TEXT_ALIGN_RIGHT, 0);
}

void draw_wpm_status(lv_obj_t *parent, const struct status_state *state) {
    draw_gauge(parent, state);
    draw_needle(parent, state);
    draw_grid(parent);
    draw_graph(parent, state);
    draw_label(parent, state);
}