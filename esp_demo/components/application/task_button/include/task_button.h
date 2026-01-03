/**
 * @file task_button.h
 * @brief Task Button Header API
 */

#ifndef TASK_BUTTON_H
#define TASK_BUTTON_H

/* Includes ------------------------------------------------------------------*/

#include "button_config.h"

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize button processing task and queue
 */
esp_err_t task_button_init(void);

/**
 * @brief Button interval callback implementations - just queues event
 *
 * @param[in] button Button type pressed
 */
void task_button_wifi_pressed(button_type_t button);

/**
 * @brief Button device callback implementations - just queues event
 *
 * @param[in] button Button type pressed
 */
void task_button_mode_pressed(button_type_t button);

/**
 * @brief Button light callback implementations - just queues event
 *
 * @param[in] button Button type pressed
 */
void task_button_light_pressed(button_type_t button);

/**
 * @brief Button fan callback implementations - just queues event
 *
 * @param[in] button Button type pressed
 */
void task_button_fan_pressed(button_type_t button);

/**
 * @brief Button ac callback implementations - just queues event
 *
 * @param[in] button Button type pressed
 */
void task_button_ac_pressed(button_type_t button);

#endif /* TASK_BUTTON_H */