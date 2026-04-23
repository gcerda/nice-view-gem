#include <zephyr/kernel.h>
#include <stdio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/battery.h>
#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <zmk/usb.h>
#include <zmk/wpm.h>

#include "battery.h"
#include "key.h"
#include "layer.h"
#include "output.h"
#include "profile.h"
#include "screen.h"
#include "wpm.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

/**
 * Draw buffers
 **/

static void draw_top(lv_obj_t *widget, lv_color_t cbuf[], const struct status_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 0);
    fill_background(canvas);

    // Draw widgets
    draw_output_status(canvas, state);
    draw_battery_status(canvas, state);
    draw_key_status(canvas, state);

    // Rotate for horizontal display (90 degrees = 900 tenths)
    rotate_canvas(canvas, 900);
}

static void draw_middle(lv_obj_t *widget, lv_color_t cbuf[], const struct status_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 1);
    fill_background(canvas);

    // Draw widgets
    draw_wpm_status(canvas, state);

    // Rotate for horizontal display
    rotate_canvas(canvas, 900);
}

static void draw_bottom(lv_obj_t *widget, lv_color_t cbuf[], const struct status_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 2);
    fill_background(canvas);

    // Draw widgets
    draw_profile_status(canvas, state);
    draw_layer_status(canvas, state);

    // Rotate for horizontal display
    rotate_canvas(canvas, 900);
}

/**
 * Battery status
 **/

static void set_battery_status(struct zmk_widget_screen *widget,
                               struct battery_status_state state) {
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    widget->state.charging = state.usb_present;
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
    widget->state.battery = state.level;

    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void battery_status_update_cb(struct battery_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_status(widget, state); }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    return (struct battery_status_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state);

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

/**
 * Layer status
 **/

static void set_layer_status(struct zmk_widget_screen *widget, struct layer_status_state state) {
    widget->state.layer_index = state.index;
    widget->state.layer_label = state.label;

    draw_bottom(widget->obj, widget->cbuf3, &widget->state);
}

static void layer_status_update_cb(struct layer_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_status(widget, state); }
}

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    return (struct layer_status_state){.index = index, .label = zmk_keymap_layer_name(index)};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

/**
 * Output status
 **/

static void set_output_status(struct zmk_widget_screen *widget,
                              const struct output_status_state *state) {
    widget->state.selected_endpoint = state->selected_endpoint;
    widget->state.active_profile_index = state->active_profile_index;
    widget->state.active_profile_connected = state->active_profile_connected;
    widget->state.active_profile_bonded = state->active_profile_bonded;

    draw_top(widget->obj, widget->cbuf, &widget->state);
    draw_bottom(widget->obj, widget->cbuf3, &widget->state);
}

static void output_status_update_cb(struct output_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_output_status(widget, &state); }
}

static struct output_status_state output_status_get_state(const zmk_event_t *_eh) {
    const struct zmk_endpoint_instance endpoint = zmk_endpoint_get_selected();

    return (struct output_status_state){
        .selected_endpoint = endpoint,
        .active_profile_index = zmk_ble_active_profile_index(),
        .active_profile_connected = zmk_ble_active_profile_is_connected(),
        .active_profile_bonded = !zmk_ble_active_profile_is_open(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_status, struct output_status_state,
                            output_status_update_cb, output_status_get_state)
ZMK_SUBSCRIPTION(widget_output_status, zmk_endpoint_changed);

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_conn_state_changed);
#endif
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_output_status, zmk_ble_active_profile_changed);
#endif

/**
 * WPM status
 **/

static void set_wpm_status(struct zmk_widget_screen *widget, struct wpm_status_state state) {
    for (int i = 0; i < 9; i++) {
        widget->state.wpm[i] = widget->state.wpm[i + 1];
    }
    widget->state.wpm[9] = state.wpm;

    draw_middle(widget->obj, widget->cbuf2, &widget->state);
}

static void wpm_status_update_cb(struct wpm_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_wpm_status(widget, state); }
}

struct wpm_status_state wpm_status_get_state(const zmk_event_t *eh) {
    return (struct wpm_status_state){.wpm = zmk_wpm_get_state()};
};

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_status, struct wpm_status_state, wpm_status_update_cb,
                            wpm_status_get_state)
ZMK_SUBSCRIPTION(widget_wpm_status, zmk_wpm_state_changed);

/**
 * Key status
 **/

struct key_status_state {
    uint16_t usage_page;
    uint32_t keycode;
    bool pressed;
};

