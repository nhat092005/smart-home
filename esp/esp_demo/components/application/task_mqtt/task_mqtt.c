/**
 * @file task_mqtt.c
 *
 * @brief MQTT Task Implementation
 */

/* Includes ------------------------------------------------------------------*/
#include "task_mqtt.h"
#include "task_manager.h"
#include "mqtt_manager.h"
#include "mqtt_callback.h"
#include "json_helper.h"
#include "shared_sensor.h"
#include "device_control.h"
#include "mode_manager.h"
#include "sensor_manager.h"
#include "wifi_manager.h"

#include "esp_wifi.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "cJSON.h"

#include <string.h>
#include <time.h>

/* Extern variables ----------------------------------------------------------*/

extern bool isMQTT;

/* Private types -------------------------------------------------------------*/

/**
 * @brief System state structure holding all device states
 */
typedef struct
{
    int mode;         //!< Current mode (0=OFF, 1=ON)
    int interval_sec; //!< Data publish interval in seconds
    int fan;          //!< Fan state (0=OFF, 1=ON)
    int light;        //!< Light state (0=OFF, 1=ON)
    int ac;           //!< AC state (0=OFF, 1=ON)
} system_state_t;

/**
 * @brief Device registry entry for extensibility
 */
typedef struct
{
    const char *name; //!< Device name from JSON
    int *state_ptr;   //!< Pointer to state variable
} device_registry_entry_t;

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "TASK_MQTT";

static system_state_t device_state = {
    .mode = 0,
    .interval_sec = INTERVAL_TIME_MS / 1000,
    .fan = 0,
    .light = 0,
    .ac = 0};

static SemaphoreHandle_t state_mutex = NULL;
static volatile bool interval_changed = false;

// Device registry for extensible device handling
static device_registry_entry_t device_registry[] = {
    {"fan", &device_state.fan},     //!< Fan device
    {"light", &device_state.light}, //!< Light device
    {"ac", &device_state.ac},       //!< AC device
    {NULL, NULL}                    //!< End marker
};

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief MQTT task periodic publishing with proper timing
 */
static void task_mqtt_run(void *pvParameters);

/**
 * @brief Find device state by name using registry
 *
 * @param[in] device_name Name of the device
 *
 * @return Pointer to state variable, or NULL if not found
 */
static int *task_mqtt_find_device_state(const char *device_name);

/**
 * @brief Get current timestamp from DS3231 RTC
 */
static uint32_t task_mqtt_get_timestamp(void);

/**
 * @brief Publish sensor data
 */
static void task_mqtt_publish_sensor_data(void);

/**
 * @brief Sync device states from hardware to MQTT state
 */
static void task_mqtt_sync_device_states(void);

/**
 * @brief Publish device info
 */
static void task_mqtt_publish_info_data(void);

/**
 * @brief Delayed reboot task to avoid blocking MQTT handler
 *
 * @param[in] pvParameters Command ID string
 */
static void task_mqtt_delayed_reboot_task(void *pvParameters);

/**
 * @brief Delayed factory reset task
 *
 * @param[in] pvParameters Command ID string
 */
static void task_mqtt_delayed_factory_reset_task(void *pvParameters);

/**
 * @brief Get current connected SSID
 */
static const char *task_mqtt_get_current_ssid(void);

/**
 * @brief Get current connected IP address
 */
static const char *task_mqtt_get_current_ip(void);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Callback when MQTT connects to broker
 */
void task_mqtt_on_connected(void)
{
    ESP_LOGI(TAG, "MQTT Connected");

    // Publish info on connection (per spec: Boot + network change)
    task_mqtt_publish_info_data();
}

/**
 * @brief Callback when MQTT disconnects from broker
 */
void task_mqtt_on_disconnected(void)
{
    ESP_LOGW(TAG, "MQTT Disconnected");
}

/**
 * @brief Callback to provide sensor data before publishing /data
 */
