#include "theme.h"

void to_upper(char *str) {
  if (!str) return;

  int i = 0;
  while (str[i] != '\0') {
    str[i] = (char)toupper((unsigned char)str[i]);
    i++;
  }
}

theme_elem_t get_widget_type(const lv_obj_t * obj) {
    if (!obj) return UNKOWN;

    if (lv_obj_has_class(obj, &lv_button_class)) return BTN;
    if (lv_obj_has_class(obj, &lv_roller_class)) return ROLLER;
    if (lv_obj_has_class(obj, &lv_label_class))  return LABEL;
    if (lv_obj_has_class(obj, &lv_scale_class))  return SCALE;
    if (lv_obj_has_class(obj, &lv_image_class))  return ICON;
    if (lv_obj_get_class(obj) == &lv_obj_class)  return CELL;

    return UNKOWN;
}

void apply_screen_theme(lv_obj_t * obj, const screen_theme_t * theme, char* content, const theme_elem_t elem) {
  if (!obj || !theme) return;
  switch (elem) {
    case ROLLER:
      if (content != NULL) {
        lv_roller_set_options(obj, content, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_visible_row_count(obj, 2);
      }
      // Circle or Rectangle
      lv_obj_set_style_radius(obj, theme->obj_shape, 0);
      lv_obj_set_style_border_width(obj, theme->obj_outline, LV_PART_MAIN);
      lv_obj_set_style_outline_width(obj, theme->obj_outline, LV_PART_MAIN);
      lv_obj_set_style_border_color(obj, lv_color_hex(theme->roller_outline_color), LV_PART_MAIN);
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->roller_outline_color), LV_PART_MAIN);
      // Focus Ring Color
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->focus_ring_color), LV_STATE_FOCUS_KEY);
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->focus_ring_color), LV_STATE_EDITED);
      // Focus Ring Width
      lv_obj_set_style_outline_width(obj, theme->focus_ring_width, LV_STATE_FOCUS_KEY);
      lv_obj_set_style_outline_width(obj, theme->focus_ring_width, LV_STATE_EDITED);
      // Roller intrinsic theme
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
      lv_obj_t * lbl = NULL;
      if (content != NULL) {
        lbl = lv_label_create(obj);
        lv_label_set_text(lbl, content);
      } else {
        lbl = lv_obj_get_child(obj, 0);
      }
      if (lbl) {
        lv_obj_set_style_text_font(lbl, theme->main_font, LV_PART_MAIN);
        lv_obj_set_style_text_color(lbl, lv_color_hex(theme->font_color), LV_PART_MAIN);
        lv_obj_center(lbl);
      }
      break;

      case LABEL:
        if (content != NULL) {
          lv_label_set_text(obj, content);
        }
        lv_obj_set_style_text_font(obj, theme->main_font, LV_PART_MAIN);
        lv_obj_set_style_text_color(obj, lv_color_hex(theme->font_color), LV_PART_MAIN);
        lv_obj_center(obj);
    default: return;
  }
}

void apply_gauge_theme(lv_obj_t * obj, const gauge_theme_t * theme, void * user_data, const theme_elem_t elem) {
  if (!obj || !theme) return;
  switch (elem) {
    case CELL:
      //Focus Ring Color
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->focus_color), LV_STATE_FOCUS_KEY);
      lv_obj_set_style_outline_color(obj, lv_color_hex(theme->focus_color), LV_STATE_EDITED);
      // Focus Ring Shape
      lv_obj_set_style_radius(obj, theme->focus_shape, LV_STATE_FOCUS_KEY);
      lv_obj_set_style_radius(obj, theme->focus_shape, LV_STATE_EDITED);
      //Focus Ring Width
      lv_obj_set_style_outline_width(obj, 4, LV_STATE_FOCUS_KEY);
      lv_obj_set_style_outline_width(obj, 4, LV_STATE_EDITED);
      lv_obj_set_style_outline_pad(obj, -10, LV_STATE_FOCUS_KEY);
      lv_obj_set_style_outline_pad(obj, -10, LV_STATE_EDITED);
      break;
    case SCALE:
      lv_obj_set_size(obj, theme->scale_width, theme->scale_height);
      if((int)(uintptr_t)user_data != 0) break;
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

    case CAP: { //Apparenty C99/C11 requires a {} block for var declaration after case keyword, interesting
      lv_obj_t * scale = (lv_obj_t *)user_data;
      lv_obj_set_size(obj, theme->cap_radius, theme->cap_radius);
      if(scale == NULL) break;
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
    case TITLE: {
      int align = (int)(uintptr_t)user_data;
      lv_obj_align(obj, align, theme->title_x_ofs, theme->title_y_ofs);
      lv_obj_set_style_text_color(obj, lv_color_hex(theme->title_font_color), LV_PART_MAIN);
      lv_obj_set_style_text_font(obj, theme->title_font, LV_PART_MAIN);
      break;
    }
    case BGND: {
      if (theme->bg_src) {
        // 1. Always set source and center image
        lv_image_set_src(obj, theme->bg_src);
        lv_obj_center(obj);
      } else {
          // If theme has no background image, clear source
        lv_image_set_src(obj, NULL);
      }
      break;
    }
    case ICON: {
      lv_image_dsc_t * img_src = (lv_image_dsc_t *)user_data;
      lv_image_set_src(obj, img_src);
      lv_obj_center(obj);
      lv_obj_set_style_image_recolor(obj, lv_color_hex(theme->icon_color), LV_PART_MAIN);
      // Set opacity to COVER (255) to fully overwrite original pixel colors while preserving alpha
      lv_obj_set_style_image_recolor_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
      lv_obj_align(obj, LV_ALIGN_CENTER, 0, theme->icon_y_ofs);
      break;
    }
    default: return;
  }
}

