// /**
//  * @file main.c
//  *
//  */

// /*********************
//  *      DEFINES
//  *********************/

// /**********************
//  *      TYPEDEFS
//  **********************/

// /**********************
//  *  STATIC PROTOTYPES
//  **********************/

// /**********************
//  *  STATIC VARIABLES
//  **********************/

// /**********************
//  *      MACROS
//  **********************/

// /**********************
//  *   GLOBAL FUNCTIONS
//  **********************/

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
#include <SDL.h>

#include "hal/hal.h"
#include "dashboard.h"

#if LV_USE_OS != LV_OS_FREERTOS

float oil_pr = 0.0f;
float bat_volt = 10.0f;

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void sim_timer_cb(lv_timer_t * t) {
    param_type_t type = (param_type_t)(uintptr_t)lv_timer_get_user_data(t);
    switch (type) {
      case PARAM_OIL_PRESS:
        if (oil_pr > 5.0) {
          oil_pr = 0.0f;
        } else {
          oil_pr += 0.2f;
        }
        gauge_update_value_by_type(type, oil_pr);
        break;

      case PARAM_BATTERY_VOLTAGE:
        if (bat_volt > 15.0) {
          bat_volt = 10.0f;
        } else {
          bat_volt += 0.2f;
        }
        gauge_update_value_by_type(type, bat_volt);
        break;

      default:
      break;
    }

}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    lv_init();
    sdl_hal_init(480, 128);

    // Create the input group for encoder/keyboard control
    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

    // Find keyboard input driver and bind it to our group
    lv_indev_t * indev = NULL;
    while((indev = lv_indev_get_next(indev)) != NULL) {
        if(lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(indev, g);
            break;
        }
    }

    // Initialize the dynamic carousel UI (replaces static gauge_create calls)
    dashboard_create(g, &ANALOGUE, 480, 128, 3);

    // Timers
    lv_timer_create(sim_timer_cb, 300, (void *)(uintptr_t)PARAM_OIL_PRESS);
    lv_timer_create(sim_timer_cb, 300, (void *)(uintptr_t)PARAM_BATTERY_VOLTAGE);
    //lv_timer_create(sim_timer_cb, 300, (void *)(uintptr_t)PARAM_OIL_PRESS);


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
