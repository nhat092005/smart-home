/**
 * @file shared_sensor_data.c
 *
 * @brief Shared sensor data Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "shared_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "SHARED_SENSOR";

static shared_sensor_data_t sensor_data = {
    .temperature = 0.0f,
    .humidity = 0.0f,
    .light = 0,
    .timestamp = 0,
    .valid = false};

static SemaphoreHandle_t data_mutex = NULL;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize shared sensor data module
 */
esp_err_t shared_sensor_data_init(void)
{
    if (data_mutex != NULL)
    {
        ESP_LOGW(TAG, "Shared sensor data already initialized");
        return ESP_OK;
    }

    data_mutex = xSemaphoreCreateMutex();
    if (data_mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create mutex");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Shared sensor data initialized");
    return ESP_OK;
}

/**
 * @brief Update shared sensor data (thread-safe)
 */
esp_err_t shared_sensor_data_update(float temperature, float humidity, int light, uint32_t timestamp)
{
    if (data_mutex == NULL)
    {
        ESP_LOGE(TAG, "Module not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        ESP_LOGW(TAG, "Failed to take mutex for update");
        return ESP_ERR_TIMEOUT;
    }

    sensor_data.temperature = temperature;
    sensor_data.humidity = humidity;
    sensor_data.light = light;
    sensor_data.timestamp = timestamp;
    sensor_data.valid = true;

    xSemaphoreGive(data_mutex);

    ESP_LOGD(TAG, "Updated: T=%.2f H=%.2f L=%d", temperature, humidity, light);
    return ESP_OK;
}

/**
 * @brief Get shared sensor data (thread-safe)
 */
esp_err_t shared_sensor_data_get(shared_sensor_data_t *data)
{
    if (data == NULL)
    {
        ESP_LOGE(TAG, "Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }

    if (data_mutex == NULL)
    {
        ESP_LOGE(TAG, "Module not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        ESP_LOGW(TAG, "Failed to take mutex for get");
        return ESP_ERR_TIMEOUT;
    }

    if (!sensor_data.valid)
    {
        xSemaphoreGive(data_mutex);
        return ESP_ERR_INVALID_STATE;
    }

    memcpy(data, &sensor_data, sizeof(shared_sensor_data_t));

    xSemaphoreGive(data_mutex);

    return ESP_OK;
}

/**
 * @brief Check if shared sensor data is valid
 */
bool shared_sensor_data_is_valid(void)
{
    if (data_mutex == NULL)
    {
        return false;
    }

    bool is_valid = false;

    if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        is_valid = sensor_data.valid;
        xSemaphoreGive(data_mutex);
    }

    return is_valid;
}
