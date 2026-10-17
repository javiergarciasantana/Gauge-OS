#ifndef GAUGE_H
#define GAUGE_H

#include "lvgl/lvgl.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
#define MAX_WIDTH        480
#define SLOT_WIDTH       160
#define SLOT_HEIGHT      128
#define LOWERING_AMM     50
#define GAUGE_MAX_TICKS  10
#define GAUGE_MAX_SLOTS  3
#define MAX_ACTIVE_GAUGES 12

// OEM Color Palette
#define COLOR_VAG_GR      lv_color_hex(0x474b4e)
#define COLOR_VAG_DARK_GR lv_color_hex(0x43454b)
#define COLOR_VAG_WHITE   lv_color_hex(0xFFFFFF)
#define COLOR_VAG_RED     lv_color_hex(0xE60000)

LV_FONT_DECLARE(ui_font_futura_bold);

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    PARAM_OIL_PRESS,
    PARAM_OIL_TEMP,
    PARAM_WATER_TEMP,
    PARAM_BATTERY_VOLTAGE
} param_type_t;

// 1. Static Configuration Descriptor (Styling & Specs)

typedef struct {
    lv_color_t screen_bg;       // Tile background
    lv_color_t card_bg;         // Gauge container background
    lv_color_t text_primary;    // Active digit/text color
    lv_color_t border_color;    // Frame border
    const lv_font_t * font_big; // Font for values
    bool show_analog_dial;      // True for needle/dial, False for digital LCD
} ui_theme_t;

typedef struct {
    param_type_t type;
    float min_val;
    float max_val;
    int mid_val_pos;

    float red_line_begin;
    float red_line_end;
    int n_ticks;
    const char * unit;

    const lv_image_dsc_t * bg_image;
    const lv_image_dsc_t * icon_image;
    uint32_t redline_color;
    bool show_redline;
} gauge_config_t;

// 2. Dynamic Runtime Instance State
typedef struct {
    lv_obj_t * scale;
    lv_obj_t * needle;
    lv_obj_t * label;

    gauge_config_t config; // Copy of this gauge's parameters

    float value;
    float current_needle_angle_rad;

    // Persistent string memory allocated at compile-time
    char start_label[8];
    char mid_label[8];
    char end_label[8];
    const char * custom_labels[GAUGE_MAX_TICKS];
} gauge_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Creates a gauge inside a slot (0, 1, or 2) using a configuration descriptor
 */
gauge_t * gauge_create(lv_obj_t * parent, int slot_index, const gauge_config_t * cfg);

/**
 * Updates needle position and readout text using internal float math
 */
void gauge_update_value(gauge_t * g, float new_val);


/**
 * Calls gauge_update_value to update all gauges that have the same param_type_t
 */
void gauge_update_value_by_type(param_type_t type, float new_val);



// Configurations
extern const gauge_config_t OIL_PRESS_CONFIG;
extern const gauge_config_t WATER_TEMP_CONFIG;
extern const gauge_config_t BATTERY_VOLTAGE_CONFIG;
extern const gauge_config_t OIL_TEMP_CONFIG;


#endif /* GAUGE_H */
