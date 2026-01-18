/**
 * @file task_button.c
 *
 * @brief Task Button Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "task_button.h"
#include "task_manager.h"
#include "button_handler.h"
#include "device_control.h"
#include "mode_manager.h"
#include "wifi_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

/* PRIVATE VARIABLES --------------------------------------------------------*/

static const char *TAG = "TASK_BUTTON";

// Button event queue to defer processing
static QueueHandle_t button_event_queue = NULL;
static TaskHandle_t button_task_handle = NULL;

/* PRIVATE FUNCTIONS --------------------------------------------------------*/

/**
 * @brief Button processing task - handles button events from queue
 */
static void button_processing_task(void *arg)
{
    button_type_t button_event;

    ESP_LOGI(TAG, "Button processing task started");

    while (1)
    {
        // Wait for button events from queue
        if (xQueueReceive(button_event_queue, &button_event, portMAX_DELAY) == pdTRUE)
        {
            // Process button event based on type
            switch (button_event)
            {
            case BUTTON_WIFI:
                ESP_LOGW(TAG, "WiFi credentials clear button pressed");
                wifi_manager_clear_credentials();
                ESP_LOGI(TAG, "Restarting to provisioning mode...");
                vTaskDelay(pdMS_TO_TICKS(1000));
                esp_restart();
                break;

            case BUTTON_MODE:
                ESP_LOGI(TAG, "Device button pressed");
                mode_manager_toggle_mode();
                task_mqtt_publish_current_state();
                break;

            case BUTTON_LIGHT:
                ESP_LOGI(TAG, "Light button pressed");
                device_control_toggle(DEVICE_LIGHT);
                task_mqtt_publish_current_state();
                break;

            case BUTTON_FAN:
                ESP_LOGI(TAG, "Fan button pressed");
                device_control_toggle(DEVICE_FAN);
                task_mqtt_publish_current_state();
                break;

            case BUTTON_AC:
                ESP_LOGI(TAG, "AC button pressed");
                device_control_toggle(DEVICE_AC);
                task_mqtt_publish_current_state();
                break;

            default:
                ESP_LOGW(TAG, "Unknown button event: %d", button_event);
                break;
            }
        }
    }
}

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize button processing task and queue
 */
esp_err_t task_button_init(void)
{
    // Create queue for button events (10 events max)
    button_event_queue = xQueueCreate(10, sizeof(button_type_t));
    if (button_event_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create button event queue");
        return ESP_FAIL;
    }

    // Create button processing task with higher priority and larger stack
    BaseType_t result = xTaskCreate(
        button_processing_task,
        "button_proc",
        4096, // Stack size - increased for MQTT operations
        NULL,
        6, // Priority - higher than button polling task
        &button_task_handle);

    if (result != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create button processing task");
        vQueueDelete(button_event_queue);
        button_event_queue = NULL;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Button processing system initialized");
    return ESP_OK;
}

/**
 * @brief Button interval callback implementations - just queues event
 */
void task_button_wifi_pressed(button_type_t button)
{
    // Queue button event for deferred processing
    if (button_event_queue != NULL)
    {
        xQueueSend(button_event_queue, &button, 0);
    }
}

/**
 * @brief Button device callback implementations - just queues event
 */
void task_button_mode_pressed(button_type_t button)
{
    // Queue button event for deferred processing
    if (button_event_queue != NULL)
    {
        xQueueSend(button_event_queue, &button, 0);
    }
}

/**
 * @brief Button light callback implementations - just queues event
 */
void task_button_light_pressed(button_type_t button)
{
    // Queue button event for deferred processing
    if (button_event_queue != NULL)
    {
        xQueueSend(button_event_queue, &button, 0);
    }
}

/**
 * @brief Button fan callback implementations - just queues event
 */
void task_button_fan_pressed(button_type_t button)
{
    // Queue button event for deferred processing
    if (button_event_queue != NULL)
    {
        xQueueSend(button_event_queue, &button, 0);
    }
}

/**
 * @brief Button ac callback implementations - just queues event
 */
void task_button_ac_pressed(button_type_t button)
{
    // Queue button event for deferred processing
    if (button_event_queue != NULL)
    {
        xQueueSend(button_event_queue, &button, 0);
    }
}
