/**
 * @file shared_sensor_data.h
 *
 * @brief Shared sensor data API
 */

#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

/* Includes ------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Shared sensor data structure
 */
typedef struct
{
    float temperature;  //!< Temperature in °C
    float humidity;     //!< Humidity in %
    int light;          //!< Light intensity in lux
    uint32_t timestamp; //!< Unix timestamp when data was read
    bool valid;         //!< Data validity flag
} shared_sensor_data_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize shared sensor data module
 *
 * @return ESP_OK on success, error code otherwise
 *
 * @note Must be called once before using other functions
 */
esp_err_t shared_sensor_data_init(void);

/**
 * @brief Update shared sensor data (thread-safe)
 *
 * Called by task_mode after reading sensors.
 *
 * @param[in] temperature Temperature value
 * @param[in] humidity Humidity value
 * @param[in] light Light intensity value
 * @param[in] timestamp Unix timestamp
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t shared_sensor_data_update(float temperature, float humidity, int light, uint32_t timestamp);

/**
 * @brief Get shared sensor data (thread-safe)
 *
 * @param[out] data Pointer to structure to fill with sensor data
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if data not valid
 *
 * @note Called by task_display and task_mqtt to read sensor values
 */
esp_err_t shared_sensor_data_get(shared_sensor_data_t *data);

/**
 * @brief Check if shared sensor data is valid
 *
 * @return true if data is valid, false otherwise
 */
bool shared_sensor_data_is_valid(void);

#endif /* SHARED_SENSOR_DATA_H */
