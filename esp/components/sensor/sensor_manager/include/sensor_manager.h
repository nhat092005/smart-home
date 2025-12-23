/**
 * @file sensor_manager.h
 *
 * @brief Sensor Manager API
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

/* Includes ------------------------------------------------------------------*/

#include "esp_err.h"
#include "i2cdev_config.h"
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Structure containing all sensor readings
 */
typedef struct
{
    float temperature;  //!< Temperature in degrees Celsius from SHT3x
    float humidity;     //!< Relative humidity in percent from SHT3x
    uint16_t light;     //!< Light intensity in lux from BH1750
    uint32_t timestamp; //!< Unix timestamp from DS3231 RTC
    bool valid;         //!< True if all sensors read successfully
} sensor_data_t;

/**
 * @brief Structure containing sensor health status
 */
typedef struct
{
    bool ds3231_ok; //!< True if DS3231 RTC is responding
    bool sht3x_ok;  //!< True if SHT3x sensor is responding
    bool bh1750_ok; //!< True if BH1750 sensor is responding
} sensor_status_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize sensor manager with default I2C pins
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sensor_manager_init_default(void);

/**
 * @brief Initialize sensor manager and all connected sensors with custom pins
 *
 * This function initializes the I2C bus and all three sensors:
 * - DS3231 RTC for timestamps
 * - SHT3x for temperature and humidity
 * - BH1750 for light intensity
 *
 * @param[in] sda GPIO pin number for I2C SDA line
 * @param[in] scl GPIO pin number for I2C SCL line
 *
 * @return ESP_OK on success, error code otherwise
 *
 * @note After initialization, use sensor_reader.h functions to read data
 */
esp_err_t sensor_manager_init(gpio_num_t sda, gpio_num_t scl);

/**
 * @brief Get health status of all sensors
 *
 * @param[out] status Pointer to sensor_status_t structure
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sensor_manager_get_status(sensor_status_t *status);

/**
 * @brief Get current timestamp from DS3231 RTC
 *
 * @param[out] timestamp Unix timestamp in seconds
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sensor_manager_get_timestamp(uint32_t *timestamp);

/**
 * @brief Deinitialize sensor manager and free resources
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sensor_manager_deinit(void);

#endif /* SENSOR_MANAGER_H */
