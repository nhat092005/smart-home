/**
 * @file led_manager.c
 *
 * @brief LED Manager Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "task_status.h"
#include "status_led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/* Private defines -----------------------------------------------------------*/

#define CHECK_INTERVAL_MS 50

/* External variables --------------------------------------------------------*/

extern bool isModeON; //!< Defined in mode_manager.c
extern bool isWiFi;   //!< Defined in wifi_manager.c
extern bool isMQTT;   //!< Defined in mqtt_manager.c

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "TASK_STATUS";

static TaskHandle_t task_handle = NULL;
static bool running = false;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Task polling LED
 */
static void led_polling_task(void *pvParameters);

/** Exported functions ------------------------------------------------------- */

/**
 * @brief Task polling LED
 */
esp_err_t task_status_set_init(void)
{
    if (running)
    {
        ESP_LOGW(TAG, "Task status already initialized");
        return ESP_OK;
    }

    // Start polling task
    running = true;
    BaseType_t result = xTaskCreate(
        led_polling_task,
        "led_polling",
        2048,
        NULL,
        5,
        &task_handle);

    if (result != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create polling task");
        running = false;
        task_handle = NULL;

        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Task status initialized");
    return ESP_OK;
}

/* Private functions ----------------------------------------------------------*/

/**
 * @brief Task polling LED
 */
static void led_polling_task(void *pvParameters)
{
    // Get current LED states
    led_state_t last_device, last_wifi, last_mqtt;

    status_led_get_state(LED_DEVICE, &last_device);
    status_led_get_state(LED_WIFI, &last_wifi);
    status_led_get_state(LED_MQTT, &last_mqtt);

    while (running)
    {
        // Check LED_DEVICE
        led_state_t current_device = isModeON ? LED_ON : LED_OFF;
        if (last_device != current_device)
        {
            status_led_set_state(LED_DEVICE, current_device);
            ESP_LOGI(TAG, "Mode LED: %s", isModeON ? "ON" : "OFF");
            last_device = current_device;
        }

        // Check LED_WIFI
        led_state_t current_wifi = isWiFi ? LED_ON : LED_OFF;
        if (last_wifi != current_wifi)
        {
            status_led_set_state(LED_WIFI, current_wifi);
            ESP_LOGI(TAG, "WiFi LED: %s", isWiFi ? "ON" : "OFF");
            last_wifi = current_wifi;
        }

        // Check LED_MQTT
        led_state_t current_mqtt = isMQTT ? LED_ON : LED_OFF;
        if (last_mqtt != current_mqtt)
        {
            status_led_set_state(LED_MQTT, current_mqtt);
            ESP_LOGI(TAG, "MQTT LED: %s", isMQTT ? "ON" : "OFF");
            last_mqtt = current_mqtt;
        }

        // Delay before the next check
        vTaskDelay(pdMS_TO_TICKS(CHECK_INTERVAL_MS));
    }

    ESP_LOGI(TAG, "LED polling task stopped");
    vTaskDelete(NULL);
}
