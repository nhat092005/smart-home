/**
 * @file device_control.h
 *
 * @brief Device Control API
 */

#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

/* Includes ------------------------------------------------------------------*/

#include "device_config.h"
#include "esp_err.h"
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Device state enumeration
 */
typedef enum
{
    DEVICE_OFF = 0, //!< Device is OFF
    DEVICE_ON = 1   //!< Device is ON
} device_state_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize device control
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t device_control_init(void);

/**
 * @brief Set device state
 *
 * @param[in] device Device type (DEVICE_FAN or DEVICE_LIGHT)
 * @param[in] state Device state (DEVICE_ON or DEVICE_OFF)
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t device_control_set_state(device_type_t device, device_state_t state);

/**
 * @brief Get current device state
 *
 * @param[in] device Device type (DEVICE_FAN or DEVICE_LIGHT)
 * @param[out] state Pointer to store current state
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t device_control_get_state(device_type_t device, device_state_t *state);

/**
 * @brief Toggle device state
 *
 * @param[in] device Device type (DEVICE_FAN or DEVICE_LIGHT)
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t device_control_toggle(device_type_t device);

/**
 * @brief Deinitialize device control system
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t device_control_deinit(void);

#endif /* DEVICE_CONTROL_H */
