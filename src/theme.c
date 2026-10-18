
#include "theme.h"


/********************************
 *      GAUGE THEME DEFINITIONS
 ********************************/
const gauge_theme_t GAUGE_ANALOGUE = {
  .scale_width = 175, .scale_height = 175, .scale_range = 100,
  .scale_offset = 220, .scale_x_ofs = 0, .scale_y_ofs = 50,
  .scale_font = &ui_font_futura_bold, .scale_font_color = 0xFFFFFF,

  .tick_color = 0xFFFFFF, .tick_width = 4, .tick_length = 14,
  .sec_tick_color = 0x474b4e, .sec_tick_width = 0, .sec_tick_length = 0,

  .redline_color = 0xE60000, .redline_width = 15, .redline_padding = -1,

  .cap_color = 0x474b4e, .cap_border_color = 0x474b4e, .cap_radius = 55,

  .label_font = &ui_font_futura_bold, .label_font_color = 0xFFFFFF,
  .label_x_ofs = 60, .label_y_ofs = -20, .label_align = LV_ALIGN_BOTTOM_MID,
  .show_value = false,

  .show_analog_dial = true, .needle_color = 0xE60000,
  .needle_base_width = 10.0f, .needle_length = 88.0f
};

const gauge_theme_t GAUGE_DIGITAL = {
  .icon_color = 0x274250,

  .label_font = &ui_font_vt323, .label_font_color = 0x274250,
  .label_x_ofs = 0, .label_y_ofs = 0, .label_align = LV_ALIGN_BOTTOM_MID,
  .show_value = true,

  .show_analog_dial = false,
};

/********************************
 *      SCREEN THEME DEFINITIONS
 ********************************/
const screen_theme_t ANALOGUE = {
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
  .gauge_theme = GAUGE_ANALOGUE
};

const screen_theme_t DIGITAL = {
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
  .gauge_theme = GAUGE_DIGITAL
};

void to_upper(char *str) {
  if (!str) return;

  int i = 0;
  while (str[i] != '\0') {
    str[i] = (char)toupper((unsigned char)str[i]);
    i++;
  }
}

void apply_screen_theme(lv_obj_t * obj, const screen_theme_t * theme, char* content, const theme_elem_t elem) {
  if (!obj || !theme) return;
  switch (elem) {
    case ROLLER:
      lv_roller_set_options(obj, content, LV_ROLLER_MODE_INFINITE);
      lv_roller_set_visible_row_count(obj, 2);
      //Circle or Rectangle
      lv_obj_set_style_radius(obj, theme->obj_shape, 0);
      lv_obj_set_style_border_width(obj, theme->obj_outline, LV_PART_MAIN);
      lv_obj_set_style_outline_width(obj, theme->obj_outline, LV_PART_MAIN);
      lv_obj_set_style_border_color(obj, lv_color_hex(theme->roller_outline_color), LV_PART_MAIN);
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->roller_outline_color), LV_PART_MAIN);
      //Focus Ring Color
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->focus_ring_color), LV_STATE_FOCUS_KEY);
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->focus_ring_color), LV_STATE_EDITED);
      //Focus Ring Width
      lv_obj_set_style_outline_width(obj, theme->focus_ring_width, LV_STATE_FOCUS_KEY);
      lv_obj_set_style_outline_width(obj, theme->focus_ring_width, LV_STATE_EDITED);

      lv_obj_set_style_bg_color(obj, lv_color_hex(theme->roller_bgnd), LV_PART_MAIN);
      lv_obj_set_style_bg_color(obj, lv_color_hex(theme->roller_selection), LV_PART_SELECTED);
      lv_obj_set_style_text_font(obj, theme->roller_font, LV_PART_MAIN);
      lv_obj_set_style_text_color(obj, lv_color_hex(theme->roller_outline_color), LV_PART_SELECTED);
      break;

    case BTN:
      lv_obj_set_style_radius(obj, theme->obj_shape, 0);
      lv_obj_set_style_bg_color(obj, lv_color_hex(theme->button_color), 0);
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->obj_outline_color), LV_PART_MAIN);
      lv_obj_set_style_outline_width(obj, theme->obj_outline, LV_PART_MAIN);
      //Focus Ring Color
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->focus_ring_color), LV_STATE_FOCUS_KEY);
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->focus_ring_color), LV_STATE_EDITED);
      //Focus Ring Width
      lv_obj_set_style_outline_width(obj, theme->focus_ring_width, LV_STATE_FOCUS_KEY);
      lv_obj_set_style_outline_width(obj, theme->focus_ring_width, LV_STATE_EDITED);

      lv_obj_t * lbl = lv_label_create(obj);
      lv_label_set_text(lbl, content);
      lv_obj_set_style_text_font(lbl, theme->main_font, LV_PART_MAIN);
      lv_obj_set_style_text_color(lbl, lv_color_hex(theme->font_color), LV_PART_MAIN);
      lv_obj_center(lbl);
      break;

  }
}

