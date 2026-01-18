/**
 * @file button_handler.h
 *
 * @brief Button Handler API
 */

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

/* Includes ------------------------------------------------------------------*/

#include "button_config.h"
#include "esp_err.h"
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Button callback type
 */
typedef void (*button_callback_t)(button_type_t button);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize button handler
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t button_handler_init(void);

/**
 * @brief Set callback for specific button
 *
 * @param[in] button Button type
 * @param[in] callback Callback function when button is pressed
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t button_handler_set_callback(button_type_t button, button_callback_t callback);

/**
 * @brief Get button state
 *
 * @param[in] button Button type
 *
 * @return true if pressed, false otherwise
 */
bool button_handler_is_pressed(button_type_t button);

/**
 * @brief Deinitialize button handler
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t button_handler_deinit(void);

#endif /* BUTTON_HANDLER_H */
