#include "screens.h"

static lv_obj_t * tileview;
static lv_group_t * encoder_group;

static int configured_screen_count = 0;
static int current_tile_col = 0;

static lv_obj_t * tile_add;
static lv_obj_t * tile_settings;

static void create_add_tile_content(lv_obj_t * parent);
static void create_settings_tile_content(lv_obj_t * parent);

/**
 * Key Event Callback: Allows switching TileView screens using Encoder / Arrow Keys
 */
static void tile_key_nav_cb(lv_event_t * e) {
    uint32_t key = lv_event_get_key(e);
    int total_tiles = configured_screen_count + 2; // Gauges + Add + Settings

    if (key == LV_KEY_RIGHT) {
        if (current_tile_col < total_tiles - 1) {
            current_tile_col++;
            lv_tileview_set_tile_by_index(tileview, current_tile_col, 0, LV_ANIM_ON);
        }
    } else if (key == LV_KEY_LEFT) {
        if (current_tile_col > 0) {
            current_tile_col--;
            lv_tileview_set_tile_by_index(tileview, current_tile_col, 0, LV_ANIM_ON);
        }
    }
}

static void slot_select_confirm_gauge_cb(lv_event_t * e) {

  lv_obj_t * roller = (lv_obj_t *)lv_event_get_user_data(e);

  // Read selected count (Index 0=1 gauge, 1=2 gauges, 2=3 gauges)
  uint16_t selected_cfg = lv_roller_get_selected(roller);

  // Retrieve btn_unit stored in btn_ok
  lv_obj_t * btn_unit = (lv_obj_t *)lv_obj_get_user_data(roller);

  // Retrieve x_pos stored inside btn_unit
  int x_pos = (int)(uintptr_t)lv_obj_get_user_data(btn_unit);

  lv_obj_t * tile_parent = lv_obj_get_parent(roller);

  // 1. MUST remove objects from encoder group BEFORE deleting parent container
  lv_group_remove_obj(roller);

  lv_obj_delete(roller);
  lv_obj_delete(btn_unit);

  const gauge_config_t * cfg = NULL;

  switch (selected_cfg)
  {
  case 0:
    cfg = &OIL_PRESS_CONFIG;
    break;
  case 1:
    cfg = &OIL_TEMP_CONFIG;
  break;
  case 2:
    cfg = &WATER_TEMP_CONFIG;
  break;
  case 3:
    cfg = &BATTERY_VOLTAGE_CONFIG;
  break;
  default:
    break;
  }

  gauge_create(tile_parent, x_pos, cfg);
}

