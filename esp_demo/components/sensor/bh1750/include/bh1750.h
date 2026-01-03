/**
 * @file bh1750.h
 *
 * @brief BH1750 Light Sensor Driver API
 */

#ifndef BH1750_H
#define BH1750_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "i2cdev.h"
#include <esp_err.h>

/* Exported defines ----------------------------------------------------------*/

#define BH1750_ADDR_LO 0x23 //!< I2C address when ADDR pin floating/low
#define BH1750_ADDR_HI 0x5c //!< I2C address when ADDR pin high

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Measurement mode
 */
typedef enum
{
    BH1750_MODE_ONE_TIME = 0, //!< One time measurement - device powers down after measurement
    BH1750_MODE_CONTINUOUS    //!< Continuous measurement - device stays powered, can read repeatedly
} bh1750_mode_t;

/**
 * @brief Measurement resolution
 */
typedef enum
{
    BH1750_RES_LOW = 0, //!< 4 lx resolution, measurement time ~16ms
    BH1750_RES_HIGH,    //!< 1 lx resolution, measurement time ~120ms (default)
    BH1750_RES_HIGH2    //!< 0.5 lx resolution, measurement time ~120ms (highest accuracy)
} bh1750_resolution_t;

/**
 * Device descriptor
 */
typedef struct
{
    i2c_dev_t i2c_dev; //!< I2C device descriptor
} bh1750_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize device descriptor
 *
 * Must be called before any other BH1750 functions. Creates mutex for thread-safety.
 *
 * @param[out] dev Device descriptor to initialize
 * @param[in] addr I2C address: BH1750_ADDR_LO (0x23) or BH1750_ADDR_HI (0x5C)
 * @param[in] port I2C port number (I2C_NUM_0 or I2C_NUM_1)
 * @param[in] sda_gpio GPIO pin number for SDA line
 * @param[in] scl_gpio GPIO pin number for SCL line
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t bh1750_init_desc(bh1750_t *dev, uint8_t addr, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio);

/**
 * @brief Free device descriptor
 *
 * @param[in] dev Pointer to device descriptor
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t bh1750_free_desc(bh1750_t *dev);

/**
 * @brief Setup device measurement parameters
 *
 * Configures the measurement mode and resolution. Device must be powered on first.
 *
 * @param[in] dev Pointer to device descriptor
 * @param[in] mode Measurement mode (one-time or continuous)
 * @param[in] resolution Measurement resolution (low/high/high2)
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t bh1750_setup(bh1750_t *dev, bh1750_mode_t mode, bh1750_resolution_t resolution);

/**
 * @brief One-shot light measurement (convenience function)
 *
 * @param[in] dev Pointer to device descriptor
 * @param[out] lux Light level in lux units (0-65535)
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t bh1750_read_light(bh1750_t *dev, uint16_t *lux);

/**
 * @brief Basic one-shot light measurement (without detailed logging)
 *
 * @param[in] dev Pointer to device descriptor
 * @param[out] lux Light level in lux units (0-65535)
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t bh1750_read_light_basic(bh1750_t *dev, uint16_t *lux);

#endif /* BH1750_H */