void task_mqtt_on_data_publish(uint32_t timestamp, float *temp, float *hum, int *light)
{
    // Read from shared sensor data (updated by task_mode)
    shared_sensor_data_t data;

    esp_err_t ret = shared_sensor_data_get(&data);

    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to get shared sensor data, using defaults");
        *temp = 0.0f;
        *hum = 0.0f;
        *light = 0;
        return;
    }

    // Provide data to MQTT publish
    *temp = data.temperature;
    *hum = data.humidity;
    *light = data.light;

    ESP_LOGI(TAG, "MQTT Data: T=%.2f°C H=%.2f%% L=%d lux", data.temperature, data.humidity, data.light);
}

/**
 * @brief Callback to sync state before publishing /state
 */
void task_mqtt_on_state_publish(uint32_t timestamp, int *mode, int *fan, int *light, int *ac)
{
    // State is already synced and copied from device_state
    ESP_LOGD(TAG, "State: mode=%d fan=%d light=%d ac=%d", *mode, *fan, *light, *ac);
}

/* Command Callbacks ---------------------------------------------------------*/

/**
 * @brief Handle set_device command
 */
void task_mqtt_on_set_device(const char *cmd_id, const char *device, int state)
{
    ESP_LOGI(TAG, "[%s] set_device: %s to %s", cmd_id, device, state ? "ON" : "OFF");

    esp_err_t result = ESP_OK;

    // Update internal state using registry
    int *state_ptr = task_mqtt_find_device_state(device);
    if (state_ptr != NULL)
    {
        if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE)
        {
            *state_ptr = state;
            xSemaphoreGive(state_mutex);
        }
    }

    // Control the actual hardware
    device_state_t dev_state = state ? DEVICE_ON : DEVICE_OFF;

    if (strcmp(device, "fan") == 0)
    {
        result = device_control_set_state(DEVICE_FAN, dev_state);
    }
    else if (strcmp(device, "light") == 0)
    {
        result = device_control_set_state(DEVICE_LIGHT, dev_state);
    }
    else if (strcmp(device, "ac") == 0)
    {
        result = device_control_set_state(DEVICE_AC, dev_state);
    }
    else
    {
        ESP_LOGW(TAG, "[%s] Unknown device: %s", cmd_id, device);
        result = ESP_ERR_INVALID_ARG;
    }

    // Publish response
    mqtt_manager_publish_response(cmd_id, (result == ESP_OK) ? "success" : "error");

    // Publish updated state
    task_mqtt_publish_current_state();
}

/**
 * @brief Handle set_devices command
 */
void task_mqtt_on_set_devices(const char *cmd_id, int fan, int light, int ac)
{
    ESP_LOGI(TAG, "[%s] set_devices: fan=%d light=%d ac=%d", cmd_id, fan, light, ac);

    esp_err_t result = ESP_OK;
    esp_err_t ret;

    // Update internal state
    if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE)
    {
        if (fan >= 0)
            device_state.fan = fan;
        if (light >= 0)
            device_state.light = light;
        if (ac >= 0)
            device_state.ac = ac;
        xSemaphoreGive(state_mutex);
    }

    // Control hardware
    if (fan >= 0)
    {
        ret = device_control_set_state(DEVICE_FAN, fan ? DEVICE_ON : DEVICE_OFF);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to set fan state: %s", esp_err_to_name(ret));
            result = ret;
        }
    }

    if (light >= 0)
    {
        ret = device_control_set_state(DEVICE_LIGHT, light ? DEVICE_ON : DEVICE_OFF);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to set light state: %s", esp_err_to_name(ret));
            result = ret;
        }
    }

    if (ac >= 0)
    {
        ret = device_control_set_state(DEVICE_AC, ac ? DEVICE_ON : DEVICE_OFF);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to set AC state: %s", esp_err_to_name(ret));
            result = ret;
        }
    }

    // Publish response
    mqtt_manager_publish_response(cmd_id, (result == ESP_OK) ? "success" : "error");

    // Publish updated state
    task_mqtt_publish_current_state();
}

/**
 * @brief Handle set_mode command
 */
void task_mqtt_on_set_mode(const char *cmd_id, int mode)
{
    ESP_LOGI(TAG, "[%s] set_mode: %d", cmd_id, mode);

    // Update internal state
    if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE)
    {
        device_state.mode = mode;
        xSemaphoreGive(state_mutex);
    }

    // Update mode manager
    mode_manager_set_mode(mode);

    // Publish response
    mqtt_manager_publish_response(cmd_id, "success");

    // Publish updated state
    task_mqtt_publish_current_state();
}

