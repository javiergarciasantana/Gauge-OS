#include "gauge.h"
#include <math.h>
#include <stdio.h>

LV_IMAGE_DECLARE(gauge_bgnd);
LV_IMAGE_DECLARE(oil_pressure);
LV_IMAGE_DECLARE(oil_temp);
LV_IMAGE_DECLARE(water_temp);
LV_IMAGE_DECLARE(battery_voltage);


// Global configurations (NO 'static' keyword)
const gauge_config_t OIL_PRESS_CONFIG = {
    .type = PARAM_OIL_PRESS,
    .min_val = 0.0, .max_val = 5.0,
    .mid_val_pos = 4,
    .red_line_begin = 2.0f, .red_line_end = 8.0f, // 0.0 to 1.0 bar
    .n_ticks = 6,
    .unit = "bar",
    .show_redline = true,
    .redline_color = 0xE60000,
    .bg_image = &gauge_bgnd,
    .icon_image = &oil_pressure
};

const gauge_config_t WATER_TEMP_CONFIG = {
    .type = PARAM_WATER_TEMP,
    .min_val = 45.0, .max_val = 135.0,
    .mid_val_pos = 2,
    .red_line_begin = 1050.0f, .red_line_end = 1296.0f, // 105 to 150ish °C
    .n_ticks = 5,
    .unit = "ºC",
    .show_redline = true,
    .redline_color = 0xE60000,
    .bg_image = &gauge_bgnd,
    .icon_image = &water_temp

};

const gauge_config_t BATTERY_VOLTAGE_CONFIG = {
    .type = PARAM_BATTERY_VOLTAGE,
    .min_val = 10.0, .max_val = 15.0,
    .mid_val_pos = 3,
    .red_line_begin = 102.0f, .red_line_end = 122.0f, // 10.2 to 12.2ish °C
    .n_ticks = 6,
    .unit = "V",
    .show_redline = true,
    .redline_color = 0xE60000,
    .bg_image = &gauge_bgnd,
    .icon_image = &battery_voltage

};

const gauge_config_t OIL_TEMP_CONFIG = {
    .type = PARAM_OIL_TEMP,
    .min_val = 60.0, .max_val = 140.0,
    .mid_val_pos = 2,
    .red_line_begin = 1150.0f, .red_line_end = 1370.0f, // 115 to 140ish °C
    .n_ticks = 5,
    .unit = "ºC",
    .show_redline = true,
    .redline_color = 0xE60000,
    .bg_image = &gauge_bgnd,
    .icon_image = &oil_temp

};


// Static instances storage
static gauge_t * active_gauges[MAX_ACTIVE_GAUGES] = { NULL };
static int active_gauge_count = 0;

// Needle Geometry
static const float needle_length     = 88.0f;
static const float needle_base_width = 10.0f;

/**
 * Procedural Needle Draw Callback
 */
