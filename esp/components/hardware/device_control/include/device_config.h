/**
 * @file device_config.h
 *
 * @brief Device Control Configuration from Kconfig
 */

#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#include "driver/gpio.h"
#include "sdkconfig.h"

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Device type enumeration
 */
typedef enum
{
    DEVICE_FAN = 0, //!< Fan device
    DEVICE_LIGHT,   //!< Light device
    DEVICE_MAX      //!< Total number of devices
} device_type_t;

/* Exported defines ----------------------------------------------------------*/

#define DEVICE_ACTIVE_LEVEL     CONFIG_DEVICE_ACTIVE_LEVEL
#define DEVICE_FAN_PIN          ((gpio_num_t)CONFIG_DEVICE_FAN_PIN)
#define DEVICE_LIGHT_PIN        ((gpio_num_t)CONFIG_DEVICE_LIGHT_PIN)

#endif /* DEVICE_CONFIG_H */
