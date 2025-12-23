/**
 * @file mqtt_callback.c
 *
 * @brief MQTT Callback Manager Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "mqtt_callback.h"
#include "mqtt_manager.h"
#include "json_helper.h"
#include "esp_log.h"
#include "cJSON.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "MQTT_CALLBACK";

// Event callback storage
static mqtt_event_connected_cb_t on_connected_cb = NULL;
static mqtt_event_disconnected_cb_t on_disconnected_cb = NULL;
static mqtt_event_data_publish_cb_t on_data_publish_cb = NULL;
static mqtt_event_state_publish_cb_t on_state_publish_cb = NULL;

// Command callback storage
static mqtt_cmd_set_device_cb_t on_set_device_cb = NULL;
static mqtt_cmd_set_devices_cb_t on_set_devices_cb = NULL;
static mqtt_cmd_set_mode_cb_t on_set_mode_cb = NULL;
static mqtt_cmd_set_interval_cb_t on_set_interval_cb = NULL;
static mqtt_cmd_set_timestamp_cb_t on_set_timestamp_cb = NULL;
static mqtt_cmd_get_status_cb_t on_get_status_cb = NULL;
static mqtt_cmd_reboot_cb_t on_reboot_cb = NULL;
static mqtt_cmd_factory_reset_cb_t on_factory_reset_cb = NULL;

/* External functions --------------------------------------------------------*/

/* Forward declarations ------------------------------------------------------*/

/**
 * @brief Internal handler for MQTT commands
 *
 * @param[in] cmd_id Command ID
 * @param[in] command Command name
 * @param[in] params Command parameters as cJSON object
 */
static void mqtt_callback_internal_command_handler(const char *cmd_id, const char *command, cJSON *params);

/**
 * @brief Internal handler for MQTT connected event
 */
static void mqtt_callback_internal_connected_handler(void);

/**
 * @brief Internal handler for MQTT disconnected event
 */
static void mqtt_callback_internal_disconnected_handler(void);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize MQTT Callback Manager
 */
