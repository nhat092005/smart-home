/**
 * @file sensor_reader.c
 * @brief Implementation of sensor reading operations
 */

/* Includes ------------------------------------------------------------------*/

#include "sensor_reader.h"
#include "ds3231.h"
#include "sht3x.h"
#include "bh1750.h"
#include "esp_log.h"
#include <string.h>

/* External variables ---------------------------------------------------------*/

// Forward declarations - internal access only
extern ds3231_t ds3231_dev;
extern sht3x_t sht3x_dev;
extern bh1750_t bh1750_dev;

extern bool initialized;

extern bool ds3231_ready;
extern bool sht3x_ready;
extern bool bh1750_ready;

/* Private variables ----------------------------------------------------------*/

static const char *TAG = "SENSOR_READER";

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Read all sensor values in one call
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

    ESP_LOGI(TAG, "Reading all sensors...");

    // Initialize data structure
    memset(data, 0, sizeof(sensor_data_t));

    // Track individual sensor success
    bool ds3231_success = false;
    bool sht3x_success = false;
    bool bh1750_success = false;

    // Read DS3231 timestamp
    if (ds3231_ready)
    {
        esp_err_t ret = ds3231_get_timestamp(&ds3231_dev, &data->timestamp);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "DS3231 read failed: %s", esp_err_to_name(ret));
            ds3231_ready = false;
        }
        else
        {
            ESP_LOGI(TAG, "DS3231: timestamp %lu", data->timestamp);
            ds3231_success = true;
        }
    }
    else
    {
        ESP_LOGW(TAG, "DS3231 not available (not initialized)");
    }

    // Read SHT3x temperature and humidity
    if (sht3x_ready)
    {
        esp_err_t ret = sht3x_measure(&sht3x_dev, &data->temperature, &data->humidity);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "SHT3x read failed: %s", esp_err_to_name(ret));
            sht3x_ready = false;
        }
        else
        {
            ESP_LOGI(TAG, "SHT3x: temp=%.2f°C, humidity=%.2f%%",
                     data->temperature, data->humidity);
            sht3x_success = true;
        }
    }
    else
    {
        ESP_LOGW(TAG, "SHT3x not available (not initialized)");
    }

    // Read BH1750 light intensity
    if (bh1750_ready)
    {
        esp_err_t ret = bh1750_read_light_basic(&bh1750_dev, &data->light);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "BH1750 read failed: %s", esp_err_to_name(ret));
            bh1750_ready = false;
        }
        else
        {
            ESP_LOGI(TAG, "BH1750: light=%u lux", data->light);
            bh1750_success = true;
        }
    }
    else
    {
        ESP_LOGW(TAG, "BH1750 not available (not initialized)");
    }

    // Determine overall validity
    data->valid = ds3231_success && sht3x_success && bh1750_success;

    if (data->valid)
    {
        ESP_LOGI(TAG, "All sensors read successfully");
    }
    else
    {
        ESP_LOGW(TAG, "Partial success: DS3231=%d, SHT3x=%d, BH1750=%d",
                 ds3231_success, sht3x_success, bh1750_success);
    }

    return ESP_OK;
}
