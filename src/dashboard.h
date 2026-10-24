#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "screen.h"

/**********************
 * DEFINES
 **********************/
#define MAX_SAVED_SCREENS 5

/**********************
 * TYPEDEFS
 **********************/
typedef struct {
  const screen_theme_t * theme;
  lv_obj_t * tile_view;
  lv_group_t * encoder_group;
  uint8_t max_n_gauges_per_screen;
  uint8_t screen_count;// Total user screens
  uint8_t current_screen_col;
  int height;
  int width;

  screen_t * screens[MAX_SAVED_SCREENS]; //Not in order, just to access and change theme

} dashboard_system_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Main UI Initialization
 */
dashboard_system_t * dashboard_create(lv_group_t * gr, const screen_theme_t * theme, int width, int height, uint8_t max_guges);

#endif
