/**
 * @file task_manager.h
 *
 * @brief Task Manager API
 */

#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

/* Includes ------------------------------------------------------------------*/

#include "esp_err.h"
#include "button_handler.h"
#include "device_control.h"
#include "status_led.h"
#include "mode_manager.h"
#include "sensor_manager.h"
#include "wifi_manager.h"

/* External variables ---------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern bool isModeON;
extern bool isWiFi;
extern bool isMQTT;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize system components
 */
void task_init(void);

/**
 * @brief Button interval callback functions
 *
 * @param[in] button Button type
 */
void task_button_wifi_pressed(button_type_t button);

/**
 * @brief Button device callback functions
 *
 * @param[in] button Button type
 */
void task_button_device_pressed(button_type_t button);

/**
 * @brief Button light callback functions
 *
 * @param[in] button Button type
 */
void task_button_light_pressed(button_type_t button);

/**
 * @brief Button fan callback functions
 *
 * @param[in] button Button type
 */
void task_button_fan_pressed(button_type_t button);

/**
 * @brief Task polling LED
 */
esp_err_t task_status_set_init(void);

/**
 * @brief WiFi event callback handler
 *
 * @param[in] event WiFi manager event
 * @param[in] data Event data
 */
void task_wifi_event_callback(wifi_manager_event_t event, void *data);

/**
 * @brief Initialize WiFi connecting task
 */
esp_err_t task_wifi_set_wifi_connecting_init();

#endif /* TASK_MANAGER_H */