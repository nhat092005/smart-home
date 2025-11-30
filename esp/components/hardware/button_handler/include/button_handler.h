/**
 * @file button_handler.h
 *
 * @brief Button Handler API for GPIO Buttons
 */

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

/* Includes ------------------------------------------------------------------*/
#include "button_config.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Button press callback function type
 *
 * @param gpio_num GPIO number of the button that was pressed
 */
typedef void (*button_press_callback_t)(gpio_num_t gpio_num);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize button handler with default configuration
 *
 * @param[in] gpio_num GPIO number for button
 * @param[in] callback Callback function when button is pressed
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_ARG if callback is NULL
 *     - ESP_ERR_NO_MEM if maximum buttons reached
 */
esp_err_t button_handler_init(gpio_num_t gpio_num, button_press_callback_t callback);

/**
 * @brief Deinitialize all buttons and free resources
 *
 * @return ESP_OK on success
 */
esp_err_t button_handler_deinit(void);

/**
 * @brief Get current button state
 *
 * @param[in] gpio_num GPIO number of button
 * @return
 *     - true if button is currently pressed
 *     - false if button is released or not found
 */
bool button_handler_is_pressed(gpio_num_t gpio_num);

#endif /* BUTTON_HANDLER_H */
