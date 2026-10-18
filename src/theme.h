#ifndef UTILS_H
#define UTILS_H

#include "lvgl/lvgl.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// OEM Color Palette
#define COLOR_VAG_GR      lv_color_hex(0x474b4e)
#define COLOR_VAG_DARK_GR lv_color_hex(0x43454b)
#define COLOR_VAG_WHITE   lv_color_hex(0xFFFFFF)
#define COLOR_VAG_RED     lv_color_hex(0xE60000)

/**********************
 *      FONT DECLARATIONS
 **********************/
LV_FONT_DECLARE(ui_font_futura_bold);
LV_FONT_DECLARE(ui_font_vt323);

/**********************
 *      IMAGE DECLARATIONS
 **********************/
LV_IMAGE_DECLARE(gauge_bgnd);
LV_IMAGE_DECLARE(oil_pressure);
LV_IMAGE_DECLARE(oil_temp);
LV_IMAGE_DECLARE(water_temp);
LV_IMAGE_DECLARE(battery_voltage);

typedef enum {
  BTN,
  ROLLER,
  LABEL,
  SCALE,
  REDLINE,
  ICON,
  NEEDLE,
  CAP
} theme_elem_t;

typedef struct {
  uint8_t scale_width;
  uint8_t scale_height;
  uint8_t scale_range;
  int scale_offset; //We can allow here the angle to take neg values
  uint8_t scale_x_ofs;
  uint8_t scale_y_ofs;
  const lv_font_t * scale_font;
  uint32_t scale_font_color;

  uint32_t tick_color;
  uint8_t tick_width;
  uint8_t tick_length;

  uint32_t sec_tick_color;
  uint8_t sec_tick_width;
  uint8_t sec_tick_length;

  uint32_t redline_color;
  uint8_t redline_width;
  int redline_padding;

  uint32_t icon_color;

  uint32_t cap_color;
  uint32_t cap_border_color;
  uint8_t cap_radius;

  const lv_font_t * label_font;
  uint32_t label_font_color;    // Active digit/text color
  int label_x_ofs;
  int label_y_ofs;
  lv_align_t label_align;
  bool show_value;


  bool show_analog_dial;       // True for needle/dial, False for digital LCD
  uint32_t needle_color;
  float needle_base_width;
  float needle_length;
} gauge_theme_t;


typedef struct {
  const gauge_theme_t gauge_theme;

  uint32_t focus_ring_color;
  uint8_t focus_ring_width;

  uint32_t obj_shape;
  uint8_t obj_outline;
  uint32_t obj_outline_color;

  uint32_t roller_bgnd;
  uint32_t roller_selection;
  uint32_t roller_outline_color;
  const lv_font_t * roller_font;

  const lv_font_t * button_font;
  uint32_t button_color;
  uint8_t plus_btn_h;
  uint8_t plus_btn_w;
  uint8_t unit_btn_w;

  const lv_font_t * main_font;
  uint32_t font_color;
  bool caps_lock;
  uint32_t main_bgnd;

} screen_theme_t;

void to_upper(char* str);
void apply_screen_theme(lv_obj_t * obj, const screen_theme_t * theme, char* content, const theme_elem_t elem);
void apply_gauge_theme(lv_obj_t * obj, const gauge_theme_t * theme, void * user_data, const theme_elem_t elem);
void apply_redline_theme(lv_style_t * styl, const gauge_theme_t * theme);


#endif
