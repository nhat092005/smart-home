/**
 * @file sensor_reader.c
 * @brief Implementation of sensor reading operations (DEMO MODE - Mock Data)
 */

/* Includes ------------------------------------------------------------------*/

#include "sensor_reader.h"
#include "esp_log.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* External variables ---------------------------------------------------------*/

extern bool initialized;

/* Private variables ----------------------------------------------------------*/

static const char *TAG = "SENSOR_READER";

// Mock data array for demo mode - rotating values
static const struct
{
    float temperature; // Celsius
    float humidity;    // Percent
    uint16_t light;    // Lux
} mock_sensor_data[] = {
    {25.55, 60.01, 300}, // Sample 1: Comfortable indoor
    {26.22, 58.55, 450}, // Sample 2: Slightly warmer
    {24.82, 62.50, 280}, // Sample 3: Cooler
    {27.07, 55.30, 600}, // Sample 4: Warm, bright
    {23.50, 65.20, 200}, // Sample 5: Cool, dim
    {25.08, 60.50, 350}, // Sample 6: Normal
    {26.50, 57.05, 500}, // Sample 7: Warmer, brighter
    {24.07, 63.50, 250}, // Sample 8: Cool
    {28.04, 52.04, 700}, // Sample 9: Hot, bright
    {22.01, 68.04, 150}, // Sample 10: Cold, dark
    {25.80, 59.01, 400}, // Sample 11: Normal
    {26.80, 56.50, 520}, // Sample 12: Warm
    {24.50, 64.08, 230}, // Sample 13: Cool
    {27.50, 54.01, 650}, // Sample 14: Hot
    {23.04, 66.50, 180}, // Sample 15: Cold
    {25.20, 60.80, 370}, // Sample 16: Normal
    {26.08, 58.05, 480}, // Sample 17: Warm
    {24.20, 62.50, 260}, // Sample 18: Cool
    {27.20, 53.50, 620}, // Sample 19: Hot
    {22.50, 67.02, 160}  // Sample 20: Cold
};

#define MOCK_DATA_COUNT (sizeof(mock_sensor_data) / sizeof(mock_sensor_data[0]))

static uint32_t current_mock_index = 0;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Read all sensor values in one call (DEMO MODE - Returns mock data)
 */
esp_err_t sensor_reader_read_all(sensor_data_t *data)
{
    if (data == NULL)
    {
        ESP_LOGE(TAG, "Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }

    if (!initialized)
    {
        ESP_LOGE(TAG, "Sensor manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "[DEMO MODE] Reading mock sensor data...");

    // Initialize data structure
    memset(data, 0, sizeof(sensor_data_t));

    // Get current timestamp from system time
    struct timeval tv;
    gettimeofday(&tv, NULL);
    data->timestamp = (uint32_t)tv.tv_sec;

    // Get mock data from array (rotating)
    data->temperature = mock_sensor_data[current_mock_index].temperature;
    data->humidity = mock_sensor_data[current_mock_index].humidity;
    data->light = mock_sensor_data[current_mock_index].light;

    // All data is valid in demo mode
    data->valid = true;

    ESP_LOGI(TAG, "[DEMO] Mock data #%lu: temp=%.2f°C, humidity=%.2f%%, light=%u lux, timestamp=%lu",
             current_mock_index,
             data->temperature,
             data->humidity,
             data->light,
             data->timestamp);

    // Rotate to next mock data
    current_mock_index = (current_mock_index + 1) % MOCK_DATA_COUNT;

    return ESP_OK;
}
