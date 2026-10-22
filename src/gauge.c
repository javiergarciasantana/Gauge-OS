#include "gauge.h"
#include <math.h>
#include <stdio.h>

/**********************
 *      GAUGE CONFIG DEFINITIONS
 **********************/
const gauge_config_t OIL_PRESS_CONFIG = {
  .type = PARAM_OIL_PRESS,
  .min_val = 0.0, .max_val = 5.0,
  .mid_val_pos = 4,
  .show_redline = true,
  .red_line_begin = 2.0f, .red_line_end = 8.0f, // 0.0 to 1.0 bar
  .n_ticks = 6, .ticks_period = 1,
  .title_content = "OIL_PRESS",
  .unit = "bar",
  .icon_src = &oil_pressure
};

const gauge_config_t WATER_TEMP_CONFIG = {
    .type = PARAM_WATER_TEMP,
    .min_val = 45.0, .max_val = 135.0,
    .mid_val_pos = 2,
    .show_redline = true,
    .red_line_begin = 1050.0f, .red_line_end = 1296.0f, // 105 to 150ish °C
    .n_ticks = 5, .ticks_period = 1,
    .title_content = "COOLANT_TEMP",
    .unit = "ºC",
    .icon_src = &water_temp
};

const gauge_config_t BATTERY_VOLTAGE_CONFIG = {
    .type = PARAM_BATTERY_VOLTAGE,
    .min_val = 10.0, .max_val = 15.0,
    .mid_val_pos = 3,
    .show_redline = true,
    .red_line_begin = 102.0f, .red_line_end = 122.0f, // 10.2 to 12.2ish °C
    .n_ticks = 6, .ticks_period = 1,
    .title_content = "BATTERY_VOLT",
    .unit = "V",
    .icon_src = &battery_voltage
};

const gauge_config_t OIL_TEMP_CONFIG = {
    .type = PARAM_OIL_TEMP,
    .min_val = 60.0, .max_val = 140.0,
    .mid_val_pos = 2,
    .show_redline = true,
    .red_line_begin = 1150.0f, .red_line_end = 1370.0f, // 115 to 140ish °C
    .n_ticks = 5, .ticks_period = 1,
    .title_content = "OIL_TEMP",
    .unit = "ºC",
    .icon_src = &oil_temp
};

/**********************
 *      STATIC FUNCTIONS
 **********************/

// Static instances storage
static gauge_t * active_gauges[MAX_ACTIVE_GAUGES] = { NULL };
static int active_gauge_count = 0;

/**
 * Procedural Needle Draw Callback
 */
static void needle_draw_cb(lv_event_t * e) {
  lv_layer_t * layer = lv_event_get_layer(e);
  lv_obj_t * obj = lv_event_get_target(e);

  gauge_t * g = (gauge_t *)lv_event_get_user_data(e);

  if (!g || !g->theme) return;

  lv_draw_triangle_dsc_t tri_dsc;
  lv_draw_triangle_dsc_t shadow_dsc;

  apply_needle_theme(&tri_dsc, &shadow_dsc, obj, g->current_needle_angle_rad, g->theme, g->width, g->height);
  if (g->theme->show_analog_dial) {
    lv_draw_triangle(layer, &shadow_dsc);
    lv_draw_triangle(layer, &tri_dsc);
  }
}

/**
 * Register gauge to active polling list
 */
static void register_gauge(gauge_t* g) {

  if(active_gauge_count < MAX_ACTIVE_GAUGES) {
    active_gauges[active_gauge_count++] = g;
  } else {
    //Print error message through event
  }
}

/**********************
 *      NON STATIC FUNCTIONS
 **********************/
