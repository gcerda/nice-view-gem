
#include <lvgl.h>
#include <zephyr/kernel.h>
#include "profile.h"

LV_IMAGE_DECLARE(profiles);


static void draw_inactive_profiles(lv_obj_t *parent, const struct status_state *state) {
    lv_obj_t *img = lv_image_create(parent);
    lv_image_set_src(img, &profiles);
    lv_obj_set_pos(img, 18, 129 + BUFFER_OFFSET_BOTTOM);
}


static void draw_active_profile(lv_obj_t *parent, const struct status_state *state) {
    int offset = state->active_profile_index * 7;
    lv_obj_t *rect = lv_obj_create(parent);
    lv_obj_set_size(rect, 3, 3);
    lv_obj_set_pos(rect, 18 + offset, 129 + BUFFER_OFFSET_BOTTOM);
    lv_obj_set_style_bg_color(rect, lv_color_white(), 0);
    lv_obj_set_style_border_width(rect, 0, 0);
    lv_obj_set_style_radius(rect, 0, 0);
}

void draw_profile_status(lv_obj_t *parent, const struct status_state *state) {
    draw_inactive_profiles(parent, state);
    draw_active_profile(parent, state);
}