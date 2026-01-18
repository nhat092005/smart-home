/**
 * @file mqtt_callback.h
 *
 * @brief MQTT Callback Manager API
 */

#ifndef MQTT_CALLBACK_H
#define MQTT_CALLBACK_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

/**
 * @brief MQTT event callback types
 */
typedef void (*mqtt_event_connected_cb_t)(void);
typedef void (*mqtt_event_disconnected_cb_t)(void);
typedef void (*mqtt_event_data_publish_cb_t)(uint32_t timestamp, float *temp, float *hum, int *light);
typedef void (*mqtt_event_state_publish_cb_t)(uint32_t timestamp, int *mode, int *fan, int *light, int *ac);

/**
 * @brief MQTT command callback types
 */
typedef void (*mqtt_cmd_set_device_cb_t)(const char *cmd_id, const char *device, int state);
typedef void (*mqtt_cmd_set_devices_cb_t)(const char *cmd_id, int fan, int light, int ac);
typedef void (*mqtt_cmd_set_mode_cb_t)(const char *cmd_id, int mode);
typedef void (*mqtt_cmd_set_interval_cb_t)(const char *cmd_id, int interval);
typedef void (*mqtt_cmd_set_timestamp_cb_t)(const char *cmd_id, uint32_t timestamp);
typedef void (*mqtt_cmd_get_status_cb_t)(const char *cmd_id);
typedef void (*mqtt_cmd_ping_cb_t)(const char *cmd_id);
typedef void (*mqtt_cmd_reboot_cb_t)(const char *cmd_id);
typedef void (*mqtt_cmd_factory_reset_cb_t)(const char *cmd_id);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Callback registration functions
 *
 * @param[in] callback Function pointer to register
 */
void mqtt_callback_register_on_connected(mqtt_event_connected_cb_t callback);
void mqtt_callback_register_on_disconnected(mqtt_event_disconnected_cb_t callback);
void mqtt_callback_register_on_data_publish(mqtt_event_data_publish_cb_t callback);
void mqtt_callback_register_on_state_publish(mqtt_event_state_publish_cb_t callback);
void mqtt_callback_register_on_set_device(mqtt_cmd_set_device_cb_t callback);
void mqtt_callback_register_on_set_devices(mqtt_cmd_set_devices_cb_t callback);
void mqtt_callback_register_on_set_mode(mqtt_cmd_set_mode_cb_t callback);
void mqtt_callback_register_on_set_interval(mqtt_cmd_set_interval_cb_t callback);
void mqtt_callback_register_on_set_timestamp(mqtt_cmd_set_timestamp_cb_t callback);
void mqtt_callback_register_on_get_status(mqtt_cmd_get_status_cb_t callback);
void mqtt_callback_register_on_ping(mqtt_cmd_ping_cb_t callback);
void mqtt_callback_register_on_reboot(mqtt_cmd_reboot_cb_t callback);
void mqtt_callback_register_on_factory_reset(mqtt_cmd_factory_reset_cb_t callback);

/**
 * @brief Initialize MQTT Callback Manager
 */
void mqtt_callback_init(void);

/**
 * @brief Callback invocation connected
 */
void mqtt_callback_invoke_connected(void);

/**
 * @brief Callback invocation disconnected
 */
void mqtt_callback_invoke_disconnected(void);

/**
 * @brief Callback invocation data publish
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[out] temp Pointer to temperature value
 * @param[out] hum Pointer to humidity value
 * @param[out] light Pointer to light value
 */
void mqtt_callback_invoke_data_publish(uint32_t timestamp, float *temp, float *hum, int *light);

/**
 * @brief Callback invocation state publish
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[out] mode Pointer to mode value
 * @param[out] fan Pointer to fan value
 * @param[out] light Pointer to light value
 * @param[out] ac Pointer to ac value
 */
void mqtt_callback_invoke_state_publish(uint32_t timestamp, int *mode, int *fan, int *light, int *ac);

/**
 * @brief Callback invocation set device command
 *
 * @param[in] cmd_id Command ID
 * @param[in] device Device name
 * @param[in] state Device state
 */
void mqtt_callback_invoke_set_device(const char *cmd_id, const char *device, int state);

/**
 * @brief Callback invocation set devices command
 *
 * @param[in] cmd_id Command ID
 * @param[in] fan Fan state
 * @param[in] light Light state
 * @param[in] ac AC state
 */
void mqtt_callback_invoke_set_devices(const char *cmd_id, int fan, int light, int ac);

/**
 * @brief Callback invocation set mode command
 *
 * @param[in] cmd_id Command ID
 * @param[in] mode Mode value
 */
void mqtt_callback_invoke_set_mode(const char *cmd_id, int mode);

/**
 * @brief Callback invocation set interval command
 *
 * @param[in] cmd_id Command ID
 * @param[in] interval Interval value
 */
void mqtt_callback_invoke_set_interval(const char *cmd_id, int interval);

/**
 * @brief Callback invocation set timestamp command
 *
 * @param[in] cmd_id Command ID
 * @param[in] timestamp Unix timestamp value
 */
void mqtt_callback_invoke_set_timestamp(const char *cmd_id, uint32_t timestamp);

/**
 * @brief Callback invocation get status command
 *
 * @param[in] cmd_id Command ID
 */
void mqtt_callback_invoke_get_status(const char *cmd_id);

/**
 * @brief Callback invocation ping command
 *
 * @param[in] cmd_id Command ID
 */
void mqtt_callback_invoke_ping(const char *cmd_id);

/**
 * @brief Callback invocation reboot command
 *
 * @param[in] cmd_id Command ID
 */
void mqtt_callback_invoke_reboot(const char *cmd_id);

/**
 * @brief Callback invocation factory reset command
 *
 * @param[in] cmd_id Command ID
 */
void mqtt_callback_invoke_factory_reset(const char *cmd_id);

#endif /* MQTT_CALLBACK_H */
