/**
 * @file task_manager.h
 *
 * @brief Task Manager API Registry
 */

#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

/* Includes ------------------------------------------------------------------*/

#include "esp_err.h"

// Component headers
#include "task_init.h"
#include "task_button.h"
#include "task_status.h"
#include "task_wifi.h"
#include "task_mqtt.h"
#include "task_mode.h"
#include "task_display.h"

// Manager headers
#include "button_handler.h"
#include "device_control.h"
#include "status_led.h"
#include "mode_manager.h"
#include "sensor_manager.h"
#include "sensor_reader.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

/* Exported defines ----------------------------------------------------------*/

#define VERSION_APP         CONFIG_VERSION_APP
#define INTERVAL_TIME_MS    CONFIG_INTERVAL_TIME_MS

/* External variables --------------------------------------------------------*/

/**
 * @brief Global state flags (defined in respective managers)
 */
extern bool isModeON; //!< Device mode state (mode_manager.c)
extern bool isWiFi;   //!< WiFi connection state (wifi_manager.c)
extern bool isMQTT;   //!< MQTT connection state (mqtt_manager.c)

/**
 * @brief Exported variables definitions
 */
extern char g_app_version[16];      //!< Application version string
extern uint32_t g_interval_time_ms; //!< Data publish interval in milliseconds

#endif /* TASK_MANAGER_H */