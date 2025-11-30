/**
 * @file button_config.h
 * 
 * @brief Button Handler Configuration from Kconfig
 */

#ifndef BUTTON_CONFIG_H
#define BUTTON_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "sdkconfig.h"

/* Exported defines ----------------------------------------------------------*/
#define BUTTON_MAX_HANDLERS     4
#define BUTTON_DEVICE           CONFIG_BUTTON_DEVICE
#define BUTTON_INTERVAL         CONFIG_BUTTON_INTERVAL
#define BUTTON_LIGHT            CONFIG_BUTTON_LIGHT
#define BUTTON_FAN              CONFIG_BUTTON_FAN

// Button timing defaults (in milliseconds)
#define DEBOUNCE_TIME_MS        CONFIG_DEBOUNCE_TIME_MS

#endif /* BUTTON_CONFIG_H */
