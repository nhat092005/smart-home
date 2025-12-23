/**
 * @file sensor_manager.c
 *
 * @brief Sensor Manager Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "sensor_manager.h"
#include "ds3231.h"
#include "sht3x.h"
#include "bh1750.h"
#include "esp_log.h"
#include <string.h>

/* Private variables ----------------------------------------------------------*/

static const char *TAG = "SENSOR_MANAGER";

// I2C device descriptors (exported for sensor_reader.c)
ds3231_t ds3231_dev;
sht3x_t sht3x_dev;
bh1750_t bh1750_dev;

// Initialization flags (exported for sensor_reader.c)
bool initialized = false;

bool ds3231_ready = false;
bool sht3x_ready = false;
bool bh1750_ready = false;

// I2C configuration
static int i2c_port = 0; // I2C port 0

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize sensor manager with default I2C pins
 */
esp_err_t sensor_manager_init_default(void)
{
    ESP_LOGI(TAG, "Initializing sensor manager with default pins");
    return sensor_manager_init(I2C_MASTER_SDA_PIN, I2C_MASTER_SCL_PIN);
}

/**
 * @brief Initialize sensor manager and all connected sensors with custom pins
 */
esp_err_t sensor_manager_init(gpio_num_t sda, gpio_num_t scl)
{
    esp_err_t ret;

    ESP_LOGI(TAG, "Initializing sensor manager (SDA=%d, SCL=%d)", sda, scl);

    if (initialized)
    {
        ESP_LOGW(TAG, "Sensor manager already initialized");
        return ESP_OK;
    }

    // Initialize I2C bus
    ret = i2c_bus_init(i2c_port, sda, scl, I2C_MASTER_FREQ_HZ);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize I2C bus: %s", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "I2C bus initialized successfully");

    // Initialize DS3231 RTC
    ret = ds3231_init_desc(&ds3231_dev, i2c_port, sda, scl);
    if (ret == ESP_OK)
    {
        // Verify hardware by reading time
        struct tm time;
        ret = ds3231_get_time(&ds3231_dev, &time);
        if (ret == ESP_OK)
        {
            ds3231_ready = true;
            ESP_LOGI(TAG, "DS3231 RTC initialized");
        }
        else
        {
            ds3231_free_desc(&ds3231_dev);
            ds3231_ready = false;
            ESP_LOGW(TAG, "DS3231 hardware verification failed: %s", esp_err_to_name(ret));
        }
    }
    else
    {
        ds3231_ready = false;
        ESP_LOGW(TAG, "DS3231 initialization failed: %s", esp_err_to_name(ret));
    }

    // Initialize SHT3x sensor
    ret = sht3x_init_desc(&sht3x_dev, SHT3X_I2C_ADDR_GND, i2c_port, sda, scl);
    if (ret == ESP_OK)
    {
        ret = sht3x_init(&sht3x_dev);
        if (ret == ESP_OK)
        {
            sht3x_ready = true;
            ESP_LOGI(TAG, "SHT3x sensor initialized");
        }
        else
        {
            sht3x_free_desc(&sht3x_dev);
            sht3x_ready = false;
            ESP_LOGW(TAG, "SHT3x hardware verification failed: %s", esp_err_to_name(ret));
        }
    }
    else
    {
        sht3x_ready = false;
        ESP_LOGW(TAG, "SHT3x initialization failed: %s", esp_err_to_name(ret));
    }

    // Initialize BH1750 light sensor
    ret = bh1750_init_desc(&bh1750_dev, BH1750_ADDR_LO, i2c_port, sda, scl);
    if (ret == ESP_OK)
    {
        // Verify hardware by setup sensor
        ret = bh1750_setup(&bh1750_dev, BH1750_MODE_CONTINUOUS, BH1750_RES_HIGH);
        if (ret == ESP_OK)
        {
            bh1750_ready = true;
            ESP_LOGI(TAG, "BH1750 sensor initialized");
        }
        else
        {
            bh1750_free_desc(&bh1750_dev);
            bh1750_ready = false;
            ESP_LOGW(TAG, "BH1750 hardware verification failed: %s", esp_err_to_name(ret));
        }
    }
    else
    {
        bh1750_ready = false;
        ESP_LOGW(TAG, "BH1750 initialization failed: %s", esp_err_to_name(ret));
    }

    // Check if at least one sensor is ready
    if (!ds3231_ready && !sht3x_ready && !bh1750_ready)
    {
        ESP_LOGE(TAG, "All sensor initializations failed");
        return ESP_ERR_NOT_FOUND;
    }

    initialized = true;

    ESP_LOGI(TAG, "Sensor manager initialized (DS3231=%d, SHT3x=%d, BH1750=%d)",
             ds3231_ready, sht3x_ready, bh1750_ready);

    return ESP_OK;
}

/**
 * @brief Get health status of all sensors
 */
esp_err_t sensor_manager_get_status(sensor_status_t *status)
{
    if (status == NULL)
    {
        ESP_LOGE(TAG, "Invalid argument: NULL pointer");
        return ESP_ERR_INVALID_ARG;
    }

    status->ds3231_ok = ds3231_ready;
    status->sht3x_ok = sht3x_ready;
    status->bh1750_ok = bh1750_ready;

    ESP_LOGD(TAG, "Sensor status: DS3231=%d, SHT3x=%d, BH1750=%d",
             ds3231_ready, sht3x_ready, bh1750_ready);

    return ESP_OK;
}

/**
 * @brief Deinitialize sensor manager and free resources
 */
esp_err_t sensor_manager_deinit(void)
{
    if (!initialized)
    {
        ESP_LOGW(TAG, "Sensor manager not initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Deinitializing sensor manager");

    // Free SHT3x resources
    if (sht3x_ready)
    {
        sht3x_free_desc(&sht3x_dev);
        sht3x_ready = false;
        ESP_LOGD(TAG, "SHT3x freed");
    }

    // Free DS3231 resources
    if (ds3231_ready)
    {
        ds3231_free_desc(&ds3231_dev);
        ds3231_ready = false;
        ESP_LOGD(TAG, "DS3231 freed");
    }

    // Free BH1750 resources
    if (bh1750_ready)
    {
        bh1750_free_desc(&bh1750_dev);
        bh1750_ready = false;
        ESP_LOGD(TAG, "BH1750 freed");
    }

    // Note: I2C Master driver (new API) is managed by i2cdev layer
    // No need to explicitly delete it here

    initialized = false;
    ESP_LOGI(TAG, "Sensor manager deinitialized");

    return ESP_OK;
}

/**
 * @brief Get current timestamp from DS3231 RTC
 */
esp_err_t sensor_manager_get_timestamp(uint32_t *timestamp)
{
    if (!ds3231_ready)
    {
        ESP_LOGW(TAG, "DS3231 not ready");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ds3231_get_timestamp(&ds3231_dev, timestamp);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get timestamp from DS3231: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}
