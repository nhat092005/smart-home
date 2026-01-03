
/**
 * @file sht3x.h
 *
 * @brief SHT3x Temperature and Humidity Sensor Driver API
 */

#ifndef SHT3X_H
#define SHT3X_H

/* Includes ------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include <i2cdev.h>
#include <esp_err.h>

/* Exported defines ----------------------------------------------------------*/

#define SHT3X_I2C_ADDR_GND 0x44
#define SHT3X_I2C_ADDR_VDD 0x45

#define SHT3X_RAW_DATA_SIZE 6

/* Exported types ------------------------------------------------------------*/

typedef uint8_t sht3x_raw_data_t[SHT3X_RAW_DATA_SIZE];

/**
 * @brief Measurement modes
 */
typedef enum
{
    SHT3X_SINGLE_SHOT = 0, //!< Single measurement (sensor powers down after)
    SHT3X_PERIODIC_05MPS,  //!< Periodic mode: 0.5 measurements/sec
    SHT3X_PERIODIC_1MPS,   //!< Periodic mode: 1 measurement/sec
    SHT3X_PERIODIC_2MPS,   //!< Periodic mode: 2 measurements/sec
    SHT3X_PERIODIC_4MPS,   //!< Periodic mode: 4 measurements/sec
    SHT3X_PERIODIC_10MPS   //!< Periodic mode: 10 measurements/sec (highest update rate)
} sht3x_mode_t;

/**
 * @brief Measurement repeatability
 */
typedef enum
{
    SHT3X_HIGH = 0, //!< High repeatability (best accuracy, ~15ms measurement time)
    SHT3X_MEDIUM,   //!< Medium repeatability (balanced, ~6ms)
    SHT3X_LOW       //!< Low repeatability (fastest, ~4ms)
} sht3x_repeat_t;

/**
 * Device descriptor
 */
typedef struct
{
    i2c_dev_t i2c_dev;            //!< I2C device descriptor
    sht3x_mode_t mode;            //!< used measurement mode
    sht3x_repeat_t repeatability; //!< used repeatability
    bool meas_started;            //!< indicates whether measurement started
    uint64_t meas_start_time;     //!< measurement start time in us
    bool meas_first;              //!< first measurement in periodic mode
} sht3x_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize device descriptor
 *
 * @param[in] dev Device descriptor to initialize
 * @param[in] addr I2C address: SHT3X_I2C_ADDR_GND (0x44) or SHT3X_I2C_ADDR_VDD (0x45)
 * @param[in] port I2C port number (I2C_NUM_0 or I2C_NUM_1)
 * @param[in] sda_gpio GPIO pin for SDA line
 * @param[in] scl_gpio GPIO pin for SCL line
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sht3x_init_desc(sht3x_t *dev, uint8_t addr, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio);

/**
 * @brief Free device descriptor
 *
 * @param[in] dev Device descriptor
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sht3x_free_desc(sht3x_t *dev);

/**
 * @brief Initialize sensor
 *
 * @param[in] dev Device descriptor
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sht3x_init(sht3x_t *dev);

/**
 * @brief Enable/disable internal heater
 *
 * @param[in] dev Device descriptor
 * @param[in] enable true to enable heater, false to disable
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sht3x_set_heater(sht3x_t *dev, bool enable);

/**
 * @brief High level measurement function (convenience)
 *
 * @param[in] dev Device descriptor
 * @param[out] temperature Temperature in degrees Celsius (-40 to +125°C), can be NULL
 * @param[out] humidity Relative humidity in percent (0-100%), can be NULL
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sht3x_measure(sht3x_t *dev, float *temperature, float *humidity);

/**
 * @brief Get the duration of a measurement in RTOS ticks.
 *
 * @param[in] repeat Repeatability, see type ::sht3x_repeat_t
 * @return Measurement duration given in RTOS ticks
 */
uint8_t sht3x_get_measurement_duration(sht3x_repeat_t repeat);

/**
 * @brief Start the measurement in single shot or periodic mode
 *
 * @param[in] dev Device descriptor
 * @param[in] mode Measurement mode, see type ::sht3x_mode_t
 * @param[in] repeat Repeatability, see type ::sht3x_repeat_t
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sht3x_start_measurement(sht3x_t *dev, sht3x_mode_t mode, sht3x_repeat_t repeat);

/**
 * @brief Stop the periodic mode measurements
 *
 * The function stops the measurements  in *periodic mode*
 * (periodic measurements) and the sensor returns in *single shot mode*
 *
 * @param[in] dev Device descriptor
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sht3x_stop_periodic_measurement(sht3x_t *dev);

/**
 * @brief Read measurement results from sensor as raw data
 *
 * The function read measurement results from the sensor, checks the CRC
 * checksum and stores them in the byte array as following.
 *
 *      data[0] = Temperature MSB
 *      data[1] = Temperature LSB
 *      data[2] = Temperature CRC
 *      data[3] = Humidity MSB
 *      data[4] = Humidity LSB
 *      data[5] = Humidity CRC
 *
 * In case that there are no new data that can be read, the function fails.
 *
 * @param[in] dev Device descriptor
 * @param[out] raw_data  Byte array in which raw data are stored
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sht3x_get_raw_data(sht3x_t *dev, sht3x_raw_data_t raw_data);

/**
 * @brief Computes sensor values from raw data
 *
 * @param raw_data Byte array that contains raw data
 * @param[out] temperature Temperature in degree Celsius
 * @param[out] humidity Humidity in percent
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sht3x_compute_values(sht3x_raw_data_t raw_data, float *temperature, float *humidity);

/**
 * @brief Get measurement results in form of sensor values
 *
 * The function combines function ::sht3x_get_raw_data() and function
 * ::sht3x_compute_values() to get the measurement results.
 *
 * In case that there are no results that can be read, the function fails.
 *
 * @param dev         Device descriptor
 * @param temperature Temperature in degree Celsius
 * @param humidity    Humidity in percent
 * @return            `ESP_OK` on success
 */
esp_err_t sht3x_get_results(sht3x_t *dev, float *temperature, float *humidity);

#endif /* SHT3X_H */