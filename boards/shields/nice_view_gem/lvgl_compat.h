// Compatibility shim to allow this module to compile against LVGL v8 and v9
#ifndef NICE_VIEW_GEM_LVGL_COMPAT_H
#define NICE_VIEW_GEM_LVGL_COMPAT_H

#include <lvgl.h>

/* Map LVGL9 names to LVGL8 when building against older LVGL
 * Provide typedefs/macros only when the LVGL9 symbol is not available.
 */

/* Image declare macro */
#ifndef LV_IMAGE_DECLARE
#ifdef LV_IMG_DECLARE
#define LV_IMAGE_DECLARE LV_IMG_DECLARE
#endif
#endif

/* Image header magic */
#ifndef LV_IMAGE_HEADER_MAGIC
#ifdef LV_IMG_HEADER_MAGIC
#define LV_IMAGE_HEADER_MAGIC LV_IMG_HEADER_MAGIC
#endif
#endif

/* Color format constants */
#ifndef LV_COLOR_FORMAT_I1
#ifdef LV_IMG_CF_INDEXED_1BIT
#define LV_COLOR_FORMAT_I1 LV_IMG_CF_INDEXED_1BIT
#endif
#endif

#ifndef LV_COLOR_FORMAT_NATIVE
#ifdef LV_IMG_CF_TRUE_COLOR
#define LV_COLOR_FORMAT_NATIVE LV_IMG_CF_TRUE_COLOR
#endif
#endif

/* Image descriptor type alias */
#ifndef lv_image_dsc_t
#ifdef lv_img_dsc_t
typedef lv_img_dsc_t lv_image_dsc_t;
#endif
#endif

/* Function/API aliases */
#ifndef lv_image_create
#ifdef lv_img_create
#define lv_image_create lv_img_create
#endif
#endif

#ifndef lv_image_set_src
#ifdef lv_img_set_src
#define lv_image_set_src lv_img_set_src
#endif
#endif

/* animimg helpers (older LVGL may use lv_animimg_*) */
#ifndef lv_animimg_create
#ifdef lv_animimg_create
/* already exists */
#elif defined(lv_animimg_create)
/* noop */
#endif
#endif

#endif /* NICE_VIEW_GEM_LVGL_COMPAT_H */
