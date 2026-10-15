#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "screen.h"

typedef struct {
    uint8_t screen_count;                            // Total user screens
    uint8_t current_screen_col;
    lv_obj_t * tile_view;
    lv_group_t * encoder_group;
    const screen_theme_t * theme;
    int height;
    int width;

} dashboard_system_t;

void slot_select_confirm_cb(lv_event_t * e);
void dashboard_add_new_screen(dashboard_system_t * sys, int slots);

/**
 * Main UI Initialization
 */
dashboard_system_t * dashboard_create(lv_group_t * gr, const screen_theme_t * theme, int width, int height);

#endif