gauge_t * gauge_create(lv_obj_t * parent, const int x_pos, const int width, const int height, const gauge_theme_t * theme, const gauge_config_t * cfg) {
  assert(cfg->mid_val_pos < GAUGE_MAX_TICKS && "position of mid_val out of bounds");
  assert(cfg->mid_val_pos >= 0 && "position of mid_val cannot be negative");
  // Allocate heap memory
  gauge_t * g = (gauge_t *)lv_malloc(sizeof(gauge_t));
  // Assert allocation succeeded (halts or logs if memory allocation failed)
  LV_ASSERT_MALLOC(g);
  // Zero out all fields (sets all ints/floats to 0, pointers to NULL, bools to false)
  memset(g, 0, sizeof(gauge_t)); // or lv_memset(g, 0, sizeof(gauge_t));

  g->config = cfg;
  g->theme = theme;
  g->width = width;
  g->height = height;

  // Slot Container
  lv_obj_t * gauge_cell = lv_obj_create(parent);
  lv_obj_remove_style_all(gauge_cell);
  lv_obj_set_size(gauge_cell, g->width, g->height);
  lv_obj_set_pos(gauge_cell, x_pos, 0);
  lv_obj_set_style_pad_all(gauge_cell, 0, LV_PART_MAIN);
  lv_obj_add_flag(gauge_cell, LV_OBJ_FLAG_CLICKABLE);

  g->gauge_cell = gauge_cell;
  apply_gauge_theme(g->gauge_cell, g->theme, NULL, BASE);

  // Background Image Asset
  if (g->theme->bg_src) {
    g->bg_img = lv_image_create(gauge_cell);
    apply_gauge_theme(g->bg_img, g->theme, NULL, BGND);
  }
  // Icon Image Asset
  if (g->config->icon_src) {
    g->icon_img = lv_image_create(gauge_cell);
    apply_gauge_theme(g->icon_img, g->theme, (void*)g->config->icon_src, ICON);
  }
  // Scale Object
  g->scale = lv_scale_create(gauge_cell);
  apply_gauge_theme(g->scale, g->theme, (void*)(uintptr_t)0, SCALE);

  lv_scale_set_range(g->scale, (int32_t)(cfg->min_val * 10), (int32_t)(cfg->max_val * 10));
  lv_scale_set_total_tick_count(g->scale, cfg->n_ticks);
  lv_scale_set_major_tick_every(g->scale, cfg->ticks_period);

  // String formatting into persistent memory
  snprintf(g->start_label, sizeof(g->start_label), "%d", (int)g->config->min_val);
  snprintf(g->mid_label,   sizeof(g->mid_label),   "%d", (int)(((g->config->max_val - g->config->min_val) / (g->config->n_ticks - 1)) * g->config->mid_val_pos + g->config->min_val));
  snprintf(g->end_label,   sizeof(g->end_label),   "%d", (int)g->config->max_val);

  for (int i = 0; i < g->config->n_ticks; i++) {
      g->custom_labels[i] = "";
  }
  g->custom_labels[0] = g->start_label;
  g->custom_labels[g->config->mid_val_pos] = g->mid_label;
  g->custom_labels[g->config->n_ticks] = NULL; // Null terminator

  lv_scale_set_text_src(g->scale, g->custom_labels);

  // Redline Arc
  if (g->config->show_redline) {
      static lv_style_t style_red_arc;
      static bool style_inited = false;
      if (!style_inited) {
          lv_style_init(&style_red_arc);
          apply_redline_theme(&style_red_arc, g->theme);
          style_inited = true;
      }
      lv_scale_section_t * sec = lv_scale_add_section(g->scale);
      lv_scale_section_set_range(sec, cfg->red_line_begin, cfg->red_line_end);
      lv_scale_set_section_style_main(g->scale, sec, &style_red_arc);
  }

  // Needle Widget
  g->needle = lv_obj_create(gauge_cell);
  lv_obj_remove_style_all(g->needle);
  lv_obj_set_size(g->needle, LV_PCT(100), LV_PCT(100));
  lv_obj_remove_flag(g->needle, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(g->needle, needle_draw_cb, LV_EVENT_DRAW_MAIN, g);

  // Center Hub Cap
  g->cap = lv_obj_create(gauge_cell);
  apply_gauge_theme(g->cap, g->theme, g->scale, CAP);

  // Title
  g->title = lv_label_create(gauge_cell);
  apply_gauge_theme(g->title, g->theme, (void*)(uintptr_t)g->theme->title_align, TITLE);
  char title_buf[16] = {0};
  snprintf(title_buf, sizeof(title_buf), "%s", g->config->title_content);
  if (g->theme->caps_lock) to_upper(title_buf);
  lv_label_set_text(g->title, title_buf);

  // Readout Label
  g->label = lv_label_create(gauge_cell);
  apply_gauge_theme(g->label, g->theme, (void*)(uintptr_t)g->theme->label_align, LABEL);
  gauge_update_value(g, cfg->min_val);
  register_gauge(g);

  return g;
}

void gauge_update(gauge_t * g, const gauge_theme_t * theme) {
  if (!g || !theme) return;

  g->theme = theme;

  // FIX: Guard optional widgets against NULL
  if (g->bg_img) {
    apply_gauge_theme(g->bg_img, g->theme, NULL, BGND);
  }
  if (g->icon_img) {
    apply_gauge_theme(g->icon_img, g->theme, (void*)g->config->icon_src, ICON);
  }
  if (g->scale) {
    apply_gauge_theme(g->scale, g->theme, (void*)(uintptr_t)0, SCALE);
  }
  if (g->needle) {
    lv_obj_invalidate(g->needle); // Invalidation triggers needle_draw_cb safely
  }
  if (g->cap) {
    apply_gauge_theme(g->cap, g->theme, g->scale, CAP);
  }
  if (g->label) {
    apply_gauge_theme(g->label, g->theme, (void*)(uintptr_t)g->theme->label_align, LABEL);
  }
  if (g->title) {
    apply_gauge_theme(g->title, g->theme, (void*)(uintptr_t)g->theme->title_align, TITLE);
  }
}

void gauge_update_value(gauge_t * g, float new_val) {
    g->value = new_val;
    if (g->value < g->config->min_val) g->value = g->config->min_val;
    if (g->value > g->config->max_val) g->value = g->config->max_val;

    float range = g->config->max_val - g->config->min_val;
    float norm  = (range > 0.001f) ? ((g->value - g->config->min_val) / range) : 0.0f;
    float angle_deg = 220.0f + (norm * 100.0f);

    g->current_needle_angle_rad = angle_deg * (3.14159265f / 180.0f);

    char unit_buf[16] = {0};
    snprintf(unit_buf, sizeof(unit_buf), "%s", g->config->unit);
    if (g->theme->caps_lock) to_upper(unit_buf);
    if (g->needle) lv_obj_invalidate(g->needle);
    if (g->label)  {
      if (g->theme->show_value) {
        lv_label_set_text_fmt(g->label, "%.1f %s", g->value, unit_buf);
      } else {
        lv_label_set_text(g->label, unit_buf);
      }
    }
}

void gauge_update_value_by_type(param_type_t type, float new_val) {
    for (int i = 0; i < active_gauge_count; ++i) {
      if (active_gauges[i]->config->type == type) {
        gauge_update_value(active_gauges[i], new_val);
      }
    }
}

void gauge_destroy(gauge_t * g) {
  if (!g) return;
}

void gauge_print(const gauge_t *g) {
    if (!g) {
        printf("null\n");
        return;
    }

    printf("{\n");

    // 1. Dimensions & LVGL Widget Handles
    printf("  \"width\": %d,\n", g->width);
    printf("  \"height\": %d,\n", g->height);
    printf("  \"scale\": %s,\n", g->scale ? "\"0x" : "null");
    if (g->scale) printf("%p\"", (void *)g->scale);
    printf("  \"needle\": %s,\n", g->needle ? "\"0x" : "null");
    if (g->needle) printf("%p\"", (void *)g->needle);
    printf("  \"label\": %s,\n", g->label ? "\"0x" : "null");
    if (g->label) printf("%p\"", (void *)g->label);

    // 2. Dynamic Runtime Values
    printf("  \"value\": %.2f,\n", g->value);
    printf("  \"current_needle_angle_rad\": %.4f,\n", g->current_needle_angle_rad);

    // 3. Persistent Label Arrays
    printf("  \"start_label\": \"%s\",\n", g->start_label);
    printf("  \"mid_label\": \"%s\",\n", g->mid_label);
    printf("  \"end_label\": \"%s\",\n", g->end_label);

    // 4. Custom Labels JSON Array
    printf("  \"custom_labels\": [");
    for (int i = 0; i < GAUGE_MAX_TICKS; i++) {
        if (g->custom_labels[i]) {
            printf("%s\"%s\"", (i > 0) ? ", " : "", g->custom_labels[i]);
        } else {
            printf("%snull", (i > 0) ? ", " : "");
        }
    }
    printf("],\n");

    // 5. Theme Address Handle
    printf("  \"theme\": \"%p\",\n", (const void *)&g->theme);

    // 6. Embedded gauge_config_t Struct
    printf("  \"config\": {\n");
    printf("    \"type\": %d,\n", (int)g->config->type);
    printf("    \"min_val\": %.2f,\n", g->config->min_val);
    printf("    \"max_val\": %.2f,\n", g->config->max_val);
    printf("    \"mid_val_pos\": %d,\n", g->config->mid_val_pos);
    printf("    \"ticks_period\": %u,\n", (unsigned int)g->config->ticks_period);
    printf("    \"show_redline\": %s,\n", g->config->show_redline ? "true" : "false");
    printf("    \"red_line_begin\": %.2f,\n", g->config->red_line_begin);
    printf("    \"red_line_end\": %.2f,\n", g->config->red_line_end);
    printf("    \"n_ticks\": %d,\n", g->config->n_ticks);
    printf("    \"unit\": %s%s%s,\n",
           g->config->unit ? "\"" : "",
           g->config->unit ? g->config->unit : "null",
           g->config->unit ? "\"" : "");
    printf("    \"bg_src\": %s,\n", g->theme->bg_src ? "\"0x" : "null");
    if (g->theme->bg_src) printf("%p\"", (const void *)g->theme->bg_src);
    printf("    \"icon_src\": %s\n", g->config->icon_src ? "\"0x" : "null");
    if (g->config->icon_src) printf("%p\"", (const void *)g->config->icon_src);
    printf("  }\n");

    printf("}\n");
}
