/**
 * @file task_button.c
 *
 * @brief Task Button Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "esp_log.h"
#include "task_manager.h"

/* PRIVATE VARIABLES --------------------------------------------------------*/

static const char *TAG = "TASK_BUTTON";

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Button interval callback implementations
 */
void task_button_wifi_pressed(button_type_t button)
{
    ESP_LOGW(TAG, "WiFi credentials clear button pressed");
    wifi_manager_clear_credentials();
    ESP_LOGI(TAG, "Restarting to provisioning mode...");
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
}

/**
 * @brief Button device callback implementations
 */
void task_button_mode_pressed(button_type_t button)
{
    ESP_LOGI(TAG, "Device button pressed");
    mode_manager_toggle_mode();
}

/**
 * @brief Button light callback implementations
 */
void task_button_light_pressed(button_type_t button)
{
    ESP_LOGI(TAG, "Light button pressed");
    device_control_toggle(DEVICE_LIGHT);
}

/**
 * @brief Button fan callback implementations
 */
void task_button_fan_pressed(button_type_t button)
{
    ESP_LOGI(TAG, "Fan button pressed");
    device_control_toggle(DEVICE_FAN);
}
