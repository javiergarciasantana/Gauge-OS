#include "screen.h"
#include "string.h"

extern void slot_select_confirm_cb(lv_event_t * e);

// Global configurations (NO 'static' keyword)
const screen_theme_t ANALOGUE = {
  .roller_bgnd = 0x43454b, .roller_font = &ui_font_futura_bold,
  .roller_outline_width = 2, .roller_outline_color = 0xFFFFFF,
  .roller_selection = 0xE60000,
  .button_color = 0x43454b, .button_font = &ui_font_futura_bold,
  .button_outline_color = 0xFFFFFF, .button_outline = 2,
  .plus_btn_h = 60, .plus_btn_w = 60, .unit_btn_w = 120,

  .main_font = &ui_font_futura_bold, .font_color = 0xFFFFFF,
  .main_bgnd = 0x474b4e
};

const screen_theme_t DIGITAL = {
  .roller_bgnd = 0x8fa37b, .roller_font = &ui_font_futura_bold,
  .roller_outline_width = 0, .roller_outline_color = 0x8fa37b,
  .roller_selection = 0x274250,
  .button_color = 0x8fa37b, .button_font = &ui_font_futura_bold,
  .button_outline_color = 0x8fa37b, .button_outline = 0,
  .plus_btn_h = 60, .plus_btn_w = 60, .unit_btn_w = 120,

  .main_font = &ui_font_futura_bold, .font_color = 0x274250,
  .main_bgnd = 0x8fa37b
};