void apply_gauge_theme(lv_obj_t * obj, const gauge_theme_t * theme, void * user_data, const theme_elem_t elem) {
  if (!obj || !theme) return;
  switch (elem) {
    case SCALE:
      lv_obj_set_size(obj, theme->scale_width, theme->scale_height);
      lv_obj_align(obj, LV_ALIGN_CENTER, theme->scale_x_ofs, theme->scale_y_ofs);
      lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
      lv_scale_set_angle_range(obj, theme->scale_range);
      lv_scale_set_rotation(obj, theme->scale_offset);
      lv_obj_set_style_text_font(obj, theme->scale_font, LV_PART_MAIN);
      lv_obj_set_style_text_color(obj, lv_color_hex(theme->scale_font_color), LV_PART_MAIN);
      // Remove Scale Line & Pad Styles(pending)
      lv_obj_set_style_arc_width(obj, 0, LV_PART_MAIN);
      lv_obj_set_style_line_width(obj, 0, LV_PART_MAIN);
      lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
      // Ticks Styling
      lv_obj_set_style_line_color(obj, lv_color_hex(theme->tick_color), LV_PART_INDICATOR);
      lv_obj_set_style_line_width(obj, theme->tick_width, LV_PART_INDICATOR);
      lv_obj_set_style_length(obj, theme->tick_length, LV_PART_INDICATOR);
      // Secondary Ticks Styling
      lv_obj_set_style_line_color(obj, lv_color_hex(theme->tick_color), LV_PART_ITEMS);
      lv_obj_set_style_line_width(obj, theme->sec_tick_width, LV_PART_ITEMS);
      lv_obj_set_style_length(obj, theme->sec_tick_length, LV_PART_ITEMS);
      break;
    case NEEDLE:
      break;


    case CAP: { //Apparenty C99/C11 requires a {} block for var declaration after case keyword, interesting
      lv_obj_t * scale = (lv_obj_t *)user_data;
      lv_obj_set_size(obj, theme->cap_radius, theme->cap_radius);
      lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
      lv_obj_set_style_bg_color(obj, lv_color_hex(theme->cap_color), 0);
      lv_obj_set_style_border_color(obj, lv_color_hex(theme->cap_border_color), 0);
      lv_obj_align_to(obj, scale, LV_ALIGN_CENTER, 0, 0);
      break;
    }
    case LABEL: {
      int align = (int)(uintptr_t)user_data;
      lv_obj_align(obj, align, theme->label_x_ofs, theme->label_y_ofs);
      lv_obj_set_style_text_color(obj, lv_color_hex(theme->label_font_color), LV_PART_MAIN);
      lv_obj_set_style_text_font(obj, theme->label_font, LV_PART_MAIN);
      break;
    }
    default: break;
  }
}

void apply_redline_theme(lv_style_t * styl, const gauge_theme_t * theme) {
  lv_style_set_arc_color(styl, lv_color_hex(theme->redline_color));
  lv_style_set_arc_width(styl, theme->redline_width);
  lv_style_set_pad_radial(styl, theme->redline_padding);
  lv_style_set_arc_opa(styl, LV_OPA_COVER);
}
