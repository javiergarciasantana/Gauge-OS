/**
 * @file main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef _DEFAULT_SOURCE
  #define _DEFAULT_SOURCE /* needed for usleep() */
#endif

#include <stdlib.h>
#include <stdio.h>
#ifdef _MSC_VER
  #include <Windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include <SDL.h>

#include "hal/hal.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_USE_OS != LV_OS_FREERTOS


#include "lvgl.h"
#include <math.h>

// Color Palette (VW OEM Indigo Blue & Translucent Red)
#define COLOR_VAG_GR      lv_color_hex(0x474b4e)  // Grey Background
#define COLOR_VAG_DARK_GR lv_color_hex(0x474b4e)  // Dark Grey Housing
#define COLOR_VAG_BLUE    lv_color_hex(0x0088FF)  // Bright Blue Ticks
#define COLOR_VAG_WHITE   lv_color_hex(0xFFFFFF)  // White Text/Major Ticks
#define COLOR_VAG_RED     lv_color_hex(0xE60000)  // Translucent Red Danger Zone / Needle
#define SLOT_WIDTH     160
#define SLOT_HEIGHT    128
#define LOWERING_AMM   50

typedef struct {
    lv_obj_t * scale;
    lv_obj_t * needle;
    lv_obj_t * label;
    float current_needle_angle_rad;
    float value;
    // Dynamic string buffers per gauge instance
    char* unit;
    float start_val;
    float end_val;
    const char * custom_labels[7];
} gauge_t;

static gauge_t gauges[3];

// Needle dimensions
static float needle_length     = 88.0f;  // Length from pivot to sharp tip (in pixels)
static float needle_base_width = 10.0f;   // Base width (in pixels)


/**
 * Custom Draw Callback for Tapered Vector Needle
 */
static void needle_draw_cb(lv_event_t * e) {
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_obj_t * obj = lv_event_get_target(e);

    // Get absolute screen coordinates of the gauge container
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);

    float center_x = coords.x1 + (SLOT_WIDTH / 2.0f);
    float center_y = coords.y1 + (SLOT_HEIGHT / 2.0f) + LOWERING_AMM;

    printf("  Center: %f, %f \n", center_x, center_y);

    gauge_t * g = (gauge_t *)lv_event_get_user_data(e);
    float angle_rad = g->current_needle_angle_rad;
    // Perpendicular angle (+90 degrees in radians) to compute base width
    float perp_angle = angle_rad + 1.57079632679f;

    // SHADOW
    float shadow_dx = 2.0f;
    float shadow_dy = 2.0f;

    lv_draw_triangle_dsc_t shadow_dsc;
    lv_draw_triangle_dsc_init(&shadow_dsc);
    shadow_dsc.color = lv_color_hex(0x000000); // Dark black shadow
    shadow_dsc.opa   = LV_OPA_40;

    shadow_dsc.p[0].x = (int32_t)roundf(center_x + shadow_dx + needle_length * cosf(angle_rad));
    shadow_dsc.p[0].y = (int32_t)roundf(center_y + shadow_dy + needle_length * sinf(angle_rad));

    shadow_dsc.p[1].x = (int32_t)roundf(center_x + shadow_dx + (needle_base_width / 2.0f) * cosf(perp_angle));
    shadow_dsc.p[1].y = (int32_t)roundf(center_y + shadow_dy + (needle_base_width / 2.0f) * sinf(perp_angle));

    shadow_dsc.p[2].x = (int32_t)roundf(center_x + shadow_dx - (needle_base_width / 2.0f) * cosf(perp_angle));
    shadow_dsc.p[2].y = (int32_t)roundf(center_y + shadow_dy - (needle_base_width / 2.0f) * sinf(perp_angle));

    lv_draw_triangle(layer, &shadow_dsc);

    // RED NEEDLE

    lv_draw_triangle_dsc_t tri_dsc;
    lv_draw_triangle_dsc_init(&tri_dsc);

    tri_dsc.color = COLOR_VAG_RED;
    tri_dsc.opa = LV_OPA_COVER;

    // Point 0: Sharp Outer Tip
    tri_dsc.p[0].x = center_x + needle_length * cosf(angle_rad);
    tri_dsc.p[0].y = center_y + needle_length * sinf(angle_rad);

    // Point 1: Base Left Corner
    tri_dsc.p[1].x = center_x + (needle_base_width / 2.0f) * cosf(perp_angle);
    tri_dsc.p[1].y = center_y + (needle_base_width / 2.0f) * sinf(perp_angle);

    // Point 2: Base Right Corner
    tri_dsc.p[2].x = center_x - (needle_base_width / 2.0f) * cosf(perp_angle);
    tri_dsc.p[2].y = center_y - (needle_base_width / 2.0f) * sinf(perp_angle);

    lv_draw_triangle(layer, &tri_dsc);
}

