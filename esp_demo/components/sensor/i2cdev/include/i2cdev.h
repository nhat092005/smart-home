/**
 * @file i2cdev.h
 *
 * @brief I2C Device Abstraction API
 */

#ifndef I2CDEV_H
#define I2CDEV_H

/* Includes ------------------------------------------------------------------*/

#include "i2cdev_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/* Exported macros -----------------------------------------------------------*/

/**
 * @brief Mutex management macros
 */
#define I2C_DEV_TAKE_MUTEX(dev)                          \
    do                                                   \
    {                                                    \
        if ((dev)->mutex)                                \
            xSemaphoreTake((dev)->mutex, portMAX_DELAY); \
    } while (0)

/**
 * @brief Give mutex back
 */
#define I2C_DEV_GIVE_MUTEX(dev)           \
    do                                    \
    {                                     \
        if ((dev)->mutex)                 \
            xSemaphoreGive((dev)->mutex); \
    } while (0)

/**
 * @brief Check function return value and handle errors
 */
#define I2C_DEV_CHECK(dev, func)     \
    do                               \
    {                                \
        esp_err_t __err = (func);    \
        if (__err != ESP_OK)         \
        {                            \
            I2C_DEV_GIVE_MUTEX(dev); \
            return __err;            \
        }                            \
    } while (0)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief I2C device descriptor
 */
typedef struct
{
    int port;                //!< I2C port number (0 or 1)
    uint8_t addr;            //!< I2C device address
    gpio_num_t sda_io_num;   //!< GPIO number for SDA
    gpio_num_t scl_io_num;   //!< GPIO number for SCL
    uint32_t clk_speed;      //!< I2C clock speed in Hz
    SemaphoreHandle_t mutex; //!< Mutex for thread-safe access
    void *dev_handle;        //!< I2C device handle (i2c_master_dev_handle_t)
} i2c_dev_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize I2C bus
 *
 * @param[in] port I2C port number (0 or 1)
 * @param[in] sda_gpio GPIO pin for SDA
 * @param[in] scl_gpio GPIO pin for SCL
 * @param[in] clk_speed I2C clock speed in Hz
 *
 * @return ESP_OK on success, otherwise error code
 */
esp_err_t i2c_bus_init(int port, gpio_num_t sda_gpio, gpio_num_t scl_gpio, uint32_t clk_speed);

/**
 * @brief Initialize I2C device handle (add device to bus)
 *
 * @param[in] dev Device descriptor
 *
 * @return ESP_OK on success
 */
esp_err_t i2c_dev_init(i2c_dev_t *dev);

/**
 * @brief Create mutex for I2C device
 *
 * @param[in] dev Device descriptor
 *
 * @return ESP_OK on success
 */
esp_err_t i2c_dev_create_mutex(i2c_dev_t *dev);

/**
 * @brief Delete mutex for I2C device
 *
 * @param[in] dev Device descriptor
 *
 * @return ESP_OK on success
 */
esp_err_t i2c_dev_delete_mutex(i2c_dev_t *dev);

/**
 * @brief Read from I2C device register
 *
 * @param[in] dev Device descriptor
 * @param[in] reg Register address to read from
 * @param[out] data Buffer to store read data
 * @param[in] len Number of bytes to read
 *
 * @return ESP_OK on success, otherwise error code
 */
esp_err_t i2c_dev_read_reg(i2c_dev_t *dev, uint8_t reg, void *data, size_t len);

/**
 * @brief Write to I2C device register
 *
 * @param[in] dev Device descriptor
 * @param[in] reg Register address to write to
 * @param[in] data Data to write
 * @param[in] len Number of bytes to write
 *
 * @return ESP_OK on success, otherwise error code
 */
esp_err_t i2c_dev_write_reg(i2c_dev_t *dev, uint8_t reg, const void *data, size_t len);

/**
 * @brief Read data from I2C device (without register address)
 *
 * @param[in] dev Device descriptor
 * @param[out] data Buffer to store read data
 * @param[in] len Number of bytes to read
 *
 * @return ESP_OK on success, otherwise error code
 */
esp_err_t i2c_dev_read(i2c_dev_t *dev, void *data, size_t len);

/**
 * @brief Write data to I2C device (without register address)
 *
 * @param[in] dev Device descriptor
 * @param[in] data Data to write
 * @param[in] len Number of bytes to write
 *
 * @return ESP_OK on success, otherwise error code
 */
esp_err_t i2c_dev_write(i2c_dev_t *dev, const void *data, size_t len);

#endif /* I2CDEV_H */