/**
 * @brief Handle set_interval command
 */
void task_mqtt_on_set_interval(const char *cmd_id, int interval)
{
    ESP_LOGI(TAG, "[%s] set_interval: %d seconds", cmd_id, interval);

    if (interval >= MIN_INTERVAL && interval <= MAX_INTERVAL)
    {
        // Thread-safe write - update both local and global interval
        if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE)
        {
            device_state.interval_sec = interval;
            g_interval_time_ms = interval * 1000; // Sync to global variable
            interval_changed = true;
            xSemaphoreGive(state_mutex);
        }

        ESP_LOGI(TAG, "Data interval: %d seconds (synced to g_interval_time_ms: %lu ms)", interval, (unsigned long)g_interval_time_ms);

        // Publish response - success
        mqtt_manager_publish_response(cmd_id, "success");

        // Publish updated state
        task_mqtt_publish_current_state();
    }
    else
    {
        ESP_LOGW(TAG, "Invalid interval: %d (must be %d-%d)", interval, MIN_INTERVAL, MAX_INTERVAL);

        // Publish response - error
        mqtt_manager_publish_response(cmd_id, "error");
    }
}

/**
 * @brief Handle set_timestamp command
 */
void task_mqtt_on_set_timestamp(const char *cmd_id, uint32_t timestamp)
{
    ESP_LOGI(TAG, "[%s] set_timestamp: %lu", cmd_id, (unsigned long)timestamp);

    esp_err_t ret = sensor_manager_set_timestamp(timestamp);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Timestamp updated successfully");
        mqtt_manager_publish_response(cmd_id, "success");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to set timestamp: %s", esp_err_to_name(ret));
        mqtt_manager_publish_response(cmd_id, "error");
    }
}

/**
 * @brief Handle get_status command
 */
void task_mqtt_on_get_status(const char *cmd_id)
{
    ESP_LOGI(TAG, "[%s] get_status publishing all topics", cmd_id);

    // Publish response first
    mqtt_manager_publish_response(cmd_id, "success");

    // Publish all topics
    task_mqtt_publish_sensor_data();
    task_mqtt_publish_current_state();
    task_mqtt_publish_info_data();
}

/**
 * @brief Handle ping command
 */
void task_mqtt_on_ping(const char *cmd_id)
{
    ESP_LOGI(TAG, "[%s] ping received", cmd_id);

    // Just respond with success - simple connectivity check
    mqtt_manager_publish_response(cmd_id, "success");
}

/**
 * @brief Handle reboot command
 */
void task_mqtt_on_reboot(const char *cmd_id)
{
    ESP_LOGW(TAG, "[%s] Reboot requested", cmd_id);

    // Publish response before reboot
    mqtt_manager_publish_response(cmd_id, "success");

    // Start delayed reboot task
    xTaskCreate(task_mqtt_delayed_reboot_task, "reboot_task", 2048, (void *)cmd_id, 1, NULL);
}

/**
 * @brief Handle factory_reset command
 */
void task_mqtt_on_factory_reset(const char *cmd_id)
{
    ESP_LOGW(TAG, "[%s] Factory reset requested", cmd_id);

    // Publish response before factory reset
    mqtt_manager_publish_response(cmd_id, "success");

    // Start delayed factory reset task
    xTaskCreate(task_mqtt_delayed_factory_reset_task, "factory_reset_task", 2048, (void *)cmd_id, 1, NULL);
}

/**
 * @brief Initialize MQTT task and register callbacks
 */