void mqtt_callback_init(void)
{
    ESP_LOGI(TAG, "Initializing MQTT Callback Manager");

    // Register internal handlers with MQTT manager
    mqtt_manager_register_connected_callback(mqtt_callback_internal_connected_handler);
    mqtt_manager_register_disconnected_callback(mqtt_callback_internal_disconnected_handler);
    mqtt_manager_register_command_callback(mqtt_callback_internal_command_handler);

    ESP_LOGI(TAG, "MQTT Callback Manager initialized");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_connected(mqtt_event_connected_cb_t callback)
{
    on_connected_cb = callback;
    ESP_LOGI(TAG, "Registered: on_connected");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_disconnected(mqtt_event_disconnected_cb_t callback)
{
    on_disconnected_cb = callback;
    ESP_LOGI(TAG, "Registered: on_disconnected");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_data_publish(mqtt_event_data_publish_cb_t callback)
{
    on_data_publish_cb = callback;
    ESP_LOGI(TAG, "Registered: on_data_publish");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_state_publish(mqtt_event_state_publish_cb_t callback)
{
    on_state_publish_cb = callback;
    ESP_LOGI(TAG, "Registered: on_state_publish");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_set_device(mqtt_cmd_set_device_cb_t callback)
{
    on_set_device_cb = callback;
    ESP_LOGI(TAG, "Registered: on_set_device");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_set_devices(mqtt_cmd_set_devices_cb_t callback)
{
    on_set_devices_cb = callback;
    ESP_LOGI(TAG, "Registered: on_set_devices");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_set_mode(mqtt_cmd_set_mode_cb_t callback)
{
    on_set_mode_cb = callback;
    ESP_LOGI(TAG, "Registered: on_set_mode");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_set_interval(mqtt_cmd_set_interval_cb_t callback)
{
    on_set_interval_cb = callback;
    ESP_LOGI(TAG, "Registered: on_set_interval");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_set_timestamp(mqtt_cmd_set_timestamp_cb_t callback)
{
    on_set_timestamp_cb = callback;
    ESP_LOGI(TAG, "Registered: on_set_timestamp");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_get_status(mqtt_cmd_get_status_cb_t callback)
{
    on_get_status_cb = callback;
    ESP_LOGI(TAG, "Registered: on_get_status");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_reboot(mqtt_cmd_reboot_cb_t callback)
{
    on_reboot_cb = callback;
    ESP_LOGI(TAG, "Registered: on_reboot");
}

/**
 * @brief Callback registration API
 */
void mqtt_callback_register_on_factory_reset(mqtt_cmd_factory_reset_cb_t callback)
{
    on_factory_reset_cb = callback;
    ESP_LOGI(TAG, "Registered: on_factory_reset");
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_connected(void)
{
    if (on_connected_cb)
    {
        on_connected_cb();
    }
    else
    {
        ESP_LOGD(TAG, "No callback registered for: connected");
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_disconnected(void)
{
    if (on_disconnected_cb)
    {
        on_disconnected_cb();
    }
    else
    {
        ESP_LOGD(TAG, "No callback registered for: disconnected");
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_data_publish(uint32_t timestamp, float *temp, float *hum, int *light)
{
    if (on_data_publish_cb)
    {
        on_data_publish_cb(timestamp, temp, hum, light);
    }
    else
    {
        ESP_LOGW(TAG, "No callback registered for: data_publish");
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_state_publish(uint32_t timestamp, int *mode, int *fan, int *light, int *ac)
{
    if (on_state_publish_cb)
    {
        on_state_publish_cb(timestamp, mode, fan, light, ac);
    }
    else
    {
        ESP_LOGD(TAG, "No callback registered for: state_publish");
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_set_device(const char *cmd_id, const char *device, int state)
{
    if (on_set_device_cb)
    {
        on_set_device_cb(cmd_id, device, state);
    }
    else
    {
        ESP_LOGW(TAG, "[%s] No callback for: set_device", cmd_id);
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_set_devices(const char *cmd_id, int fan, int light, int ac)
{
    if (on_set_devices_cb)
    {
        on_set_devices_cb(cmd_id, fan, light, ac);
    }
    else
    {
        ESP_LOGW(TAG, "[%s] No callback for: set_devices", cmd_id);
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_set_mode(const char *cmd_id, int mode)
{
    if (on_set_mode_cb)
    {
        on_set_mode_cb(cmd_id, mode);
    }
    else
    {
        ESP_LOGW(TAG, "[%s] No callback for: set_mode", cmd_id);
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_set_interval(const char *cmd_id, int interval)
{
    if (on_set_interval_cb)
    {
        on_set_interval_cb(cmd_id, interval);
    }
    else
    {
        ESP_LOGW(TAG, "[%s] No callback for: set_interval", cmd_id);
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_set_timestamp(const char *cmd_id, uint32_t timestamp)
{
    if (on_set_timestamp_cb)
    {
        on_set_timestamp_cb(cmd_id, timestamp);
    }
    else
    {
        ESP_LOGW(TAG, "[%s] No callback for: set_timestamp", cmd_id);
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_get_status(const char *cmd_id)
{
    if (on_get_status_cb)
    {
        on_get_status_cb(cmd_id);
    }
    else
    {
        ESP_LOGW(TAG, "[%s] No callback for: get_status", cmd_id);
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_reboot(const char *cmd_id)
{
    if (on_reboot_cb)
    {
        on_reboot_cb(cmd_id);
    }
    else
    {
        ESP_LOGW(TAG, "[%s] No callback for: reboot", cmd_id);
    }
}

/**
 * @brief Callback invocation APIs
 */
void mqtt_callback_invoke_factory_reset(const char *cmd_id)
{
    if (on_factory_reset_cb)
    {
        on_factory_reset_cb(cmd_id);
    }
    else
    {
        ESP_LOGW(TAG, "[%s] No callback for: factory_reset", cmd_id);
    }
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Internal handler for MQTT connected event
 */
static void mqtt_callback_internal_connected_handler(void)
{
    ESP_LOGI(TAG, "MQTT connected");
    mqtt_callback_invoke_connected();
}

/**
 * @brief Internal handler for MQTT disconnected event
 */
static void mqtt_callback_internal_disconnected_handler(void)
{
    ESP_LOGW(TAG, "MQTT disconnected");
    mqtt_callback_invoke_disconnected();
}

/**
 * @brief Internal handler for MQTT commands
 */
static void mqtt_callback_internal_command_handler(const char *cmd_id, const char *command, cJSON *params)
{
    if (!cmd_id || !command)
    {
        ESP_LOGE(TAG, "Invalid command parameters");
        return;
    }

    ESP_LOGI(TAG, "Processing command: %s (ID: %s)", command, cmd_id);

    /* Command: set_device */
    if (strcmp(command, "set_device") == 0)
    {
        const char *device = json_helper_get_string(params, "device", "");
        int state = json_helper_get_int(params, "state", 0);
        mqtt_callback_invoke_set_device(cmd_id, device, state);
    }
    /* Command: set_devices */
    else if (strcmp(command, "set_devices") == 0)
    {
        int fan = json_helper_get_int(params, "fan", -1);
        int light = json_helper_get_int(params, "light", -1);
        int ac = json_helper_get_int(params, "ac", -1);
        mqtt_callback_invoke_set_devices(cmd_id, fan, light, ac);
    }
    /* Command: set_mode */
    else if (strcmp(command, "set_mode") == 0)
    {
        int mode = json_helper_get_int(params, "mode", 0);
        mqtt_callback_invoke_set_mode(cmd_id, mode);
    }
    /* Command: set_interval */
    else if (strcmp(command, "set_interval") == 0)
    {
        int interval = json_helper_get_int(params, "interval", 0);
        mqtt_callback_invoke_set_interval(cmd_id, interval);
    }
    /* Command: set_timestamp */
    else if (strcmp(command, "set_timestamp") == 0)
    {
        uint32_t timestamp = (uint32_t)json_helper_get_int(params, "timestamp", 0);
        mqtt_callback_invoke_set_timestamp(cmd_id, timestamp);
    }
    /* Command: get_status */
    else if (strcmp(command, "get_status") == 0)
    {
        mqtt_callback_invoke_get_status(cmd_id);
    }
    /* Command: reboot */
    else if (strcmp(command, "reboot") == 0)
    {
        mqtt_callback_invoke_reboot(cmd_id);
    }
    /* Command: factory_reset */
    else if (strcmp(command, "factory_reset") == 0)
    {
        mqtt_callback_invoke_factory_reset(cmd_id);
    }
    /* Unknown Command */
    else
    {
        ESP_LOGW(TAG, "Unknown command: %s (ID: %s)", command, cmd_id);
    }
}
