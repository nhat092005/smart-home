/**
 * @file task_mode.h
 *
 * @brief Task Mode Header File
 */

#ifndef TASK_MODE_H
#define TASK_MODE_H

/* Includes ------------------------------------------------------------------*/

#include "esp_err.h"
#include "mode_manager.h"

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize display update task
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t task_mode_init(void);

/**
 * @brief Mode change event callback
 *
 * @param[in] old_mode Previous mode
 * @param[in] new_mode New mode
 */
void task_mode_change_event_callback(device_mode_t old_mode, device_mode_t new_mode);

/**
 * @brief Stop display task
 */
void task_mode_stop(void);

#endif /* TASK_MODE_H */
