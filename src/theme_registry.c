#include "theme_registry.h"

/********************************
 *      GAUGE THEME DEFINITIONS
 ********************************/
const gauge_theme_t GAUGE_VW_POLO_6N2 = {
  .icon_color = 0xFFFFFF, .icon_y_ofs = 0,

  .scale_width = 175, .scale_height = 175, .scale_range = 100,
  .scale_offset = 220, .scale_x_ofs = 0, .scale_y_ofs = 50,
  .scale_font = &ui_font_futura_bold, .scale_font_color = 0xFFFFFF,

  .tick_color = 0xFFFFFF, .tick_width = 4, .tick_length = 14,
  .sec_tick_color = 0x474b4e, .sec_tick_width = 0, .sec_tick_length = 0,

  .redline_color = 0xE60000, .redline_width = 15, .redline_padding = -1,

  .cap_color = 0x474b4e, .cap_border_color = 0x474b4e, .cap_radius = 55,

  .title_font = &ui_font_futura_bold, .title_font_color = 0x474b4e,
  .title_x_ofs = 0, .title_y_ofs = -150, .title_align = LV_ALIGN_CENTER,

  .label_font = &ui_font_futura_bold, .label_font_color = 0xFFFFFF,
  .label_x_ofs = 60, .label_y_ofs = -20, .label_align = LV_ALIGN_BOTTOM_MID,
  .show_value = false,

  .show_analog_dial = true, .needle_color = 0xE60000,
  .needle_base_width = 10.0f, .needle_length = 88.0f,

  .focus_color = 0x9ca2a5, .focus_shape = 20,

  .bg_src = &gauge_bgnd,

  .caps_lock = false
};

const gauge_theme_t GAUGE_VW_DIGITAL = {
  .icon_color = 0x274250, .icon_y_ofs = -60,

  .cap_color = 0x8fa37b, .cap_border_color = 0x8fa37b, .cap_radius = 0,

  .scale_width = 0, .scale_height = 0, .scale_range = 0,
  .scale_offset = 0, .scale_x_ofs = 0, .scale_y_ofs = 0,
  .scale_font = &ui_font_futura_bold, .scale_font_color = 0x8fa37b,

  .tick_color = 0x8fa37b, .tick_width = 0, .tick_length = 0,
  .sec_tick_color = 0x8fa37b, .sec_tick_width = 0, .sec_tick_length = 0,

  .redline_color = 0x8fa37b, .redline_width = 0, .redline_padding = 0,

  .title_font = &ui_font_vt323, .title_font_color = 0x274250,
  .title_x_ofs = 0, .title_y_ofs = -15, .title_align = LV_ALIGN_CENTER,

  .label_font = &ui_font_vt323, .label_font_color = 0x274250,
  .label_x_ofs = 0, .label_y_ofs = 15, .label_align = LV_ALIGN_CENTER,
  .show_value = true,

  .show_analog_dial = false, .needle_color = 0x8fa37b,
  .needle_base_width = 0.0f, .needle_length = 0.0f,

  .focus_color = 0x517163, .focus_shape = 0,

  .bg_src = NULL,

  .caps_lock = true
};

/********************************
 *      SCREEN THEME DEFINITIONS
 ********************************/
const screen_theme_t VW_POLO_6N2 = {
  .obj_shape = 20, .obj_outline = 2,
  .obj_outline_color = 0xFFFFFF,
  .focus_ring_color = 0xFFFFFF, .focus_ring_width = 4,
  .roller_bgnd = 0x43454b, .roller_font = &ui_font_futura_bold,
  .roller_outline_color = 0xFFFFFF,
  .roller_selection = 0xE60000,
  .button_color = 0x43454b, .button_font = &ui_font_futura_bold,
  .plus_btn_h = 60, .plus_btn_w = 60, .unit_btn_w = 120,

  .main_font = &ui_font_futura_bold, .font_color = 0xFFFFFF,
  .caps_lock = false,
  .main_bgnd = 0x474b4e,
  .gauge_theme = &GAUGE_VW_POLO_6N2
};

const screen_theme_t VW_DIGITAL = {
  .obj_shape = 0, .obj_outline = 0,
  .obj_outline_color = 0x8fa37b,
  .focus_ring_color = 0x274250, .focus_ring_width = 4,
  .roller_bgnd = 0x8fa37b, .roller_font = &ui_font_vt323,
  .roller_outline_color = 0x8fa37b,
  .roller_selection = 0x274250,
  .button_color = 0x8fa37b, .button_font = &ui_font_vt323,
  .plus_btn_h = 60, .plus_btn_w = 60, .unit_btn_w = 120,

  .main_font = &ui_font_vt323, .font_color = 0x274250,
  .caps_lock = true,
  .main_bgnd = 0x8fa37b,
  .gauge_theme = &GAUGE_VW_DIGITAL
};

/**
 * Theme lookup table
 */
static const screen_theme_t * THEME_MAP[THEME_ID_COUNT] = {
    [THEME_ID_VW_POLO_6N2] = &VW_POLO_6N2,
    [THEME_ID_VW_DIGITAL]  = &VW_DIGITAL,
};

const screen_theme_t * theme_get_by_id(theme_id_t id) {
    if (id >= THEME_ID_COUNT) return &VW_POLO_6N2;
    return THEME_MAP[id];
}

const char * theme_get_roller_options(bool caps_lock) {
  char * content = "Analogue\nDigital";
  if (caps_lock) to_upper(content);
  return content;
}
