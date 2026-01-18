/**
 * @file i2cdev_config.h
 *
 * @brief I2C Configuration from Kconfig
 */

#ifndef I2CDEV_CONFIG_H
#define I2CDEV_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#include "driver/gpio.h"
#include "driver/i2c_types.h"
#include "sdkconfig.h"

/* Exported defines ----------------------------------------------------------*/

/* I2C Master Pin Definitions */
#define I2C_MASTER_SDA_PIN ((gpio_num_t)CONFIG_I2C_MASTER_SDA_PIN)
#define I2C_MASTER_SCL_PIN ((gpio_num_t)CONFIG_I2C_MASTER_SCL_PIN)

/* I2C Timeout */
#define I2C_TIMEOUT_MS      CONFIG_I2CDEV_TIMEOUT_MS

/* I2C Master Frequency */
#define I2C_MASTER_FREQ_HZ  CONFIG_I2C_MASTER_FREQ_HZ

/* I2C Debugging */
#define I2CDEV_DEBUG        CONFIG_I2CDEV_DEBUG

#endif /* I2CDEV_CONFIG_H */