static void apply_theme(lv_obj_t * obj, const screen_theme_t * theme, char* content, char* type) {
  if (strcmp(type, "roller") == 0) {
    lv_roller_set_options(obj, content, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(obj, 2);
    lv_obj_set_style_border_width(obj, theme->roller_outline_width, LV_PART_MAIN);
    lv_obj_set_style_outline_width(obj, theme->roller_outline_width, LV_PART_MAIN);
    lv_obj_set_style_border_color(obj, lv_color_hex(theme->roller_outline_color), LV_PART_MAIN);
    lv_obj_set_style_outline_color(obj, lv_color_hex(theme->roller_outline_color), LV_PART_MAIN);
    //Focus Ring Thickness
    lv_obj_set_style_outline_width(obj, theme->roller_outline_width, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(obj, theme->roller_outline_width, LV_PART_MAIN | LV_STATE_EDITED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(theme->roller_bgnd), LV_PART_MAIN);
    lv_obj_set_style_bg_color(obj, lv_color_hex(theme->roller_selection), LV_PART_SELECTED);
    lv_obj_set_style_text_font(obj, theme->roller_font, LV_PART_MAIN);

  } else if (strcmp(type, "btn") == 0) {
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(theme->button_color), 0);
    lv_obj_set_style_outline_color(obj, lv_color_hex(theme->button_outline_color), LV_PART_MAIN);
    lv_obj_set_style_outline_width(obj, theme->button_outline, LV_PART_MAIN);

    lv_obj_t * lbl = lv_label_create(obj);
    lv_label_set_text(lbl, content);
    lv_obj_set_style_text_font(lbl, theme->main_font, LV_PART_MAIN);
    lv_obj_set_style_text_color(lbl, lv_color_hex(theme->font_color), LV_PART_MAIN);
    lv_obj_center(lbl);
  }
}

static void slot_select_confirm_gauge_cb(lv_event_t * e) {

  screen_t * s = (screen_t *)lv_event_get_user_data(e);
  lv_obj_t * roller = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t * btn_unit = (lv_obj_t *)(uintptr_t)lv_obj_get_user_data(roller);

  // Read selected count (Index 0=1 gauge, 1=2 gauges, 2=3 gauges)
  uint16_t selected_cfg = lv_roller_get_selected(roller);
  int x_pos = (int)(uintptr_t)lv_obj_get_user_data(btn_unit);
  //printf("x position for gauge: %d\n", x_pos);
  lv_obj_t * tile_parent = s->tile;

  // MUST remove objects from encoder group BEFORE deleting parent container
  lv_group_remove_obj(roller);
  lv_obj_delete(roller);

  const gauge_config_t * cfg = NULL;

  switch (selected_cfg) {
    case 0: cfg = &OIL_PRESS_CONFIG; break;
    case 1: cfg = &OIL_TEMP_CONFIG; break;
    case 2: cfg = &WATER_TEMP_CONFIG; break;
    case 3: cfg = &BATTERY_VOLTAGE_CONFIG; break;
    default: return;
  }

  lv_obj_delete(btn_unit);
  gauge_create(tile_parent, x_pos, cfg);
}

static void slot_select_unit_modal_cb(lv_event_t * e) {

  lv_obj_t * btn_unit = lv_event_get_target(e);
  screen_t * s = (screen_t *)lv_event_get_user_data(e);
  lv_obj_t * parent = s->tile;

  // Create Roller
  lv_obj_t * roller = lv_roller_create(parent);
  apply_theme(roller, s->theme, "Oil Pressure\nOil Temp\nCoolant temp\nBattery volt\nBack", "roller");
  lv_obj_align_to(roller, btn_unit, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_user_data(roller, btn_unit);

  lv_obj_add_event_cb(roller, slot_select_confirm_gauge_cb, LV_EVENT_CLICKED, s);

  // Remove button from group so encoder won't focus it while modal is open
  lv_group_remove_obj(btn_unit);

  // Add to group and AUTO-ENTER editing mode for roller
  lv_group_add_obj(s->encoder_group, roller);

  lv_group_focus_obj(roller);
  lv_group_set_editing(s->encoder_group, true); // Enables immediate scrolling without extra click
}

/**
 * Event: Click "+" to open gauge selection modal
 */
static void add_btn_modal_cb(lv_event_t * e) {
  // 1. Retrieve the opaque void * pointer without casting it
  void * sys_ptr = lv_event_get_user_data(e);
  screen_t * s = (screen_t *)lv_obj_get_user_data(lv_event_get_target(e));

  lv_obj_t * roller = lv_roller_create(s->tile);
  apply_theme(roller, s->theme, "1 Gauge\n2 Gauges\n3 Gauges\nBack", "roller");
  lv_obj_align(roller, LV_ALIGN_CENTER, 0, 0);

  // 2. Forward sys_ptr directly to slot_select_confirm_cb in dashboard.c
  lv_obj_add_event_cb(roller, slot_select_confirm_cb, LV_EVENT_CLICKED, sys_ptr);

  lv_group_add_obj(s->encoder_group, roller);
  lv_group_focus_obj(roller);
  lv_group_set_editing(s->encoder_group, true);
}

screen_t * screen_create(lv_obj_t * parent,
                         screen_type_t type,
                         const screen_theme_t * theme,
                         lv_group_t * encoder_group,
                         int selected_slots,
                         int col_index,
                         void * user_data) {

  screen_t * s = (screen_t *)lv_malloc(sizeof(screen_t));
  LV_ASSERT_MALLOC(s);

  s->type = type;
  s->theme = theme;
  s->encoder_group = encoder_group;
  lv_obj_t * tile = NULL;
  lv_obj_t * lbl = NULL;

  switch ((int)s->type) {
    case SETTINGS:
      tile = lv_tileview_add_tile(parent, 1, 0, LV_DIR_LEFT);
      lv_obj_set_user_data(tile, (void *)(uintptr_t)SETTINGS);
      lbl = lv_label_create(tile);
      lv_label_set_text(lbl, "SETTINGS");
      lv_obj_set_style_text_font(lbl, s->theme->main_font, LV_PART_MAIN);
      lv_obj_set_style_text_color(lbl, lv_color_hex(s->theme->font_color), LV_PART_MAIN);
      lv_obj_center(lbl);
      //Pending implementation...
      lv_group_add_obj(encoder_group, tile);
      break;

    case EMPTY:
      tile = lv_tileview_add_tile(parent, 0, 0, LV_DIR_RIGHT);
      lv_obj_set_user_data(tile, (void *)(uintptr_t)EMPTY);
      lv_obj_t * btn = lv_button_create(tile);
      lv_obj_set_size(btn, s->theme->plus_btn_w, s->theme->plus_btn_h);
      lv_obj_center(btn);
      apply_theme(btn, s->theme, "+", "btn");

      // Save screen context onto the button so add_btn_modal_cb can find s->tile
      lv_obj_set_user_data(btn, s);

      //Calling next function with pointer as last param :)
      lv_obj_add_event_cb(btn, add_btn_modal_cb, LV_EVENT_CLICKED, user_data);
      lv_group_add_obj(encoder_group, btn);
      break;

    case POPULATED:

      tile = lv_tileview_add_tile(parent, col_index, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
      lv_obj_set_user_data(tile, (void *)(uintptr_t)POPULATED);

      int button_x_pos;

      // Populate requested gauge count inside new tile
      for (int i = 0; i < selected_slots; i++) {
        lv_obj_t * btn_unit = lv_button_create(tile);
        lv_obj_set_size(btn_unit, s->theme->unit_btn_w, s->theme->plus_btn_h);
        button_x_pos = ((MAX_WIDTH / selected_slots) * i + (MAX_WIDTH / selected_slots / 2));
        lv_obj_set_pos(btn_unit, button_x_pos - s->theme->unit_btn_w / 2, s->theme->plus_btn_h / 2);
        apply_theme(btn_unit, s->theme, "Select unit...", "btn");

        lv_obj_set_user_data(btn_unit, (void *)(uintptr_t)(button_x_pos - (SLOT_WIDTH / 2)));

        lv_obj_add_event_cb(btn_unit, slot_select_unit_modal_cb, LV_EVENT_CLICKED, s);
      }

      lv_group_add_obj(encoder_group, tile);
  }
  s->tile = tile;
  return s;
}