/**
 * Create procedural needle container
 */
void create_code_needle(lv_obj_t * parent, int slot_index) {
    gauges[slot_index].needle = lv_obj_create(parent);
    lv_obj_remove_style_all(gauges[slot_index].needle);

    // Set size to 100% of parent screen so LVGL does not clip the triangle
    lv_obj_set_size(gauges[slot_index].needle, LV_PCT(100), LV_PCT(100));
    lv_obj_remove_flag(gauges[slot_index].needle, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(gauges[slot_index].needle, LV_OBJ_FLAG_SCROLLABLE);

    printf("Needle: %d ", slot_index + 1);

    // Register custom draw event
    lv_obj_add_event_cb(gauges[slot_index].needle, needle_draw_cb, LV_EVENT_DRAW_MAIN, &gauges[slot_index]);
  }

/**
 * Update needle angle and request redraw
 */
void update_needle_position(gauge_t* g, char unit[], float value, float start, float end) {
    if (value < start) value = start;
    if (value > end) value = end;

    // Map 0 - whatever val to 220° - 320° V-orientation angle
    float angle_deg = 220.0f + (value * (100.0f / end));
    g->current_needle_angle_rad = angle_deg * (3.14159265f / 180.0f);

    if (g->needle) lv_obj_invalidate(g->needle);
    if (g->label)  lv_label_set_text_fmt(g->label, "%.1f %s", value, unit);
}

static void oil_pressure_sim_cb(lv_timer_t * timer) {
    static float sim_val = 0.0f;
    gauge_t * g = (gauge_t *)lv_timer_get_user_data(timer);
    g->value += 1.0f;
    if (g->value > 5.0f) g->value = 0.0f;

    update_needle_position(g, g->unit, g->value, g->start_val, g->end_val);
}


/**
 * Initialize the VW Polo 6N2 Oil Pressure Gauge UI
 */
void create_polo_gauge_ui(int slot_index, float red_line_range_begin, float red_line_range_end, char *unit, float start_value, float end_value) {
    lv_obj_t * scr = lv_screen_active();
    printf("Gauge: %d ", slot_index + 1);
    printf("X %d \n", slot_index * 160);

    // 1. Background Style
    lv_obj_set_style_bg_color(scr, COLOR_VAG_GR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    // 1. Create a 160x128 container box for Slot N (e.g., slot 0 = left, slot 1 = middle, slot 2 = right)
    lv_obj_t * gauge_cell = lv_obj_create(scr);
    // Remove container background styling/borders
    lv_obj_remove_style_all(gauge_cell);
    // Needs to be removed first since pos and size are also stored in style
    lv_obj_set_size(gauge_cell, 160, 128);
    lv_obj_set_pos(gauge_cell, slot_index * 160, 0); // Position at X=0, 160, or 320

    //lv_obj_add_event_cb(gauge_cell, draw_embossed_fan_bg, LV_EVENT_DRAW_MAIN, NULL);

    lv_obj_remove_flag(gauge_cell, LV_OBJ_FLAG_SCROLLABLE);

    // Remove default padding so layout coordinates inside remain exact
    lv_obj_set_style_pad_all(gauge_cell, 0, LV_PART_MAIN);

    // 2. Center scale widget inside the 160x128 container
    gauges[slot_index].scale = lv_scale_create(gauge_cell);
    lv_obj_set_size(gauges[slot_index].scale, 175, 175);
    lv_obj_center(gauges[slot_index].scale);

    lv_obj_align(gauges[slot_index].scale, LV_ALIGN_CENTER, 0, LOWERING_AMM);

    lv_scale_set_mode(gauges[slot_index].scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_range(gauges[slot_index].scale, start_value * 10, end_value * 10);                 // 0.0 to 5.0 bar range (scaled x10)

    // V-Orientation Sweep (100° arc, 2.5 bar at top 12 o'clock)
    lv_scale_set_angle_range(gauges[slot_index].scale, 100);
    lv_scale_set_rotation(gauges[slot_index].scale, 220);

    // Ticks Setup
    lv_scale_set_total_tick_count(gauges[slot_index].scale, 6);
    lv_scale_set_major_tick_every(gauges[slot_index].scale, 1);

    gauges[slot_index].start_val = start_value;
    gauges[slot_index].end_val = end_value;
    gauges[slot_index].unit = unit;
    gauges[slot_index].value = start_value;

    char start_str[4];
    char end_str[4];

    snprintf(start_str, 4, "%.1f", start_value);
    snprintf(end_str, 4, "%.1f", end_value);

    // Assign pointers dynamically at runtime (NOT static)
    gauges[slot_index].custom_labels[0] = start_str;
    gauges[slot_index].custom_labels[1] = "";
    gauges[slot_index].custom_labels[2] = "";
    gauges[slot_index].custom_labels[3] = "";
    gauges[slot_index].custom_labels[4] = "";
    gauges[slot_index].custom_labels[5] = end_str;
    gauges[slot_index].custom_labels[6] = NULL; // Array terminator

    lv_scale_set_text_src(gauges[slot_index].scale, gauges[slot_index].custom_labels);

    // 3. Remove Outer Circumference Line
    lv_obj_set_style_arc_width(gauges[slot_index].scale, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(gauges[slot_index].scale, 0, LV_PART_MAIN);

    // 4. Tick Styling (Thicker & Taller White Lines)
    lv_obj_set_style_line_color(gauges[slot_index].scale, COLOR_VAG_WHITE, LV_PART_INDICATOR);
    lv_obj_set_style_line_width(gauges[slot_index].scale, 4, LV_PART_INDICATOR);
    lv_obj_set_style_length(gauges[slot_index].scale, 14, LV_PART_INDICATOR);

    lv_obj_set_style_line_color(gauges[slot_index].scale, COLOR_VAG_WHITE, LV_PART_ITEMS);
    lv_obj_set_style_line_width(gauges[slot_index].scale, 4, LV_PART_ITEMS);
    lv_obj_set_style_length(gauges[slot_index].scale, 14, LV_PART_ITEMS);

    lv_obj_set_style_text_color(gauges[slot_index].scale, COLOR_VAG_WHITE, LV_PART_MAIN);


    // 5. Solid Red Danger Arc (0.0 to 1.0 bar)
    static lv_style_t style_red_arc;
    static bool style_inited = false;
    if (!style_inited) {
      lv_style_init(&style_red_arc);
      lv_style_set_arc_color(&style_red_arc, COLOR_VAG_RED);
      lv_style_set_arc_width(&style_red_arc, 15);
      style_inited = true;
    }

    lv_scale_section_t * red_sec = lv_scale_add_section(gauges[slot_index].scale);
    lv_scale_section_set_range(red_sec, red_line_range_begin, red_line_range_end);
    lv_scale_set_section_style_main(gauges[slot_index].scale, red_sec, &style_red_arc);

    // Needle
    create_code_needle(gauge_cell, slot_index);

    // 7. Center Hub Cap (Drawn on top of line start point)
    lv_obj_t * needle_cap = lv_obj_create(gauge_cell);
    lv_obj_set_size(needle_cap, 30, 30);
    lv_obj_set_style_radius(needle_cap, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(needle_cap, COLOR_VAG_DARK_GR, 0);
    lv_obj_set_style_border_color(needle_cap, COLOR_VAG_DARK_GR, 0);
    lv_obj_set_style_border_width(needle_cap, 2, 0);
    lv_obj_remove_flag(needle_cap, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(needle_cap, gauges[slot_index].scale, LV_ALIGN_CENTER, 0, 0);

    // 8. Unit Readout Label
    gauges[slot_index].label = lv_label_create(gauge_cell);
    lv_obj_align(gauges[slot_index].label, LV_ALIGN_BOTTOM_MID, 0, -6);
    lv_obj_set_style_text_color(gauges[slot_index].label, COLOR_VAG_WHITE, LV_PART_MAIN);

    LV_FONT_DECLARE(ui_font_eurostile_extended);
    lv_obj_set_style_text_font(gauges[slot_index].label, &ui_font_eurostile_extended, LV_PART_MAIN);
    lv_label_set_text(gauges[slot_index].label, "0.0 bar");

    // Store initial properties inside gauge instance
    update_needle_position(&gauges[slot_index], unit, 0.0f, start_value, end_value);

    lv_timer_create(oil_pressure_sim_cb, 1000, &gauges[slot_index]);
}



int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();
  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  sdl_hal_init(480, 128);

  create_polo_gauge_ui(0, 2, 8, "bar", 0, 5);
  create_polo_gauge_ui(1, 100, 576, "ºC", 0, 180);
  create_polo_gauge_ui(2, 9.6, 76.8, "V", 0, 24);


  while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    uint32_t sleep_time_ms = lv_timer_handler();
    if(sleep_time_ms == LV_NO_TIMER_READY){
	sleep_time_ms =  LV_DEF_REFR_PERIOD;
    }
#ifdef _MSC_VER
    Sleep(sleep_time_ms);
#else
    usleep(sleep_time_ms * 1000);
#endif
  }

  return 0;
}


#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/