esp_err_t task_mqtt_init(void)
{
    // Initialize MQTT Callback Manager first
    mqtt_callback_init();

    // Register all callbacks with mqtt_callback
    mqtt_callback_register_on_connected(task_mqtt_on_connected);
    mqtt_callback_register_on_disconnected(task_mqtt_on_disconnected);
    mqtt_callback_register_on_data_publish(task_mqtt_on_data_publish);
    mqtt_callback_register_on_state_publish(task_mqtt_on_state_publish);
    mqtt_callback_register_on_set_device(task_mqtt_on_set_device);
    mqtt_callback_register_on_set_devices(task_mqtt_on_set_devices);
    mqtt_callback_register_on_set_mode(task_mqtt_on_set_mode);
    mqtt_callback_register_on_set_interval(task_mqtt_on_set_interval);
    mqtt_callback_register_on_set_timestamp(task_mqtt_on_set_timestamp);
    mqtt_callback_register_on_get_status(task_mqtt_on_get_status);
    mqtt_callback_register_on_ping(task_mqtt_on_ping);
    mqtt_callback_register_on_reboot(task_mqtt_on_reboot);
    mqtt_callback_register_on_factory_reset(task_mqtt_on_factory_reset);

    // Create mutex for thread-safe device state access
    state_mutex = xSemaphoreCreateMutex();
    if (state_mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create state mutex");
        return ESP_FAIL;
    }

    // Create MQTT task
    BaseType_t ret = xTaskCreate(
        task_mqtt_run,
        "task_mqtt",
        4096,
        NULL,
        5,
        NULL);

    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create Task MQTT");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Task MQTT initialized");
    return ESP_OK;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Get current timestamp from DS3231 RTC
 */
static uint32_t task_mqtt_get_timestamp(void)
{
    uint32_t timestamp = 0;

    esp_err_t ret = sensor_manager_get_timestamp(&timestamp);
    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to get timestamp from DS3231 RTC");
        timestamp = 0;
    }

    return timestamp;
}

/**
 * @brief Find device state by name using registry
 */
static int *task_mqtt_find_device_state(const char *device_name)
{
    for (int i = 0; device_registry[i].name != NULL; i++)
    {
        if (strcmp(device_registry[i].name, device_name) == 0)
        {
            return device_registry[i].state_ptr;
        }
    }
    return NULL;
}

/**
 * @brief Publish sensor data
 */
static void task_mqtt_publish_sensor_data(void)
{
    uint32_t timestamp = task_mqtt_get_timestamp();

    float temp = 0.0f, hum = 0.0f;
    int light = 0;

    // Invoke data_publish callback to get sensor values
    if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE)
    {
        mqtt_callback_invoke_data_publish(timestamp, &temp, &hum, &light);
        xSemaphoreGive(state_mutex);
    }

    mqtt_manager_publish_data(timestamp, temp, hum, light);
}

/**
 * @brief Publish current device state
 */
