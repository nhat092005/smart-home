/**
 * @file sensor_reader.h
 *
 * @brief Sensor Reader API
 */

#ifndef SENSOR_READER_H
#define SENSOR_READER_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sensor_manager.h"

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Read all sensor values in one call
 *
 * Reads temperature, humidity, light intensity, and timestamp from all sensors.
 * If one sensor fails, the function continues reading others and sets valid flag.
 *
 * @param[out] data Pointer to sensor_data_t structure to store readings
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sensor_reader_read_all(sensor_data_t *data);

#endif /* SENSOR_READER_H */
