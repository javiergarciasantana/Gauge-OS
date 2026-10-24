#include "dashboard.h"

/**********************
 * STATIC FUNCTIONS
 **********************/

/**
 * Key Event Callback: Allows switching TileView screens using Encoder / Arrow Keys
 */
static void tile_key_nav_cb(lv_event_t * e) {
    dashboard_system_t * sys = (dashboard_system_t *)lv_event_get_user_data(e);
    if (!sys) return;
    uint32_t key = lv_event_get_key(e);
    int total_tiles = sys->screen_count + 2; // Gauges + Add + Settings

    if (key == LV_KEY_RIGHT) {
        if (sys->current_screen_col < total_tiles - 1) {
            sys->current_screen_col++;
            lv_tileview_set_tile_by_index(sys->tile_view, sys->current_screen_col, 0, LV_ANIM_ON);
        }
    } else if (key == LV_KEY_LEFT) {
        if (sys->current_screen_col > 0) {
            sys->current_screen_col--;
            lv_tileview_set_tile_by_index(sys->tile_view, sys->current_screen_col, 0, LV_ANIM_ON);
        }
    }
}

/**
 * Modal change theme(non static so that it can be linked)
 */
static void slot_select_theme_cb(lv_event_t * e) {
  dashboard_system_t * sys = (dashboard_system_t *)lv_event_get_user_data(e);
  lv_obj_t * roller = lv_event_get_target(e);

  uint16_t selected_slot = lv_roller_get_selected(roller);
  sys->theme = theme_get_by_id(selected_slot);

  int total_screens = sys->screen_count + 2;
  for (int i = 0; i < total_screens; ++i) {
    screen_update(sys->screens[i], sys->theme);
  }
}

/**
 * Modal confirm event callback(non static so that it can be linked)
 */
static void slot_select_confirm_cb(lv_event_t * e) {
    dashboard_system_t * sys = (dashboard_system_t *)lv_event_get_user_data(e);
    lv_obj_t * roller = lv_event_get_target(e);

    uint16_t selected_slots = lv_roller_get_selected(roller) + 1;

    lv_group_remove_obj(roller);
    lv_obj_delete(roller);

    if (selected_slots > sys->max_n_gauges_per_screen) return;
    // Dashboard creates the new screen directly
    dashboard_add_new_screen(sys, selected_slots);
}

/*
  Repositions Add (+) and Settings tiles to stay on the far right
*/
static void reposition_screens(dashboard_system_t * sys) {
  uint32_t total_tiles = lv_obj_get_child_count(sys->tile_view);

  for (uint32_t i = 0; i < total_tiles; i++) {
    lv_obj_t * tile = lv_obj_get_child(sys->tile_view, i);
    screen_type_t type = (screen_type_t)(uintptr_t)lv_obj_get_user_data(tile);
    if (type  == SETTINGS) {
      lv_obj_set_pos(tile, (sys->screen_count + 1) * sys->width, 0);
    } else if (type == EMPTY) {
      lv_obj_set_pos(tile, 0 * sys->width, 0);
    }
  }
}

static void dashboard_add_new_screen(dashboard_system_t * sys, int slots) {
  int new_col = sys->screen_count;
  // 1. Screen module creates the tile and its slot buttons
  screen_t * scr = screen_create(sys->tile_view, POPULATED, sys->theme, sys->encoder_group, slots, new_col + 1, sys->width, sys->height, sys->max_n_gauges_per_screen, sys);
  sys->screens[sys->screen_count + 2] = scr;
  // 2. Dashboard attaches its OWN navigation callback to the tile object
  lv_obj_add_event_cb(scr->tile, tile_key_nav_cb, LV_EVENT_KEY, sys);

  // 3. Dashboard manages layout and focus
  sys->screen_count++;
  reposition_screens(sys);

  sys->current_screen_col = new_col;
  lv_tileview_set_tile_by_index(sys->tile_view, sys->current_screen_col, 0, LV_ANIM_ON);
  lv_group_focus_obj(scr->tile);
}

/**********************
 * NON STATIC FUNCTIONS
 **********************/

/**
 * Main Dynamic Carousel UI Initialization
 */
dashboard_system_t * dashboard_create(lv_group_t * gr, const screen_theme_t * theme, int width, int height, uint8_t max_guges) {
  // Set background color
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(theme->main_bgnd), LV_PART_MAIN);
  lv_obj_t * tileview = lv_tileview_create(lv_screen_active());
  lv_obj_remove_style_all(tileview);
  lv_obj_set_size(tileview, width , height);

  dashboard_system_t * sys = (dashboard_system_t *)lv_malloc(sizeof(dashboard_system_t));
  LV_ASSERT_MALLOC(sys);
  sys->height = height;
  sys->width = width;
  sys->max_n_gauges_per_screen = max_guges;
  sys->encoder_group = gr;
  sys->tile_view = tileview;
  sys->theme = theme;
  sys->screen_count = 0;
  sys->current_screen_col = 0;

  screen_t * empty_screen = screen_create(tileview, EMPTY, theme, gr, 0, 0, sys->width, sys->height, 0, sys);
  screen_t * settings_screen = screen_create(tileview, SETTINGS, theme, gr, 0, 1, sys->width, sys->height, 0, sys);
  sys->screens[0] = settings_screen;
  sys->screens[1] = empty_screen;


  /// Attach key navigation callback to static tiles as well!
  lv_obj_add_event_cb(empty_screen->tile,tile_key_nav_cb, LV_EVENT_KEY, sys);
  lv_obj_add_event_cb(settings_screen->tile, tile_key_nav_cb, LV_EVENT_KEY, sys);

  lv_tileview_set_tile_by_index(tileview, 0, 0, LV_ANIM_OFF);

  return sys;
}
