/**
 * @file i2cdev.c
 *
 * @brief I2C Device Abstraction Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "i2cdev.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string.h>

/* Private defines -----------------------------------------------------------*/

static const char *TAG = "I2CDEV";
static i2c_master_bus_handle_t i2c_bus_handle = NULL;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize I2C bus using new I2C Master API
 */
esp_err_t i2c_bus_init(int port, gpio_num_t sda_gpio, gpio_num_t scl_gpio, uint32_t clk_speed)
{
    ESP_LOGI(TAG, "Initializing I2C bus on port %d (SDA: GPIO%d, SCL: GPIO%d, Speed: %lu Hz)",
             port, sda_gpio, scl_gpio, clk_speed);

    // Check if already initialized
    if (i2c_bus_handle != NULL)
    {
        ESP_LOGW(TAG, "I2C bus already initialized on port %d", port);
        return ESP_OK;
    }

    i2c_master_bus_config_t bus_config = {
        .i2c_port = port,
        .sda_io_num = sda_gpio,
        .scl_io_num = scl_gpio,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t ret = i2c_new_master_bus(&bus_config, &i2c_bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize I2C master bus: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "I2C bus initialized successfully on port %d", port);
    return ESP_OK;
}

/**
 * @brief Initialize I2C device handle (add device to bus once)
 */
esp_err_t i2c_dev_init(i2c_dev_t *dev)
{
    if (!dev)
    {
        ESP_LOGE(TAG, "Device descriptor is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (dev->dev_handle != NULL)
    {
        ESP_LOGW(TAG, "Device 0x%02x already initialized", dev->addr);
        return ESP_OK;
    }

    if (i2c_bus_handle == NULL)
    {
        ESP_LOGE(TAG, "I2C bus not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Create device configuration
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev->addr,
        .scl_speed_hz = dev->clk_speed,
    };

    esp_err_t ret = i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg,
                                               (i2c_master_dev_handle_t *)&dev->dev_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add device 0x%02x: %s", dev->addr, esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Device 0x%02x added successfully (speed: %lu Hz)", dev->addr, dev->clk_speed);
    return ESP_OK;
}

/**
 * @brief Create mutex for I2C device
 */
esp_err_t i2c_dev_create_mutex(i2c_dev_t *dev)
{
    if (!dev)
    {
        ESP_LOGE(TAG, "Device descriptor is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (!dev->mutex)
    {
        dev->mutex = xSemaphoreCreateMutex();
        if (!dev->mutex)
        {
            ESP_LOGE(TAG, "Failed to create mutex");
            return ESP_ERR_NO_MEM;
        }
        ESP_LOGD(TAG, "Mutex created for device 0x%02x on port %d", dev->addr, dev->port);
    }

    return ESP_OK;
}

/**
 * @brief Delete mutex for I2C device
 */
esp_err_t i2c_dev_delete_mutex(i2c_dev_t *dev)
{
    if (!dev)
    {
        ESP_LOGE(TAG, "Device descriptor is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (dev->mutex)
    {
        vSemaphoreDelete(dev->mutex);
        dev->mutex = NULL;
        ESP_LOGD(TAG, "Mutex deleted for device 0x%02x", dev->addr);
    }

    return ESP_OK;
}

/**
 * @brief Read from I2C device register using new I2C Master API
 */
esp_err_t i2c_dev_read_reg(i2c_dev_t *dev, uint8_t reg, void *data, size_t len)
{
    if (!dev || !data || len == 0)
    {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    if (dev->dev_handle == NULL)
    {
        ESP_LOGE(TAG, "Device 0x%02x not initialized", dev->addr);
        return ESP_ERR_INVALID_STATE;
    }

    I2C_DEV_TAKE_MUTEX(dev);

    // Use existing device handle
    i2c_master_dev_handle_t dev_handle = (i2c_master_dev_handle_t)dev->dev_handle;

    // Write register address then read data
    esp_err_t ret = i2c_master_transmit_receive(dev_handle, &reg, 1, (uint8_t *)data, len, I2C_TIMEOUT_MS);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C read failed from addr 0x%02x reg 0x%02x: %s",
                 dev->addr, reg, esp_err_to_name(ret));
    }
#if CONFIG_I2CDEV_DEBUG
    else
    {
        ESP_LOGD(TAG, "Read %d bytes from addr 0x%02x reg 0x%02x", len, dev->addr, reg);
    }
#endif

    I2C_DEV_GIVE_MUTEX(dev);
    return ret;
}

/**
 * @brief Write to I2C device register using new I2C Master API
 */
esp_err_t i2c_dev_write_reg(i2c_dev_t *dev, uint8_t reg, const void *data, size_t len)
{
    if (!dev || !data || len == 0)
    {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    if (dev->dev_handle == NULL)
    {
        ESP_LOGE(TAG, "Device 0x%02x not initialized", dev->addr);
        return ESP_ERR_INVALID_STATE;
    }

    I2C_DEV_TAKE_MUTEX(dev);

    // Use existing device handle
    i2c_master_dev_handle_t dev_handle = (i2c_master_dev_handle_t)dev->dev_handle;

    // Prepare buffer: register address + data
    uint8_t *write_buf = malloc(len + 1);
    if (!write_buf)
    {
        I2C_DEV_GIVE_MUTEX(dev);
        return ESP_ERR_NO_MEM;
    }

    write_buf[0] = reg;
    memcpy(write_buf + 1, data, len);

    esp_err_t ret = i2c_master_transmit(dev_handle, write_buf, len + 1, I2C_TIMEOUT_MS);

    free(write_buf);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C write failed to addr 0x%02x reg 0x%02x: %s",
                 dev->addr, reg, esp_err_to_name(ret));
    }
#if CONFIG_I2CDEV_DEBUG
    else
    {
        ESP_LOGD(TAG, "Wrote %d bytes to addr 0x%02x reg 0x%02x", len, dev->addr, reg);
    }
#endif

    I2C_DEV_GIVE_MUTEX(dev);
    return ret;
}

/**
 * @brief Read data from I2C device (without register address) using new I2C Master API
 */
esp_err_t i2c_dev_read(i2c_dev_t *dev, void *data, size_t len)
{
    if (!dev || !data || len == 0)
    {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    if (dev->dev_handle == NULL)
    {
        ESP_LOGE(TAG, "Device 0x%02x not initialized", dev->addr);
        return ESP_ERR_INVALID_STATE;
    }

    I2C_DEV_TAKE_MUTEX(dev);

    // Use existing device handle
    i2c_master_dev_handle_t dev_handle = (i2c_master_dev_handle_t)dev->dev_handle;

    esp_err_t ret = i2c_master_receive(dev_handle, (uint8_t *)data, len, I2C_TIMEOUT_MS);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C read failed from addr 0x%02x: %s", dev->addr, esp_err_to_name(ret));
    }

    I2C_DEV_GIVE_MUTEX(dev);
    return ret;
}

/**
 * @brief Write data to I2C device (without register address) using new I2C Master API
 */
esp_err_t i2c_dev_write(i2c_dev_t *dev, const void *data, size_t len)
{
    if (!dev || !data || len == 0)
    {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    if (dev->dev_handle == NULL)
    {
        ESP_LOGE(TAG, "Device 0x%02x not initialized", dev->addr);
        return ESP_ERR_INVALID_STATE;
    }

    I2C_DEV_TAKE_MUTEX(dev);

    // Use existing device handle
    i2c_master_dev_handle_t dev_handle = (i2c_master_dev_handle_t)dev->dev_handle;

    esp_err_t ret = i2c_master_transmit(dev_handle, (const uint8_t *)data, len, I2C_TIMEOUT_MS);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C write failed to addr 0x%02x: %s", dev->addr, esp_err_to_name(ret));
    }

    I2C_DEV_GIVE_MUTEX(dev);
    return ret;
}