void apply_needle_theme(lv_draw_triangle_dsc_t * tri, lv_draw_triangle_dsc_t * shadow, lv_obj_t * obj, float angle, const gauge_theme_t * theme, int width, int height) {
  if (!obj || !tri || !shadow || !theme) return;

  lv_area_t coords;
  lv_obj_get_coords(obj, &coords);

  float center_x = coords.x1 + (width / 2.0f);
  float center_y = coords.y1 + (height / 2.0f) + theme->scale_y_ofs;
  float perp_angle = angle + 1.57079632679f;

  if(theme->show_analog_dial) {

    lv_draw_triangle_dsc_init(shadow);
    shadow->color = lv_color_hex(0x000000);
    shadow->opa = LV_OPA_40;

    lv_draw_triangle_dsc_init(tri);
    tri->color = lv_color_hex(theme->needle_color);
    tri->opa = LV_OPA_COVER;
  } else {
    lv_draw_triangle_dsc_init(shadow);
    shadow->opa = LV_OPA_0;

    lv_draw_triangle_dsc_init(tri);
    tri->opa = LV_OPA_0;
  }

  shadow->p[0].x = (int32_t)roundf(center_x + 2.0f + theme->needle_length * cosf(angle));
  shadow->p[0].y = (int32_t)roundf(center_y + 2.0f + theme->needle_length * sinf(angle));
  shadow->p[1].x = (int32_t)roundf(center_x + 2.0f + (theme->needle_base_width / 2.0f) * cosf(perp_angle));
  shadow->p[1].y = (int32_t)roundf(center_y + 2.0f + (theme->needle_base_width / 2.0f) * sinf(perp_angle));
  shadow->p[2].x = (int32_t)roundf(center_x + 2.0f - (theme->needle_base_width / 2.0f) * cosf(perp_angle));
  shadow->p[2].y = (int32_t)roundf(center_y + 2.0f - (theme->needle_base_width / 2.0f) * sinf(perp_angle));

  tri->p[0].x = (int32_t)roundf(center_x + theme->needle_length * cosf(angle));
  tri->p[0].y = (int32_t)roundf(center_y + theme->needle_length * sinf(angle));
  tri->p[1].x = (int32_t)roundf(center_x + (theme->needle_base_width / 2.0f) * cosf(perp_angle));
  tri->p[1].y = (int32_t)roundf(center_y + (theme->needle_base_width / 2.0f) * sinf(perp_angle));
  tri->p[2].x = (int32_t)roundf(center_x - (theme->needle_base_width / 2.0f) * cosf(perp_angle));
  tri->p[2].y = (int32_t)roundf(center_y - (theme->needle_base_width / 2.0f) * sinf(perp_angle));
}

void apply_redline_theme(lv_style_t * styl, const gauge_theme_t * theme) {
  lv_style_set_arc_color(styl, lv_color_hex(theme->redline_color));
  lv_style_set_arc_width(styl, theme->redline_width);
  lv_style_set_pad_radial(styl, theme->redline_padding);
  lv_style_set_arc_opa(styl, LV_OPA_COVER);

}
