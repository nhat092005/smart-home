/**
 * @file sh1106.h
 *
 * @brief SH1106 OLED Display Driver API
 */

#ifndef SH1106_H
#define SH1106_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "i2cdev.h"

/* Exported defines ----------------------------------------------------------*/

#define SH1106_I2C_ADDR_DEFAULT 0x3C

/* Exported types ------------------------------------------------------------*/

/**
 * @brief SH1106 device descriptor
 */
typedef struct
{
    i2c_dev_t i2c_dev;       //!< I2C device descriptor
    uint8_t buffer[1024];    //!< Display buffer (128x64 / 8)
} sh1106_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize device descriptor
 *
 * @param[in] dev Device descriptor to initialize
 * @param[in] addr I2C address (default 0x3C)
 * @param[in] port I2C port number (I2C_NUM_0 or I2C_NUM_1)
 * @param[in] sda_gpio GPIO pin for SDA line
 * @param[in] scl_gpio GPIO pin for SCL line
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sh1106_init_desc(sh1106_t *dev, uint8_t addr, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio);

/**
 * @brief Free device descriptor
 *
 * @param[in] dev Device descriptor
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sh1106_free_desc(sh1106_t *dev);

/**
 * @brief Initialize SH1106 display
 *
 * @param[in] dev Device descriptor
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sh1106_init(sh1106_t *dev);

/**
 * @brief Set a single pixel in the display buffer
 *
 * @param[in] dev Device descriptor
 * @param[in] x X coordinate (0-127)
 * @param[in] y Y coordinate (0-63)
 * @param[in] color 1 = white/on, 0 = black/off
 */
void sh1106_set_pixel(sh1106_t *dev, int x, int y, uint8_t color);

/**
 * @brief Draw a horizontal line across the entire display width
 *
 * @param[in] dev Device descriptor
 * @param[in] y Y coordinate (0-63)
 */
void sh1106_draw_horizontal_line(sh1106_t *dev, int y);

/**
 * @brief Clear the entire display buffer (set all pixels to 0)
 *
 * @param[in] dev Device descriptor
 */
void sh1106_clear_display(sh1106_t *dev);

/**
 * @brief Send the display buffer to the hardware
 *
 * @param[in] dev Device descriptor
 *
 * @note Call this after modifying the buffer to make changes visible
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t sh1106_update_display(sh1106_t *dev);

/**
 * @brief Get direct access to the display buffer
 *
 * @param[in] dev Device descriptor
 *
 * @return Pointer to internal display buffer (1024 bytes for 128x64 display)
 *
 * @note Buffer format: Each byte represents 8 vertical pixels
 *       buffer[x + (y/8)*128] contains pixels at (x, y) to (x, y+7)
 */
uint8_t *sh1106_get_buffer(sh1106_t *dev);

/**
 * @brief Get display dimensions
 *
 * @param[out] width Pointer to store width (128)
 * @param[out] height Pointer to store height (64)
 */
void sh1106_get_dimensions(int *width, int *height);

#endif /* SH1106_H */
