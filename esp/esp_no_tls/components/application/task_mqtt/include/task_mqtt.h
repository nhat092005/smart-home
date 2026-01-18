/**
 * @file task_mqtt.h
 * @brief Task MQTT Header API
 */

#ifndef TASK_MQTT_H
#define TASK_MQTT_H

/* Includes ------------------------------------------------------------------*/

#include "esp_err.h"

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Callback when MQTT connects to broker
 */
void task_mqtt_on_connected(void);

/**
 * @brief Callback when MQTT disconnects from broker
 */
void task_mqtt_on_disconnected(void);

/**
 * @brief Publish current device state
 */
void task_mqtt_publish_current_state(void);

/**
 * @brief Callback to provide sensor data before publishing /data
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[out] temp Pointer to temperature value
 * @param[out] hum Pointer to humidity value
 * @param[out] light Pointer to light value
 */
void task_mqtt_on_data_publish(uint32_t timestamp, float *temp, float *hum, int *light);

/**
 * @brief Callback to sync state before publishing /state
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[out] mode Pointer to mode value
 * @param[out] fan Pointer to fan state
 * @param[out] light Pointer to light state
 * @param[out] ac Pointer to AC state
 */
void task_mqtt_on_state_publish(uint32_t timestamp, int *mode, int *fan, int *light, int *ac);

/* Command Callbacks ---------------------------------------------------------*/

/**
 * @brief Handle set_device command
 *
 * @param[in] cmd_id Command ID
 * @param[in] device Device name
 * @param[in] state Device state
 */
void task_mqtt_on_set_device(const char *cmd_id, const char *device, int state);

/**
 * @brief Handle set_devices command
 *
 * @param[in] cmd_id Command ID
 * @param[in] fan Fan state
 * @param[in] light Light state
 * @param[in] ac AC state
 */
void task_mqtt_on_set_devices(const char *cmd_id, int fan, int light, int ac);

/**
 * @brief Handle set_mode command
 *
 * @param[in] cmd_id Command ID
 * @param[in] mode Mode value
 */
void task_mqtt_on_set_mode(const char *cmd_id, int mode);

/**
 * @brief Handle set_interval command
 *
 * @param[in] cmd_id Command ID
 * @param[in] interval Interval value
 */
void task_mqtt_on_set_interval(const char *cmd_id, int interval);

/**
 * @brief Handle set_timestamp command
 *
 * @param[in] cmd_id Command ID
 * @param[in] timestamp Unix timestamp
 */
void task_mqtt_on_set_timestamp(const char *cmd_id, uint32_t timestamp);

/**
 * @brief Handle get_status command
 *
 * @param[in] cmd_id Command ID
 */
void task_mqtt_on_get_status(const char *cmd_id);

/**
 * @brief Handle reboot command
 *
 * @param[in] cmd_id Command ID
 */
void task_mqtt_on_reboot(const char *cmd_id);

/**
 * @brief Handle factory_reset command
 *
 * @param[in] cmd_id Command ID
 */
void task_mqtt_on_factory_reset(const char *cmd_id);

/**
 * @brief Initialize MQTT task and register callbacks
 */

esp_err_t task_mqtt_init(void);

#endif /* TASK_MQTT_H */