static void slot_select_unit_modal_cb(lv_event_t * e) {

    //int x_pos = (int)(uintptr_t)lv_event_get_user_data(e);

    lv_obj_t * btn_unit = lv_event_get_target(e);
    lv_obj_t * parent = lv_obj_get_parent(btn_unit);

    // Retrieve x_pos stored inside btn_unit
    int x_pos = (int)(uintptr_t)lv_obj_get_user_data(btn_unit);

    // 2. Create Roller
    lv_obj_t * roller = lv_roller_create(parent);
    lv_roller_set_options(roller, "Oil Pressure\n Oil Temp\nCoolant temp\n Battery volt", LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(roller, 2);
    lv_obj_set_style_bg_color(roller, COLOR_VAG_DARK_GR, LV_PART_MAIN);
    lv_obj_set_style_bg_color(roller, COLOR_VAG_RED, LV_PART_SELECTED);
    lv_obj_set_style_text_font(roller, &ui_font_futura_bold, LV_PART_MAIN);
    lv_obj_align_to(roller, btn_unit, LV_ALIGN_CENTER, 0, 0);

    // Pass btn_unit pointer into btn_ok so slot_select_confirm_gauge_cb can access and delete it
    lv_obj_set_user_data(roller, btn_unit);

    lv_obj_add_event_cb(roller, slot_select_confirm_gauge_cb, LV_EVENT_CLICKED, roller);

    // Remove button from group so encoder won't focus it while modal is open
    lv_group_remove_obj(btn_unit);

    // Add to group and AUTO-ENTER editing mode for roller
    lv_group_add_obj(encoder_group, roller);

    lv_group_focus_obj(roller);
    lv_group_set_editing(encoder_group, true); // Enables immediate scrolling without extra click
}

/**
 * Event: User confirms gauge count modal
 */
static void slot_select_confirm_cb(lv_event_t * e) {
    lv_obj_t * roller = (lv_obj_t *)lv_event_get_user_data(e);

    // Read selected count (Index 0=1 gauge, 1=2 gauges, 2=3 gauges)
    uint16_t selected_slots = lv_roller_get_selected(roller) + 1;

    // 1. MUST remove objects from encoder group BEFORE deleting parent container
    lv_group_remove_obj(roller);

    lv_obj_delete(roller);

    // 2. Create a new tile at current configured_screen_count index
    int new_col = configured_screen_count;
    lv_obj_t * new_gauge_tile = lv_tileview_add_tile(tileview, new_col, 0, LV_DIR_LEFT | LV_DIR_RIGHT);

    int button_x_pos;
    int x_pos;

    // 3. Populate requested gauge count inside new tile
    for (int i = 0; i < selected_slots; i++) {
        // const gauge_config_t * cfg = (i % 2 == 0) ? &OIL_PRESS_CONFIG : &WATER_TEMP_CONFIG;
        // gauge_create(new_gauge_tile, i, cfg);
        lv_obj_t * btn_unit = lv_button_create(new_gauge_tile);
        lv_obj_set_size(btn_unit, 120, 60);
        button_x_pos = ((MAX_WIDTH / selected_slots) * i + (MAX_WIDTH / selected_slots / 2));
        lv_obj_set_pos(btn_unit, button_x_pos - 60, 30);
        lv_obj_set_style_radius(btn_unit, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(btn_unit, COLOR_VAG_DARK_GR, 0);
        lv_obj_set_style_outline_color(btn_unit, COLOR_VAG_WHITE, LV_PART_MAIN);
        lv_obj_set_style_outline_width(btn_unit, 2, LV_PART_MAIN);

        lv_obj_t * lbl = lv_label_create(btn_unit);
        lv_label_set_text(lbl, "Select unit...");
        lv_obj_set_style_text_font(lbl, &ui_font_futura_bold, LV_PART_MAIN);
        lv_obj_align_to(lbl, btn_unit, LV_ALIGN_CENTER, 0, 0);

        x_pos = button_x_pos - (SLOT_WIDTH / 2);

        lv_obj_set_user_data(btn_unit, (void *)(uintptr_t)x_pos);

        lv_obj_add_event_cb(btn_unit, slot_select_unit_modal_cb, LV_EVENT_CLICKED, NULL);
    }

    // Enable encoder tile navigation on this new screen
    lv_group_add_obj(encoder_group, new_gauge_tile);
    lv_obj_add_event_cb(new_gauge_tile, tile_key_nav_cb, LV_EVENT_KEY, NULL);

    configured_screen_count++;

    // 4. Reposition Add (+) and Settings tiles to stay on the far right
    lv_obj_set_pos(tile_add, configured_screen_count * 480, 0);
    lv_obj_set_pos(tile_settings, (configured_screen_count + 1) * 480, 0);

    // 5. Jump to the newly created gauge screen
    current_tile_col = new_col;
    lv_tileview_set_tile_by_index(tileview, current_tile_col, 0, LV_ANIM_ON);
    lv_group_focus_obj(new_gauge_tile);
}

/**
 * Event: Click "+" to open gauge selection modal
 */
static void add_btn_modal_cb(lv_event_t * e) {
    lv_obj_t * btn_plus = lv_event_get_target(e);
    lv_obj_t * parent = lv_obj_get_parent(btn_plus);

    // 2. Create Roller
    lv_obj_t * roller = lv_roller_create(parent);
    lv_roller_set_options(roller, "1 Gauge\n2 Gauges\n3 Gauges", LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(roller, 2);
    lv_obj_set_style_bg_color(roller, COLOR_VAG_DARK_GR, LV_PART_MAIN);
    lv_obj_set_style_bg_color(roller, COLOR_VAG_RED, LV_PART_SELECTED);
    lv_obj_set_style_text_font(roller, &ui_font_futura_bold, LV_PART_MAIN);
    lv_obj_align(roller, LV_ALIGN_CENTER, 0, 0);

    // Pass OK button to Roller key callback so Enter jumps focus to OK
    lv_obj_add_event_cb(roller, slot_select_confirm_cb, LV_EVENT_CLICKED, roller);

    lv_group_focus_obj(roller);
    lv_group_set_editing(encoder_group, true); // Enables immediate scrolling without extra click
}

/**
 * Main Dynamic Carousel UI Initialization
 */
void system_ui_init(lv_group_t * g) {
    encoder_group = g;

    tileview = lv_tileview_create(lv_screen_active());
    lv_obj_remove_style_all(tileview);
    lv_obj_set_size(tileview, 480, 128);

    // 1. Tile 0: Add (+) Screen
    tile_add = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_RIGHT);
    create_add_tile_content(tile_add);

    // 2. Tile 1: Settings Screen
    tile_settings = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_LEFT);
    create_settings_tile_content(tile_settings);

    current_tile_col = 0;
    lv_tileview_set_tile_by_index(tileview, 0, 0, LV_ANIM_OFF);
}

static void create_add_tile_content(lv_obj_t * parent) {
    lv_obj_t * btn = lv_button_create(parent);
    lv_obj_set_size(btn, 60, 60);
    lv_obj_center(btn);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_outline_color(btn, COLOR_VAG_WHITE, LV_PART_MAIN);
    lv_obj_set_style_outline_width(btn, 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, COLOR_VAG_DARK_GR, 0);

    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, "+");
    lv_obj_set_style_text_font(lbl, &ui_font_futura_bold, LV_PART_MAIN);
    lv_obj_center(lbl);

    lv_obj_add_event_cb(btn, add_btn_modal_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(parent, tile_key_nav_cb, LV_EVENT_KEY, NULL);

    lv_group_add_obj(encoder_group, btn);
}

static void create_settings_tile_content(lv_obj_t * parent) {
    lv_obj_t * lbl = lv_label_create(parent);
    lv_label_set_text(lbl, "SETTINGS");
    lv_obj_center(lbl);

    lv_group_add_obj(encoder_group, parent);
    lv_obj_add_event_cb(parent, tile_key_nav_cb, LV_EVENT_KEY, NULL);
}
