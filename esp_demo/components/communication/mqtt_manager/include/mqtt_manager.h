/**
 * @file mqtt_manager.h
 *
 * @brief MQTT Manager API
 */

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

/* Includes ------------------------------------------------------------------*/

#include "esp_err.h"
#include "cJSON.h"
#include "mqtt_config.h"
#include <stdbool.h>
#include <stdint.h>

/* Exported variables --------------------------------------------------------*/

/**
 * @brief Global MQTT connection state
 *
 * @note true when MQTT client is connected to broker, false otherwise
 */
extern bool isMQTT;

/* Exported types ------------------------------------------------------------*/

/**
 * @brief MQTT event callback function types
 */
typedef void (*mqtt_event_callback_t)(void);

/**
 * @brief MQTT command callback function type
 *
 * @param cmd_id Command ID (e.g., "a1b2")
 * @param command Command name (e.g., "set_device", "reboot")
 * @param params JSON parameters object (can be NULL if no params)
 */
typedef void (*mqtt_command_callback_t)(const char *cmd_id, const char *command, cJSON *params);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize MQTT manager
 *
 * @return ESP_OK on success, error code otherwise
 *
 * @note Builds topic strings from Kconfig and prepares MQTT client
 */
esp_err_t mqtt_manager_init(void);

/**
 * @brief Start MQTT client and connect to broker
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t mqtt_manager_start(void);

/**
 * @brief Stop MQTT client
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t mqtt_manager_stop(void);

/**
 * @brief Check if MQTT is connected
 *
 * @return true if connected to broker
 */
bool mqtt_manager_is_connected(void);

/**
 * @brief Publish sensor data to {base}/{device_id}/data
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[in] temperature Temperature in Celsius
 * @param[in] humidity Humidity in percentage
 * @param[in] light Light level (lux)
 *
 * @return ESP_OK on success, error code otherwise
 *
 * @note QoS: 0, Retain: No, Frequency: Every N seconds
 */
esp_err_t mqtt_manager_publish_data(uint32_t timestamp, float temperature,
                                    float humidity, int light);

/**
 * @brief Publish device state to {base}/{device_id}/state
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[in] mode Mode state (1=ON, 0=OFF)
 * @param[in] interval Data reporting interval in seconds
 * @param[in] fan Fan state (1=ON, 0=OFF)
 * @param[in] light Light state (1=ON, 0=OFF)
 * @param[in] ac AC state (1=ON, 0=OFF)
 *
 * @return ESP_OK on success, error code otherwise
 *
 * @note QoS: 1, Retain: Yes, Frequency: On change + backup 60s
 */
esp_err_t mqtt_manager_publish_state(uint32_t timestamp, int mode, int interval, int fan, int light, int ac);

/**
 * @brief Publish device info to {base}/{device_id}/info
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[in] device_id Device identifier
 * @param[in] ssid WiFi SSID
 * @param[in] ip IP address string
 * @param[in] broker MQTT broker URI
 * @param[in] firmware Firmware version string
 *
 * @return ESP_OK on success, error code otherwise
 *
 * @note QoS: 1, Retain: Yes, Frequency: Boot + network change
 */
esp_err_t mqtt_manager_publish_info(uint32_t timestamp, const char *device_id, const char *ssid,
                                    const char *ip, const char *broker, const char *firmware);

/**
 * @brief Publish command response to {base}/{device_id}/response
 *
 * @param[in] cmd_id Command ID (e.g., "a1b2")
 * @param[in] status Status string (e.g., "success", "error")
 *
 * @return ESP_OK on success, error code otherwise
 *
 * @note QoS: 1, Retain: Yes
 */
esp_err_t mqtt_manager_publish_response(const char *cmd_id, const char *status);

/**
 * @brief Register callback for incoming commands
 *
 * @param[in] callback Function to handle commands
 *
 * @note The callback will be invoked when a command is received on {base}/{device_id}/command
 */
void mqtt_manager_register_command_callback(mqtt_command_callback_t callback);

/**
 * @brief Register callback for MQTT connected event
 *
 * @param[in] callback Function to call when connected to broker
 */
void mqtt_manager_register_connected_callback(mqtt_event_callback_t callback);

/**
 * @brief Register callback for MQTT disconnected event
 *
 * @param[in] callback Function to call when disconnected from broker
 */
void mqtt_manager_register_disconnected_callback(mqtt_event_callback_t callback);

#endif /* MQTT_MANAGER_H */