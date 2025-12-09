/**
 * @file mode_manager.h
 *
 * @brief Mode Manager API
 */

#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

/* Includes ------------------------------------------------------------------*/

#include "esp_err.h"
#include <stdbool.h>

/* External variables --------------------------------------------------------*/
extern bool isModeON;

/* Exported types --------------------------------------------------------*/

/**
 * @brief Device operation modes
 */
typedef enum
{
    MODE_OFF = 0, //!< Device off
    MODE_ON       //!< Device on (normal operation)
} device_mode_t;

/**
 * @brief Mode change callback function type
 *
 * @param[in] old_mode Previous mode
 * @param[in] new_mode New mode
 */
typedef void (*mode_change_callback_t)(device_mode_t old_mode, device_mode_t new_mode);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize mode manager
 *
 * @return ESP_OK on success, error code otherwise
 *
 * @note Loads saved mode from NVS or defaults to MODE_OFF
 */
esp_err_t mode_manager_init(void);

/**
 * @brief Set device operation mode
 *
 * @param[in] mode New mode to set
 *
 * @return ESP_OK on success, error code otherwise
 *
 * @note Changes mode and saves to NVS for persistence across reboots
 */
esp_err_t mode_manager_set_mode(device_mode_t mode);

/**
 * @brief Toggle device operation mode
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t mode_manager_toggle_mode(void);

/**
 * @brief Get current device operation mode
 *
 * @return Current mode (MODE_OFF or MODE_ON)
 */
device_mode_t mode_manager_get_mode(void);

/**
 * @brief Register callback for mode changes
 *
 * @param[in] callback Function to handle mode changes
 *
 * @note The callback will be invoked whenever mode is changed via mode_manager_set_mode()
 */
void mode_manager_register_change_callback(mode_change_callback_t callback);

#endif /* MODE_MANAGER_H */