static void needle_draw_cb(lv_event_t * e) {
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_obj_t * obj = lv_event_get_target(e);

    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);

    float center_x = coords.x1 + (SLOT_WIDTH / 2.0f);
    float center_y = coords.y1 + (SLOT_HEIGHT / 2.0f) + LOWERING_AMM;

    gauge_t * g = (gauge_t *)lv_event_get_user_data(e);
    float angle_rad = g->current_needle_angle_rad;
    float perp_angle = angle_rad + 1.57079632679f;

    // Drop Shadow
    lv_draw_triangle_dsc_t shadow_dsc;
    lv_draw_triangle_dsc_init(&shadow_dsc);
    shadow_dsc.color = lv_color_hex(0x000000);
    shadow_dsc.opa   = LV_OPA_40;

    shadow_dsc.p[0].x = (int32_t)roundf(center_x + 2.0f + needle_length * cosf(angle_rad));
    shadow_dsc.p[0].y = (int32_t)roundf(center_y + 2.0f + needle_length * sinf(angle_rad));
    shadow_dsc.p[1].x = (int32_t)roundf(center_x + 2.0f + (needle_base_width / 2.0f) * cosf(perp_angle));
    shadow_dsc.p[1].y = (int32_t)roundf(center_y + 2.0f + (needle_base_width / 2.0f) * sinf(perp_angle));
    shadow_dsc.p[2].x = (int32_t)roundf(center_x + 2.0f - (needle_base_width / 2.0f) * cosf(perp_angle));
    shadow_dsc.p[2].y = (int32_t)roundf(center_y + 2.0f - (needle_base_width / 2.0f) * sinf(perp_angle));
    lv_draw_triangle(layer, &shadow_dsc);

    // Red Needle
    lv_draw_triangle_dsc_t tri_dsc;
    lv_draw_triangle_dsc_init(&tri_dsc);
    tri_dsc.color = COLOR_VAG_RED;
    tri_dsc.opa   = LV_OPA_COVER;

    tri_dsc.p[0].x = (int32_t)roundf(center_x + needle_length * cosf(angle_rad));
    tri_dsc.p[0].y = (int32_t)roundf(center_y + needle_length * sinf(angle_rad));
    tri_dsc.p[1].x = (int32_t)roundf(center_x + (needle_base_width / 2.0f) * cosf(perp_angle));
    tri_dsc.p[1].y = (int32_t)roundf(center_y + (needle_base_width / 2.0f) * sinf(perp_angle));
    tri_dsc.p[2].x = (int32_t)roundf(center_x - (needle_base_width / 2.0f) * cosf(perp_angle));
    tri_dsc.p[2].y = (int32_t)roundf(center_y - (needle_base_width / 2.0f) * sinf(perp_angle));
    lv_draw_triangle(layer, &tri_dsc);
}

void gauge_update_value(gauge_t * g, float new_val) {
    g->value = new_val;
    if (g->value < g->config.min_val) g->value = g->config.min_val;
    if (g->value > g->config.max_val) g->value = g->config.max_val;

    float range = g->config.max_val - g->config.min_val;
    float norm  = (range > 0.001f) ? ((g->value - g->config.min_val) / range) : 0.0f;
    float angle_deg = 220.0f + (norm * 100.0f);

    g->current_needle_angle_rad = angle_deg * (3.14159265f / 180.0f);

    if (g->needle) lv_obj_invalidate(g->needle);
    //if (g->label)  lv_label_set_text_fmt(g->label, "%.1f %s", g->value, g->config.unit);
}

