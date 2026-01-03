/**
 * @file bh1750.c
 *
 * @brief BH1750 Light Sensor Driver Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "bh1750.h"
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include <string.h>

/* Private macros ------------------------------------------------------------*/

/**
 * @brief Error checking macro
 */
#define CHECK(x)                                                           \
    do                                                                     \
    {                                                                      \
        esp_err_t __err = (x);                                             \
        if (__err != ESP_OK)                                               \
        {                                                                  \
            ESP_LOGE(TAG, "Operation failed: %s", esp_err_to_name(__err)); \
            return __err;                                                  \
        }                                                                  \
    } while (0)

/**
 * @brief Argument checking macro
 */
#define CHECK_ARG(VAL)                         \
    do                                         \
    {                                          \
        if (!(VAL))                            \
        {                                      \
            ESP_LOGE(TAG, "Invalid argument"); \
            return ESP_ERR_INVALID_ARG;        \
        }                                      \
    } while (0)

/* Private defines ----------------------------------------------------------- */

// BH1750 Opcode definitions
#define OPCODE_HIGH 0x0  //!< High resolution mode opcode
#define OPCODE_HIGH2 0x1 //!< High resolution mode 2 opcode
#define OPCODE_LOW 0x3   //!< Low resolution mode opcode

#define OPCODE_CONT 0x10 //!< Continuous measurement mode
#define OPCODE_OT 0x20   //!< One-time measurement mode

#define OPCODE_POWER_DOWN 0x00 //!< Power down command
#define OPCODE_POWER_ON 0x01   //!< Power on command
#define OPCODE_MT_HI 0x40      //!< Measurement time high byte
#define OPCODE_MT_LO 0x60      //!< Measurement time low byte

#define I2C_FREQ_HZ I2C_MASTER_FREQ_HZ //!< I2C bus frequency in Hz

/* Private variables --------------------------------------------------------- */

static const char *TAG = "BH1750";

/* Private functions Prototypes --------------------------------------------- */

/**
 * @brief Send command to BH1750 without taking mutex
 *
 * @param[in] dev Pointer to device descriptor
 * @param[in] cmd Command byte to send
 *
 * @return ESP_OK on success, error code otherwise
 */
inline static esp_err_t send_command_nolock(bh1750_t *dev, uint8_t cmd);

/**
 * @brief Send command to BH1750
 *
 * @param[in] dev Pointer to device descriptor
 * @param[in] cmd Command byte to send
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t send_command(bh1750_t *dev, uint8_t cmd);

/**
 * @brief Power on BH1750 device
 *
 * @param[in] dev Pointer to device descriptor
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t bh1750_power_on(bh1750_t *dev);

/**
 * @brief Read raw light level from BH1750
 *
 * @param[in] dev Pointer to device descriptor
 * @param[out] level Pointer to store raw light level
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t bh1750_read(bh1750_t *dev, uint16_t *level);

/* External functions -------------------------------------------------------- */

/**
 * @brief Initialize BH1750 device descriptor
 */
esp_err_t bh1750_init_desc(bh1750_t *dev, uint8_t addr, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio)
{
    CHECK_ARG(dev);

    if (addr != BH1750_ADDR_LO && addr != BH1750_ADDR_HI)
    {
        ESP_LOGE(TAG, "Invalid I2C address: 0x%02x (must be 0x%02x or 0x%02x)", addr, BH1750_ADDR_LO, BH1750_ADDR_HI);
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Initializing BH1750");

    dev->i2c_dev.port = port;
    dev->i2c_dev.addr = addr;
    dev->i2c_dev.sda_io_num = sda_gpio;
    dev->i2c_dev.scl_io_num = scl_gpio;
    dev->i2c_dev.clk_speed = I2C_FREQ_HZ;

    esp_err_t res = i2c_dev_create_mutex(&dev->i2c_dev);
    if (res == ESP_OK)
    {
        ESP_LOGI(TAG, "BH1750 initialized on port %d (addr: 0x%02x, SDA: GPIO%d, SCL: GPIO%d)",
                 port, addr, sda_gpio, scl_gpio);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to initialize BH1750: %s", esp_err_to_name(res));
    }

    ESP_LOGD(TAG, "BH1750 descriptor initialized successfully");
    return ESP_OK;
}

/**
 * @brief Free BH1750 device descriptor
 */
esp_err_t bh1750_free_desc(bh1750_t *dev)
{
    CHECK_ARG(dev);

    ESP_LOGD(TAG, "Freeing BH1750 descriptor");
    esp_err_t ret = i2c_dev_delete_mutex(&dev->i2c_dev);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to delete mutex: %s", esp_err_to_name(ret));
    }
    return ret;
}

/**
 * @brief Power on BH1750 device
 */
