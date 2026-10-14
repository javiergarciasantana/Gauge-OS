#ifndef SCREENS_H
#define SCREENS_H

#include "gauge.h"
#include <stdio.h>

#define MAX_SAVED_SCREENS 5
#define MAX_GAUGES_PER_SCREEN 3

// Saved layout representation
typedef struct {
    uint8_t gauge_count;                             // 1, 2, or 3
    uint8_t param_types[MAX_GAUGES_PER_SCREEN];     // Stores PARAM_OIL_PRESS, etc.
} saved_screen_t;

typedef struct {
    uint8_t screen_count;                            // Total user screens
    uint8_t theme_mode;                             // THEME_ANALOG_GAUGE or THEME_VW_GAMMA_LCD
    saved_screen_t screens[MAX_SAVED_SCREENS];
} dashboard_save_data_t;

/**
 * Main UI Initialization
 */
void system_ui_init(lv_group_t * g);


#endif