void gauge_update_value_by_type(const param_type_t * type, float new_val) {
    for (int i = 0; i < active_gauge_count; ++i) {
      if (active_gauges[i]->config.type == type) {
        gauge_update_value(active_gauges[i], new_val);
      }
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

gauge_t * gauge_create(lv_obj_t * parent, int x_pos, const gauge_config_t * cfg) {

    assert(cfg->mid_val_pos < GAUGE_MAX_TICKS && "position of mid_val out of bounds");
    assert(cfg->mid_val_pos >= 0 && "position of mid_val cannot be negative");
    gauge_t * g = (gauge_t *)lv_malloc(sizeof(gauge_t));
    LV_ASSERT_MALLOC(g);

    g->config = *cfg;

    // Slot Container
    lv_obj_t * gauge_cell = lv_obj_create(parent);
    lv_obj_remove_style_all(gauge_cell);
    lv_obj_set_size(gauge_cell, SLOT_WIDTH, SLOT_HEIGHT);
    lv_obj_set_pos(gauge_cell, x_pos, 0);
    lv_obj_set_style_pad_all(gauge_cell, 0, LV_PART_MAIN);

    // Background Image Asset
    if (cfg->bg_image) {
        lv_obj_t * bg_img = lv_image_create(gauge_cell);
        lv_image_set_src(bg_img, cfg->bg_image);
        lv_obj_center(bg_img);
    }

    // Icon Image Asset
    if (cfg->icon_image) {
        lv_obj_t * icon_img = lv_image_create(gauge_cell);
        lv_image_set_src(icon_img, cfg->icon_image);
        lv_obj_center(icon_img);
    }

    // Scale Object
    g->scale = lv_scale_create(gauge_cell);
    lv_obj_set_size(g->scale, 175, 175);
    lv_obj_align(g->scale, LV_ALIGN_CENTER, 0, LOWERING_AMM);
    lv_scale_set_mode(g->scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_range(g->scale, (int32_t)(cfg->min_val * 10), (int32_t)(cfg->max_val * 10));
    lv_scale_set_angle_range(g->scale, 100);
    lv_scale_set_rotation(g->scale, 220);

    lv_scale_set_total_tick_count(g->scale, cfg->n_ticks);
    lv_scale_set_major_tick_every(g->scale, 1);

    // String formatting into persistent memory
    snprintf(g->start_label, sizeof(g->start_label), "%d", (int)g->config.min_val);
    snprintf(g->mid_label,   sizeof(g->mid_label),   "%d", (int)(((g->config.max_val - g->config.min_val) / (g->config.n_ticks - 1)) * g->config.mid_val_pos + g->config.min_val));
    snprintf(g->end_label,   sizeof(g->end_label),   "%d", (int)g->config.max_val);

    for (int i = 0; i < g->config.n_ticks; i++) {
        g->custom_labels[i] = "";
    }
    g->custom_labels[0] = g->start_label;
    //printf("Mid label: %s \n:", g->mid_label);
    g->custom_labels[g->config.mid_val_pos] = g->mid_label;
    g->custom_labels[g->config.n_ticks] = NULL; // Null terminator

    lv_scale_set_text_src(g->scale, g->custom_labels);
    lv_obj_set_style_text_font(g->scale, &ui_font_futura_bold, LV_PART_MAIN);

    // Remove Scale Line & Pad Styles
    lv_obj_set_style_arc_width(g->scale, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(g->scale, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(g->scale, 0, LV_PART_MAIN);

    // Ticks Styling
    lv_obj_set_style_line_color(g->scale, COLOR_VAG_WHITE, LV_PART_INDICATOR);
    lv_obj_set_style_line_width(g->scale, 4, LV_PART_INDICATOR);
    lv_obj_set_style_length(g->scale, 14, LV_PART_INDICATOR);

    lv_obj_set_style_line_color(g->scale, COLOR_VAG_WHITE, LV_PART_ITEMS);
    lv_obj_set_style_line_width(g->scale, 4, LV_PART_ITEMS);
    lv_obj_set_style_length(g->scale, 14, LV_PART_ITEMS);
    lv_obj_set_style_text_color(g->scale, COLOR_VAG_WHITE, LV_PART_MAIN);

    // Redline Danger Arc
    if (cfg->show_redline) {
        static lv_style_t style_red_arc;
        static bool style_inited = false;
        if (!style_inited) {
            lv_style_init(&style_red_arc);
            lv_style_set_arc_color(&style_red_arc, lv_color_hex(cfg->redline_color));
            lv_style_set_arc_width(&style_red_arc, 15);
            lv_style_set_pad_radial(&style_red_arc, -1);
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
    lv_obj_t * cap = lv_obj_create(gauge_cell);
    lv_obj_set_size(cap, 55, 55);
    lv_obj_set_style_radius(cap, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cap, COLOR_VAG_GR, 0);
    lv_obj_set_style_border_color(cap, COLOR_VAG_GR, 0);
    lv_obj_align_to(cap, g->scale, LV_ALIGN_CENTER, 0, 0);

    // Readout Label
    g->label = lv_label_create(gauge_cell);
    lv_obj_align(g->label, LV_ALIGN_BOTTOM_MID, 60, -20);
    lv_obj_set_style_text_color(g->label, COLOR_VAG_WHITE, LV_PART_MAIN);
    lv_obj_set_style_text_font(g->label, &ui_font_futura_bold, LV_PART_MAIN);
    lv_label_set_text(g->label, g->config.unit);

    gauge_update_value(g, cfg->min_val);
    register_gauge(g);

    return g;
}

void gauge_destroy(gauge_t * g) {
  if (!g) return;
}
