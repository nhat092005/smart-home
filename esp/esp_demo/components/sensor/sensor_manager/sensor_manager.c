/**
 * @file sensor_manager.c
 *
 * @brief Sensor Manager Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "sensor_manager.h"
#include "esp_log.h"
#include <string.h>
#include <sys/time.h>

/* DEMO MODE: I2C and sensor driver includes removed - not used */

/* Exported variables ---------------------------------------------------------*/

// Initialization flags (exported for sensor_reader.c)
bool initialized = false;

/* DEMO MODE: Device descriptors removed - not used in mock mode */

// Sensor readiness flags (kept for compatibility but not actively used in demo mode)
bool ds3231_ready = false;
bool sht3x_ready = false;
bool bh1750_ready = false;
bool sh1106_ready = false;

/* Private variables ----------------------------------------------------------*/

static const char *TAG = "SENSOR_MANAGER";

/* DEMO MODE: I2C configuration removed - not needed */

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize sensor manager with default I2C pins
 */
esp_err_t sensor_manager_init_default(void)
{
    ESP_LOGI(TAG, "Initializing Sensor Manager with default pins");
    return sensor_manager_init(I2C_MASTER_SDA_PIN, I2C_MASTER_SCL_PIN);
}

/**
 * @brief Initialize sensor manager and all connected sensors with custom pins
 * DEMO MODE: No I2C initialization, just sets flags for mock data operation
 */
esp_err_t sensor_manager_init(gpio_num_t sda, gpio_num_t scl)
{
    ESP_LOGI(TAG, "[DEMO MODE] Initializing Sensor Manager (SDA=%d, SCL=%d)", sda, scl);
    ESP_LOGI(TAG, "[DEMO MODE] Skipping I2C and sensor initialization - using mock data");

    if (initialized)
    {
        ESP_LOGW(TAG, "Sensor Manager already initialized");
        return ESP_OK;
    }

    // DEMO MODE: No actual I2C or sensor initialization
    // Just set the initialized flag to true so sensor_reader can work
    initialized = true;

    // Note: We don't set ds3231_ready, sht3x_ready, bh1750_ready, sh1106_ready to true
    // because sensor_reader.c in demo mode doesn't check these flags anymore
    
    ESP_LOGI(TAG, "[DEMO MODE] Sensor Manager initialized - ready for mock data");

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
    status->sh1106_ok = sh1106_ready;

    ESP_LOGD(TAG, "Sensor status: DS3231=%d, SHT3x=%d, BH1750=%d, SH1106=%d", 
             ds3231_ready, sht3x_ready, bh1750_ready, sh1106_ready);

    return ESP_OK;
}

/**
 * @brief Deinitialize sensor manager and free resources
 * DEMO MODE: No resources to free
 */
esp_err_t sensor_manager_deinit(void)
{
    if (!initialized)
    {
        ESP_LOGW(TAG, "Sensor manager not initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "[DEMO MODE] Deinitializing sensor manager");

    // DEMO MODE: No actual resources to free
    // All sensor ready flags set to false for consistency
    ds3231_ready = false;
    sht3x_ready = false;
    bh1750_ready = false;
    sh1106_ready = false;

    initialized = false;
    ESP_LOGI(TAG, "[DEMO MODE] Sensor manager deinitialized");

    return ESP_OK;
}

/**
 * @brief Get current timestamp from DS3231 RTC
 * DEMO MODE: Returns system time instead of RTC time
 */
esp_err_t sensor_manager_get_timestamp(uint32_t *timestamp)
{
    if (timestamp == NULL)
    {
        ESP_LOGE(TAG, "Invalid argument: NULL pointer");
        return ESP_ERR_INVALID_ARG;
    }

    // DEMO MODE: Use system time instead of DS3231
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *timestamp = (uint32_t)tv.tv_sec;

    ESP_LOGD(TAG, "[DEMO MODE] Using system timestamp: %lu", (unsigned long)*timestamp);

    return ESP_OK;
}

/**
 * @brief Set timestamp to DS3231 RTC
 * DEMO MODE: No operation (system time is used)
 */
esp_err_t sensor_manager_set_timestamp(uint32_t timestamp)
{
    ESP_LOGI(TAG, "[DEMO MODE] Ignoring timestamp set request: %lu", (unsigned long)timestamp);
    ESP_LOGI(TAG, "[DEMO MODE] System time is used automatically");
    
    return ESP_OK;
}

/**
 * @brief Get SH1106 display device descriptor
 * DEMO MODE: Returns NULL as display is not initialized
 */
sh1106_t* sensor_manager_get_display_device(void)
{
    ESP_LOGD(TAG, "[DEMO MODE] Display not initialized - returning NULL");
    return NULL;
}