esp_err_t bh1750_setup(bh1750_t *dev, bh1750_mode_t mode, bh1750_resolution_t resolution)
{
    CHECK_ARG(dev);

    // Validate mode
    if (mode != BH1750_MODE_ONE_TIME && mode != BH1750_MODE_CONTINUOUS)
    {
        ESP_LOGE(TAG, "Invalid mode: %d (must be 0 or 1)", mode);
        return ESP_ERR_INVALID_ARG;
    }

    // Validate resolution
    if (resolution < BH1750_RES_LOW || resolution > BH1750_RES_HIGH2)
    {
        ESP_LOGE(TAG, "Invalid resolution: %d (must be 0-2)", resolution);
        return ESP_ERR_INVALID_ARG;
    }

    // const char *mode_str = (mode == BH1750_MODE_CONTINUOUS) ? "CONTINUOUS" : "ONE_TIME";
    // const char *res_str;
    uint8_t opcode = mode == BH1750_MODE_CONTINUOUS ? OPCODE_CONT : OPCODE_OT;

    switch (resolution)
    {
    case BH1750_RES_LOW:
        opcode |= OPCODE_LOW;
        // res_str = "LOW (4 lx)";
        break;
    case BH1750_RES_HIGH:
        opcode |= OPCODE_HIGH;
        // res_str = "HIGH (1 lx)";
        break;
    default:
        opcode |= OPCODE_HIGH2;
        // res_str = "HIGH2 (0.5 lx)";
        break;
    }

    // ESP_LOGI(TAG, "Configuring BH1750 (port=%d, addr=0x%02x): mode=%s, resolution=%s",
    //          dev->i2c_dev.port, dev->i2c_dev.addr, mode_str, res_str);

    CHECK(send_command(dev, opcode));

    ESP_LOGD(TAG, "BH1750 setup complete");

    return ESP_OK;
}

/**
 * @brief Perform one-shot light measurement and read result
 */
esp_err_t bh1750_read_light(bh1750_t *dev, uint16_t *lux)
{
    CHECK_ARG(dev && lux);

    ESP_LOGD(TAG, "Starting one-shot light measurement");

    // Power on the device
    esp_err_t ret = bh1750_power_on(dev);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to power on: %s", esp_err_to_name(ret));
        return ret;
    }

    // Setup for one-time high resolution measurement
    ret = bh1750_setup(dev, BH1750_MODE_ONE_TIME, BH1750_RES_HIGH);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to setup: %s", esp_err_to_name(ret));
        return ret;
    }

    // Wait for measurement to complete (typical 120ms for high resolution)
    vTaskDelay(pdMS_TO_TICKS(180));

    // Read the result
    ret = bh1750_read(dev, lux);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "One-shot measurement complete: %d lx", *lux);
    return ESP_OK;
}

/**
 * @brief Basic one-shot light measurement (without detailed logging)
 */
esp_err_t bh1750_read_light_basic(bh1750_t *dev, uint16_t *lux)
{
    CHECK_ARG(dev && lux);

    esp_err_t ret;
    uint8_t cmd;

    // inline bh1750_power_on
    cmd = OPCODE_POWER_ON;
    ret = i2c_dev_write(&dev->i2c_dev, &cmd, 1);
    if (ret != ESP_OK)
    {
        return ret;
    }

    // inline bh1750_setup
    bh1750_resolution_t resolution = BH1750_RES_HIGH;

    if (resolution < BH1750_RES_LOW || resolution > BH1750_RES_HIGH2)
    {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t opcode = OPCODE_OT | OPCODE_HIGH; // One-time + High resolution

    // Send setup command
    ret = i2c_dev_write(&dev->i2c_dev, &opcode, 1);
    if (ret != ESP_OK)
    {
        return ret;
    }

    // Wait for measurement to complete (typical 120ms for high resolution)
    vTaskDelay(pdMS_TO_TICKS(180));

    // inline bh1750_read
    uint8_t buf[2];
    ret = i2c_dev_read(&dev->i2c_dev, buf, 2);
    if (ret != ESP_OK)
    {
        return ret;
    }

    uint16_t raw_value = (buf[0] << 8) | buf[1];
    *lux = (raw_value * 10) / 12; // convert to LUX

    return ESP_OK;
}

/* Private fuctions --------------------------------------------------------- */

/**
 * @brief Send command to BH1750 without taking mutex
 */
inline static esp_err_t send_command_nolock(bh1750_t *dev, uint8_t cmd)
{
    return i2c_dev_write(&dev->i2c_dev, &cmd, 1);
}

/**
 * @brief Send command to BH1750
 */
static esp_err_t send_command(bh1750_t *dev, uint8_t cmd)
{
    return send_command_nolock(dev, cmd);
}

/**
 * @brief Power on BH1750 device
 */
static esp_err_t bh1750_power_on(bh1750_t *dev)
{
    CHECK_ARG(&dev->i2c_dev);

    ESP_LOGD(TAG, "Powering on BH1750");
    esp_err_t ret = send_command(dev, OPCODE_POWER_ON);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "BH1750 powered on");
    }
    return ret;
}

/**
 * @brief Read raw light level from BH1750
 */
static esp_err_t bh1750_read(bh1750_t *dev, uint16_t *level)
{
    CHECK_ARG(dev && level);

    uint8_t buf[2];

    ESP_LOGD(TAG, "Reading light level from BH1750");

    CHECK(i2c_dev_read(&dev->i2c_dev, buf, 2));

    uint16_t raw_value = buf[0] << 8 | buf[1];
    *level = (raw_value * 10) / 12; // convert to LUX

    ESP_LOGI(TAG, "Light level: %d lx (raw: %d)", *level, raw_value);

    return ESP_OK;
}