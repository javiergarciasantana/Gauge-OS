#ifndef SCREEN_H
#define SCREEN_H

#include "gauge.h"

typedef enum {
  EMPTY,
  POPULATED,
  SETTINGS
} screen_type_t;


typedef struct {
  uint32_t roller_bgnd;
  uint32_t roller_selection;
  int roller_outline_width;
  uint32_t roller_outline_color;
  const lv_font_t * roller_font;
  const lv_font_t * button_font;
  uint32_t button_color;
  uint32_t button_outline_color;
  int button_outline;
  int plus_btn_h;
  int plus_btn_w;
  int unit_btn_w;

  const lv_font_t * main_font;
  uint32_t font_color;
  uint32_t main_bgnd;

} screen_theme_t;


typedef struct {
  screen_type_t type;
  const screen_theme_t * theme;
  lv_group_t * encoder_group;
  lv_obj_t * tile; //this contains tileview as parent
  int selected_cfg;

} screen_t;


extern const screen_theme_t ANALOGUE;
extern const screen_theme_t DIGITAL;

screen_t * screen_create(lv_obj_t * parent,
                         screen_type_t type,
                         const screen_theme_t * theme,
                         lv_group_t * encoder_group,
                         int selected_slots,
                         int col_index,
                         void * user_data);

#endif
