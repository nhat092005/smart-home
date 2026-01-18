/**
 * @file button_config.h
 *
 * @brief Button Handler Configuration from Kconfig
 */

#ifndef BUTTON_CONFIG_H
#define BUTTON_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#include "driver/gpio.h"
#include "sdkconfig.h"

/* Exported types --------------------------------------------------------*/

/**
 * @brief Button type enumeration
 */
typedef enum
{
    BUTTON_MODE = 0, //!< Mode button
    BUTTON_WIFI,     //!< WiFi button
    BUTTON_LIGHT,    //!< Light button
    BUTTON_FAN,      //!< Fan button
    BUTTON_AC,       //!< AC button
    BUTTON_MAX       //!< Total number of buttons
} button_type_t;

/* Exported defines --------------------------------------------------------*/

/* Timing definitions */
#define BUTTON_POLL_INTERVAL_MS     CONFIG_BUTTON_POLL_INTERVAL_MS
#define DEBOUNCE_TIME_MS            CONFIG_DEBOUNCE_TIME_MS

/* Button pin definitions */
#define BUTTON_MODE_PIN             ((gpio_num_t)CONFIG_BUTTON_MODE)
#define BUTTON_WIFI_PIN             ((gpio_num_t)CONFIG_BUTTON_WIFI)
#define BUTTON_LIGHT_PIN            ((gpio_num_t)CONFIG_BUTTON_LIGHT)
#define BUTTON_FAN_PIN              ((gpio_num_t)CONFIG_BUTTON_FAN)
#define BUTTON_AC_PIN               ((gpio_num_t)CONFIG_BUTTON_AC)

#endif /* BUTTON_CONFIG_H */
