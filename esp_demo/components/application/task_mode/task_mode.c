/**
 * @file task_mode.c
 *
 * @brief Task Mode Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "task_mode.h"
#include "task_manager.h"
#include "sensor_manager.h"
#include "sensor_reader.h"
#include "mode_manager.h"
#include "shared_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <time.h>

/* Private defines -----------------------------------------------------------*/

#define DISPLAY_UPDATE_INTERVAL_MS 1000 //!< Update display every second

/* External variables --------------------------------------------------------*/

extern char g_app_version[16];      //!< Application version from task_manager.c
extern uint32_t g_interval_time_ms; //!< Shared interval from task_manager.h

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "TASK_MODE";

static volatile bool display_task_running = false;
static TaskHandle_t display_task_handle = NULL;

/* Private function prototypes -----------------------------------------------*/

static void display_update_task(void *pvParameters);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize display update task
 */
esp_err_t task_mode_init(void)
{
    ESP_LOGI(TAG, "Initializing display management task");

    display_task_running = true;

    // Create display update task with larger stack
    BaseType_t ret = xTaskCreate(
        display_update_task,
        "display_task",
        6144, // Increased stack size (was 4096)
        NULL,
        4, // Lower priority to not block other tasks
        &display_task_handle);

    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create display task");
        display_task_running = false;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Display management task initialized successfully");
    return ESP_OK;
}

/**
 * @brief Mode change event callback
 */
void task_mode_change_event_callback(device_mode_t old_mode, device_mode_t new_mode)
{
    ESP_LOGI(TAG, "Mode changed: %s -> %s",
             old_mode == MODE_ON ? "ON" : "OFF",
             new_mode == MODE_ON ? "ON" : "OFF");

    if (new_mode == MODE_ON)
    {
        ESP_LOGI(TAG, "Display: Full UI with sensors");
    }
    else
    {
        ESP_LOGI(TAG, "Display: Time-only mode");
    }
}

/**
 * @brief Stop display task (cleanup)
 */
void task_mode_stop(void)
{
    if (display_task_running)
    {
        ESP_LOGI(TAG, "Stopping display task");
        display_task_running = false;

        // Wait for task to finish
        vTaskDelay(pdMS_TO_TICKS(100));
        display_task_handle = NULL;
    }
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Display update task
 */
static void display_update_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Display update task started");

    display_data_t display_data = {
        .version = g_app_version,
        .interval = g_interval_time_ms / 1000};

    TickType_t last_wake_time = xTaskGetTickCount();
    // Initialize to trigger immediate sensor read on first iteration
    TickType_t last_sensor_read = xTaskGetTickCount() - pdMS_TO_TICKS(g_interval_time_ms);

    while (display_task_running)
    {
        TickType_t now = xTaskGetTickCount();

        // Update interval from global variable (may be changed by MQTT)
        display_data.interval = g_interval_time_ms / 1000;

        // Read time from sensor_manager (DS3231) - every second
        struct tm time_data;
        uint32_t timestamp = 0;

        if (sensor_manager_get_timestamp(&timestamp) == ESP_OK)
        {
            time_t raw_time = (time_t)timestamp;
            localtime_r(&raw_time, &time_data);

            display_data.hour = time_data.tm_hour;
            display_data.minute = time_data.tm_min;
            display_data.second = time_data.tm_sec;
        }

        // Get current mode
        device_mode_t current_mode = mode_manager_get_mode();

        if (current_mode == MODE_ON)
        {
            // Read sensors at intervals and update shared data
            if ((now - last_sensor_read) >= pdMS_TO_TICKS(g_interval_time_ms))
            {
                sensor_data_t sensor_data;

                if (sensor_reader_read_all(&sensor_data) == ESP_OK)
                {
                    // Update shared sensor data (single source of truth)
                    shared_sensor_data_update(
                        sensor_data.temperature,
                        sensor_data.humidity,
                        sensor_data.light,
                        timestamp);
                    ESP_LOGI(TAG, "Sensor updated: T=%.2f H=%.2f L=%d",
                             sensor_data.temperature, sensor_data.humidity, sensor_data.light);
                }
                last_sensor_read = now;
            }

            // Get data from shared store for display
            shared_sensor_data_t shared_data;
            if (shared_sensor_data_get(&shared_data) == ESP_OK)
            {
                display_data.temperature = shared_data.temperature;
                display_data.humidity = shared_data.humidity;
                display_data.light = (float)shared_data.light;
                task_display_render_full_ui(&display_data);
            }
            else
            {
                // No valid sensor data yet, show time only
                task_display_update_time(display_data.hour,
                                         display_data.minute,
                                         display_data.second);
            }
        }
        else
        {
            // MODE_OFF: Only update time
            task_display_update_time(display_data.hour,
                                     display_data.minute,
                                     display_data.second);
        }

        // Update display every second
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(DISPLAY_UPDATE_INTERVAL_MS));
    }

    ESP_LOGI(TAG, "Display task stopped");
    vTaskDelete(NULL);
}