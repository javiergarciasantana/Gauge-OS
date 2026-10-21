#ifndef SCREEN_H
#define SCREEN_H

#include "gauge.h"

#define MAX_GAUGES 3

typedef enum {
  EMPTY,
  POPULATED,
  SETTINGS
} screen_type_t;

typedef struct {
  //Maye here declare also dashboard parent to omit having data duplicates? maybe use extern?
  const screen_theme_t * theme;
  screen_type_t type;
  lv_group_t * encoder_group;
  lv_obj_t * tile; //this contains tileview as parent
  uint8_t max_gauges;
  int width;
  int height;

  uint8_t gauge_count;
  gauge_t * gauges[MAX_GAUGES];

} screen_t;


extern const screen_theme_t ANALOGUE;
extern const screen_theme_t DIGITAL;

void screen_print(const screen_t * scr);
void screen_update(screen_t * scr, const screen_theme_t* theme);
screen_t * screen_create(lv_obj_t * parent,
                         screen_type_t type,
                         const screen_theme_t * theme,
                         lv_group_t * encoder_group,
                         int selected_slots,
                         int col_index,
                         int width,
                         int height,
                         uint8_t max_gauges,
                         void* user_data);

#endif
