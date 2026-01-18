/**
 * @file status_led.h
 *
 * @brief Status LED Control API
 */

#ifndef STATUS_LED_H
#define STATUS_LED_H

/* Includes ------------------------------------------------------------------*/

#include "status_config.h"
#include "esp_err.h"

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize all status LEDs
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t status_led_init(void);

/**
 * @brief Set LED state
 *
 * @param[in] led LED type
 * @param[in] state LED state (LED_ON or LED_OFF)
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t status_led_set_state(led_type_t led, led_state_t state);

/**
 * @brief Get LED state
 *
 * @param[in] led LED type
 * @param[out] state Pointer to store LED state
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t status_led_get_state(led_type_t led, led_state_t *state);

/**
 * @brief Toggle LED state
 *
 * @param[in] led LED type
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t status_led_toggle(led_type_t led);

/**
 * @brief Deinitialize all status LEDs
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t status_led_deinit(void);

#endif /* STATUS_LED_H */