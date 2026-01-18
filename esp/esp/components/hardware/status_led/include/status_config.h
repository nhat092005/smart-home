/**
 * @file status_config.h
 *
 * @brief Status Configurations from Kconfig
 */

#ifndef STATUS_CONFIG_H
#define STATUS_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#include "driver/gpio.h"
#include "sdkconfig.h"

/* Exported types ------------------------------------------------------------*/

/**
 * @brief LED type enumeration
 */
typedef enum
{
    LED_DEVICE = 0, //!< Device LED
    LED_WIFI,       //!< WiFi LED
    LED_MQTT,       //!< MQTT LED
    LED_MAX         //!< Maximum number of LEDs
} led_type_t;

/**
 * @brief LED state enumeration
 */
typedef enum
{
    LED_OFF = 0, //!< LED off state
    LED_ON = 1   //!< LED on state
} led_state_t;

/* Exported defines ----------------------------------------------------------*/

/* LED active level (from Kconfig) */
#define LED_ACTIVE_LEVEL    CONFIG_LED_ACTIVE_LEVEL

/* LED pin definitions */
#define LED_DEVICE_PIN      ((gpio_num_t)CONFIG_LED_GPIO_DEVICE)
#define LED_WIFI_PIN        ((gpio_num_t)CONFIG_LED_GPIO_WIFI)
#define LED_MQTT_PIN        ((gpio_num_t)CONFIG_LED_GPIO_MQTT)

#endif /* STATUS_CONFIG_H */