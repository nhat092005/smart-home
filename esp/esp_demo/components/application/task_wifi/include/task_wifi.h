/**
 * @file task_wifi.h
 *
 * @brief Task WiFi Header API
 */

#ifndef TASK_WIFI_H
#define TASK_WIFI_H

/* Includes ------------------------------------------------------------------*/

#include "wifi_manager.h"

/* Exported functions --------------------------------------------------------*/

/**
 * @brief WiFi event callback implementation
 *
 * @param[in] event WiFi manager event
 * @param[in] data Pointer to event data (if any)
 */
void task_wifi_event_callback(wifi_manager_event_t event, void *data);

/**
 * @brief Initialize WiFi connecting task
 */
esp_err_t task_wifi_set_wifi_connecting_init(void);

#endif /* TASK_WIFI_H */