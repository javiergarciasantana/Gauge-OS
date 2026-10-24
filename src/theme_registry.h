#ifndef THEME_REGISTRY_H
#define THEME_REGISTRY_H

#include "theme.h"

/**********************
 * TYPEDEFS
 **********************/
typedef enum {
    THEME_ID_VW_POLO_6N2,
    THEME_ID_VW_DIGITAL,
    THEME_ID_COUNT
} theme_id_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Gets theme structure by enum ID
 */
const screen_theme_t * theme_get_by_id(theme_id_t id);

/**
 * Gets roller option string (e.g., "VW Analogue\nVW Digital\nSaxo VTS")
 */
const char * theme_get_roller_options(bool caps_lock);

#endif
