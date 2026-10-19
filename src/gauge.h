#ifndef GAUGE_H
#define GAUGE_H

#include <assert.h>
#include <stdbool.h>
#include "theme.h"


/*********************
 *      DEFINES
 *********************/
#define MAX_WIDTH        480
#define SLOT_WIDTH       160
#define SLOT_HEIGHT      128
#define GAUGE_MAX_TICKS  10
#define MAX_ACTIVE_GAUGES 12

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
    param_type_t type;
    float min_val;
    float max_val;
    int mid_val_pos;

    uint8_t ticks_period;

    bool show_redline;

    float red_line_begin;
    float red_line_end;
    int n_ticks;
    const char * unit;

    const lv_image_dsc_t * bg_image;
    const lv_image_dsc_t * icon_image;
} gauge_config_t;

// 2. Dynamic Runtime Instance State
typedef struct {
    int width;
    int height;
    lv_obj_t * scale;
    lv_obj_t * needle;
    lv_obj_t * label;

    gauge_theme_t theme;
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
gauge_t * gauge_create(lv_obj_t * parent, const int x_pos, const int width, const int height, const gauge_theme_t * theme, const gauge_config_t * cfg);

/**
 * Updates needle position and readout text using internal float math
 */
void gauge_update_value(gauge_t * g, float new_val);


/**
 * Calls gauge_update_value to update all gauges that have the same param_type_t
 */
void gauge_update_value_by_type(param_type_t type, float new_val);

/**
 * Prints all gauge_t attributes and also the gauge_config_t attributes
 */

void gauge_print(const gauge_t *g);

// Configurations
extern const gauge_theme_t GAUGE_ANALOGUE;
extern const gauge_theme_t GAUGE_DIGITAL;

extern const gauge_config_t OIL_PRESS_CONFIG;
extern const gauge_config_t WATER_TEMP_CONFIG;
extern const gauge_config_t BATTERY_VOLTAGE_CONFIG;
extern const gauge_config_t OIL_TEMP_CONFIG;


#endif /* GAUGE_H */
