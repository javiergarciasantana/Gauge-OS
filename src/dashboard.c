#include "dashboard.h"

static int configured_screen_count = 0;
static int current_tile_col = 0;

/**
 * Key Event Callback: Allows switching TileView screens using Encoder / Arrow Keys
 */
static void tile_key_nav_cb(lv_event_t * e) {
    screen_t * s = (screen_t *)lv_event_get_user_data(e);

    uint32_t key = lv_event_get_key(e);
    int total_tiles = configured_screen_count + 2; // Gauges + Add + Settings

    if (key == LV_KEY_RIGHT) {
        if (current_tile_col < total_tiles - 1) {
            current_tile_col++;
            lv_tileview_set_tile_by_index(s->tile_view, current_tile_col, 0, LV_ANIM_ON);
        }
    } else if (key == LV_KEY_LEFT) {
        if (current_tile_col > 0) {
            current_tile_col--;
            lv_tileview_set_tile_by_index(s->tile_view, current_tile_col, 0, LV_ANIM_ON);
        }
    }
}

/**
 * Modal confirm event callback(non static so that it can be linked)
 */
void slot_select_confirm_cb(lv_event_t * e) {
    dashboard_system_t * sys = (dashboard_system_t *)lv_event_get_user_data(e);
    lv_obj_t * roller = lv_event_get_target(e);

    uint16_t selected_slots = lv_roller_get_selected(roller) + 1;

    lv_group_remove_obj(roller);
    lv_obj_delete(roller);

    // Dashboard creates the new screen directly
    dashboard_add_new_screen(sys, selected_slots);
}

static void reposition_screens(dashboard_system_t * sys) {
      // 4. Reposition Add (+) and Settings tiles to stay on the far right
      uint32_t total_tiles = lv_obj_get_child_count(sys->tile_view);

      for (uint32_t i = 0; i < total_tiles; i++) {
        lv_obj_t * tile = lv_obj_get_child(sys->tile_view, i);
        // For now to identify, get the tile user_data
        screen_type_t type = (screen_type_t)(uintptr_t)lv_obj_get_user_data(tile);
        if (type  == SETTINGS) {
          lv_obj_set_pos(tile, (configured_screen_count + 1) * 480, 0);
        } else if (type == EMPTY) {
          lv_obj_set_pos(tile, configured_screen_count * 480, 0);
        }
    }
}

void dashboard_add_new_screen(dashboard_system_t * sys, int slots) {
    // 1. Screen module creates the tile and its slot buttons
    screen_t * scr = screen_create(sys->tile_view, POPULATED, sys->theme, sys->encoder_group, slots, sys->screen_count, sys);

    // 2. Dashboard attaches its OWN navigation callback to the tile object
    lv_obj_add_event_cb(scr->tile, tile_key_nav_cb, LV_EVENT_KEY, sys);

    // 3. Dashboard manages layout and focus
    sys->screen_count++;
    reposition_screens(sys);
    lv_tileview_set_tile_by_index(sys->tile_view, sys->screen_count - 1, 0, LV_ANIM_ON);
}

/**
 * Main Dynamic Carousel UI Initialization
 */
dashboard_system_t * dashboard_create(lv_group_t * gr, const screen_theme_t * theme, int width, int height) {
    lv_obj_t * tileview = lv_tileview_create(lv_screen_active());
    lv_obj_remove_style_all(tileview);
    lv_obj_set_size(tileview, width , height);

    dashboard_system_t * sys = (dashboard_system_t *)lv_malloc(sizeof(dashboard_system_t));
    LV_ASSERT_MALLOC(sys);
    sys->height = height;
    sys->width = width;
    sys->encoder_group = gr;
    sys->theme = theme;
    sys->tile_view = tileview;
    sys->screen_count = 0;
    sys->current_screen_col = 0;

    screen_t * emmpty_screen = screen_create(tileview, EMPTY, theme, gr, 0, sys->screen_count ,sys);
    screen_t * settings_screen = screen_create(tileview, SETTINGS, theme, gr, 0, sys->screen_count ,sys);

    lv_obj_add_event_cb(tileview, tile_key_nav_cb, LV_EVENT_KEY, NULL);
    lv_tileview_set_tile_by_index(tileview, 0, 0, LV_ANIM_OFF);

    return sys;
}