static void format_key_text(char *out, size_t out_size, uint16_t usage_page, uint32_t keycode,
                            bool pressed) {
    if (!pressed) {
        snprintf(out, out_size, "--");
        return;
    }

    if (usage_page == HID_USAGE_KEY) {
        if (keycode >= HID_USAGE_KEY_KEYBOARD_A && keycode <= HID_USAGE_KEY_KEYBOARD_Z) {
            snprintf(out, out_size, "%c", 'A' + (keycode - HID_USAGE_KEY_KEYBOARD_A));
            return;
        }

        if (keycode >= HID_USAGE_KEY_KEYBOARD_1_AND_EXCLAMATION &&
            keycode <= HID_USAGE_KEY_KEYBOARD_9_AND_LEFT_PARENTHESIS) {
            snprintf(out, out_size, "%c",
                     '1' + (char)(keycode - HID_USAGE_KEY_KEYBOARD_1_AND_EXCLAMATION));
            return;
        }

        if (keycode == HID_USAGE_KEY_KEYBOARD_0_AND_RIGHT_PARENTHESIS) {
            snprintf(out, out_size, "0");
            return;
        }

        switch (keycode) {
        case HID_USAGE_KEY_KEYBOARD_RETURN_ENTER:
            snprintf(out, out_size, "ENTER");
            return;
        case HID_USAGE_KEY_KEYBOARD_ESCAPE:
            snprintf(out, out_size, "ESC");
            return;
        case HID_USAGE_KEY_KEYBOARD_DELETE_BACKSPACE:
            snprintf(out, out_size, "BKSP");
            return;
        case HID_USAGE_KEY_KEYBOARD_TAB:
            snprintf(out, out_size, "TAB");
            return;
        case HID_USAGE_KEY_KEYBOARD_SPACEBAR:
            snprintf(out, out_size, "SPACE");
            return;
        case HID_USAGE_KEY_KEYBOARD_LEFTSHIFT:
        case HID_USAGE_KEY_KEYBOARD_RIGHTSHIFT:
            snprintf(out, out_size, "SHIFT");
            return;
        case HID_USAGE_KEY_KEYBOARD_LEFTCONTROL:
        case HID_USAGE_KEY_KEYBOARD_RIGHTCONTROL:
            snprintf(out, out_size, "CTRL");
            return;
        case HID_USAGE_KEY_KEYBOARD_LEFTALT:
        case HID_USAGE_KEY_KEYBOARD_RIGHTALT:
            snprintf(out, out_size, "ALT");
            return;
        case HID_USAGE_KEY_KEYBOARD_LEFT_GUI:
        case HID_USAGE_KEY_KEYBOARD_RIGHT_GUI:
            snprintf(out, out_size, "GUI");
            return;
        case HID_USAGE_KEY_KEYBOARD_LEFTARROW:
            snprintf(out, out_size, "LEFT");
            return;
        case HID_USAGE_KEY_KEYBOARD_RIGHTARROW:
            snprintf(out, out_size, "RGHT");
            return;
        case HID_USAGE_KEY_KEYBOARD_UPARROW:
            snprintf(out, out_size, "UP");
            return;
        case HID_USAGE_KEY_KEYBOARD_DOWNARROW:
            snprintf(out, out_size, "DOWN");
            return;
        default:
            break;
        }
    }

    if (usage_page == HID_USAGE_CONSUMER) {
        switch (keycode) {
        case HID_USAGE_CONSUMER_VOLUME_INCREMENT:
            snprintf(out, out_size, "VOL+");
            return;
        case HID_USAGE_CONSUMER_VOLUME_DECREMENT:
            snprintf(out, out_size, "VOL-");
            return;
        case HID_USAGE_CONSUMER_MUTE:
            snprintf(out, out_size, "MUTE");
            return;
        case HID_USAGE_CONSUMER_PLAY:
        case HID_USAGE_CONSUMER_PLAY_PAUSE:
            snprintf(out, out_size, "PLAY");
            return;
        case HID_USAGE_CONSUMER_STOP:
            snprintf(out, out_size, "STOP");
            return;
        case HID_USAGE_CONSUMER_SCAN_NEXT_TRACK:
            snprintf(out, out_size, "NEXT");
            return;
        case HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK:
            snprintf(out, out_size, "PREV");
            return;
        default:
            break;
        }
    }

    snprintf(out, out_size, "0x%X", (unsigned int)keycode);
}

static void set_key_status(struct zmk_widget_screen *widget, struct key_status_state state) {
    format_key_text(widget->state.key_text, sizeof(widget->state.key_text), state.usage_page,
                    state.keycode, state.pressed);
    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void key_status_update_cb(struct key_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_key_status(widget, state); }
}

static struct key_status_state key_status_get_state(const zmk_event_t *eh) {
    static struct key_status_state state = {.usage_page = HID_USAGE_KEY, .keycode = 0, .pressed = false};
    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);

    if (ev == NULL) {
        return state;
    }

    if (ev->state) {
        state.usage_page = ev->usage_page;
        state.keycode = ev->keycode;
        state.pressed = true;
        return state;
    }

    if (state.pressed && state.usage_page == ev->usage_page && state.keycode == ev->keycode) {
        state.pressed = false;
    }

    return state;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_key_status, struct key_status_state, key_status_update_cb,
                            key_status_get_state)
ZMK_SUBSCRIPTION(widget_key_status, zmk_keycode_state_changed);

/**
 * Initialization
 **/

int zmk_widget_screen_init(struct zmk_widget_screen *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, SCREEN_HEIGHT, SCREEN_WIDTH);

    lv_obj_t *top = lv_canvas_create(widget->obj);
    lv_obj_align(top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_canvas_set_buffer(top, widget->cbuf, BUFFER_SIZE, BUFFER_SIZE, LV_COLOR_FORMAT_NATIVE);

    lv_obj_t *middle = lv_canvas_create(widget->obj);
    lv_obj_align(middle, LV_ALIGN_TOP_RIGHT, BUFFER_OFFSET_MIDDLE, 0);
    lv_canvas_set_buffer(middle, widget->cbuf2, BUFFER_SIZE, BUFFER_SIZE, LV_COLOR_FORMAT_NATIVE);

    lv_obj_t *bottom = lv_canvas_create(widget->obj);
    lv_obj_align(bottom, LV_ALIGN_TOP_RIGHT, BUFFER_OFFSET_BOTTOM, 0);
    lv_canvas_set_buffer(bottom, widget->cbuf3, BUFFER_SIZE, BUFFER_SIZE, LV_COLOR_FORMAT_NATIVE);

    snprintf(widget->state.key_text, sizeof(widget->state.key_text), "--");
    sys_slist_append(&widgets, &widget->node);
    widget_battery_status_init();
    widget_layer_status_init();
    widget_output_status_init();
    widget_wpm_status_init();

    return 0;
}

lv_obj_t *zmk_widget_screen_obj(struct zmk_widget_screen *widget) { return widget->obj; }