void task_mqtt_publish_current_state(void)
{
    // Check MQTT connection first to avoid unnecessary work
    if (!mqtt_manager_is_connected())
    {
        ESP_LOGD(TAG, "MQTT not connected, skipping state publish");
        return;
    }

    // Sync device states from hardware BEFORE taking mutex
    task_mqtt_sync_device_states();

    uint32_t timestamp = task_mqtt_get_timestamp();

    int mode = 0, fan = 0, light = 0, ac = 0, interval = 0;

    // Use timeout instead of portMAX_DELAY
    if (xSemaphoreTake(state_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        mode = device_state.mode;
        interval = device_state.interval_sec;
        fan = device_state.fan;
        light = device_state.light;
        ac = device_state.ac;

        // Allow callback to modify before publish
        mqtt_callback_invoke_state_publish(timestamp, &mode, &fan, &light, &ac);

        xSemaphoreGive(state_mutex);

        // Publish after releasing mutex
        mqtt_manager_publish_state(timestamp, mode, interval, fan, light, ac);
    }
    else
    {
        ESP_LOGW(TAG, "Cannot take state_mutex for publishing, skipping");
    }
}

/**
 * @brief Sync device states from hardware to MQTT state
 */
static void task_mqtt_sync_device_states(void)
{
    device_state_t fan_state, light_state, ac_state;

    // Read current hardware states with timeout to prevent blocking
    if (device_control_get_state(DEVICE_FAN, &fan_state) != ESP_OK ||
        device_control_get_state(DEVICE_LIGHT, &light_state) != ESP_OK ||
        device_control_get_state(DEVICE_AC, &ac_state) != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to read device states, skipping sync");
        return;
    }

    // Update internal state with timeout (100ms max)
    if (xSemaphoreTake(state_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        // Sync mode from mode_manager
        device_state.mode = (mode_manager_get_mode() == MODE_ON) ? 1 : 0;

        // Sync device states from hardware
        device_state.fan = (fan_state == DEVICE_ON) ? 1 : 0;
        device_state.light = (light_state == DEVICE_ON) ? 1 : 0;
        device_state.ac = (ac_state == DEVICE_ON) ? 1 : 0;
        xSemaphoreGive(state_mutex);
    }
    else
    {
        ESP_LOGW(TAG, "Cannot take state_mutex, skipping sync");
    }
}

/**
 * @brief Publish device info
 */
static void task_mqtt_publish_info_data(void)
{
    uint32_t timestamp = task_mqtt_get_timestamp();

    mqtt_manager_publish_info(timestamp,
                              MQTT_DEVICE_ID,               //!< Device ID
                              task_mqtt_get_current_ssid(), //!< SSID
                              task_mqtt_get_current_ip(),   //!< IP
                              MQTT_BROKER_URI,              //!< MQTT URI
                              g_app_version);               //!< Firmware version
}

/**
 * @brief Delayed reboot task to avoid blocking MQTT handler
 */
static void task_mqtt_delayed_reboot_task(void *pvParameters)
{

    ESP_LOGW(TAG, "Reboot in 1 seconds...");
    vTaskDelay(pdMS_TO_TICKS(1000));

    esp_restart();

    vTaskDelete(NULL);
}

/**
 * @brief Delayed factory reset task
 */
static void task_mqtt_delayed_factory_reset_task(void *pvParameters)
{

    ESP_LOGW(TAG, "Factory reset in 1 seconds...");
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Erase NVS flash
    nvs_flash_erase();

    esp_restart();

    vTaskDelete(NULL);
}

/**
 * @brief MQTT task - periodic publishing with proper timing
 */
static void task_mqtt_run(void *pvParameters)
{
    ESP_LOGI(TAG, "MQTT task started");

    // Proper tick timing without overflow issues
    TickType_t last_data_publish = xTaskGetTickCount();
    TickType_t last_state_publish = xTaskGetTickCount();

    // Initialize from global interval
    uint32_t current_interval_ms = g_interval_time_ms;

    while (1)
    {
        if (mqtt_manager_is_connected())
        {
            TickType_t now = xTaskGetTickCount();

            // Thread-safe read of interval (use global variable)
            current_interval_ms = g_interval_time_ms;

            // If interval changed, reset timer immediately
            if (interval_changed)
            {
                interval_changed = false;
                last_data_publish = now;
                ESP_LOGI(TAG, "Interval timer reset - next publish in %lu seconds",
                         (unsigned long)(current_interval_ms / 1000));
            }

            // Publish sensor data only when MODE is ON (LED is on)
            TickType_t data_elapsed = now - last_data_publish;
            if (data_elapsed >= pdMS_TO_TICKS(current_interval_ms))
            {
                if (isModeON)
                {
                    task_mqtt_publish_sensor_data();
                }
                else
                {
                    ESP_LOGD(TAG, "Skipping sensor data publish - Mode is OFF");
                }
                last_data_publish = now;
            }

            // Publish state backup every 60 seconds
            TickType_t state_elapsed = now - last_state_publish;
            if (state_elapsed >= pdMS_TO_TICKS(STATE_BACKUP_INTERVAL * 1000))
            {
                task_mqtt_publish_current_state();
                last_state_publish = now;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Get current SSID as string
 */
static const char *task_mqtt_get_current_ssid(void)
{
    static char ssid[33] = "Unknown";

    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK)
    {
        memcpy(ssid, ap_info.ssid, sizeof(ap_info.ssid));
        ssid[32] = '\0';
    }

    return ssid;
}

/**
 * @brief Get current IP address as string
 */
static const char *task_mqtt_get_current_ip(void)
{
    static char ip_str[16] = "0.0.0.0";

    esp_netif_ip_info_t ip_info;
    if (wifi_manager_get_ip_info(&ip_info) == ESP_OK)
    {
        snprintf(ip_str, sizeof(ip_str), IPSTR, IP2STR(&ip_info.ip));
    }

    return ip_str;
}