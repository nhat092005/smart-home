/**
 * @file sh1106.c
 *
 * @brief SH1106 OLED Display Driver Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "sh1106.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "i2cdev.h"

/*Private defines macro ------------------------------------------------------*/

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

#define CHECK_ARG(VAL)                         \
    do                                         \
    {                                          \
        if (!(VAL))                            \
        {                                      \
            ESP_LOGE(TAG, "Invalid argument"); \
            return ESP_ERR_INVALID_ARG;        \
        }                                      \
    } while (0)

/* Private defines -----------------------------------------------------------*/

/* I2C Configuration */
#define I2C_FREQ_HZ I2C_MASTER_FREQ_HZ

/* SH1106 HARDWARE CONSTANTS */
#define SH1106_WIDTH 128
#define SH1106_HEIGHT 64
#define SH1106_PIXELS_PER_BYTE 8
#define SH1106_BUFFER_SIZE (SH1106_HEIGHT * SH1106_WIDTH / SH1106_PIXELS_PER_BYTE)
#define SH1106_I2C_ADDR_DEFAULT 0x3C

/* SH1106 COMMANDS */
#define SH1106_CMD_SET_CHARGE_PUMP_CTRL 0xAD
#define SH1106_CMD_SET_CHARGE_PUMP_ON 0x8B
#define SH1106_CMD_SET_SEGMENT_REMAP_INVERSE 0xA1
#define SH1106_CMD_SET_COM_SCAN_MODE_REVERSE 0xC8
#define SH1106_CMD_SET_DISPLAY_START_LINE 0x40
#define SH1106_CMD_SET_DISPLAY_OFFSET 0xD3
#define SH1106_CMD_SET_PADS_HW_CONFIG 0xDA
#define SH1106_CMD_SET_PADS_HW_ALTERNATIVE 0x12
#define SH1106_CMD_SET_MULTIPLEX_RATIO 0xA8
#define SH1106_CMD_SET_PAGE_ADDR 0xB0
#define SH1106_CMD_SET_COLUMN_ADDR_LOW 0x00
#define SH1106_CMD_SET_COLUMN_ADDR_HIGH 0x10
#define SH1106_CMD_SET_ENTIRE_DISPLAY_OFF 0xA4
#define SH1106_CMD_SET_DISPLAY_ON 0xAF
#define SH1106_CMD_SET_DISPLAY_OFF 0xAE

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "SH1106";

/* Private functions prototypes ----------------------------------------------*/

/**
 * @brief Send a command byte to SH1106
 *
 * @param[in] dev Device descriptor
 * @param[in] cmd Command byte to send
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t sh1106_write_cmd(sh1106_t *dev, uint8_t cmd);

/**
 * @brief Send command with parameter to SH1106
 *
 * @param[in] dev Device descriptor
 * @param[in] cmd Command byte
 * @param[in] param Parameter byte
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t sh1106_write_cmd_param(sh1106_t *dev, uint8_t cmd, uint8_t param);

/**
 * @brief Send data bytes to SH1106 (for pixel data)
 *
 * @param[in] dev Device descriptor
 * @param[in] data Pointer to data bytes
 * @param[in] len Number of data bytes
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t sh1106_write_data(sh1106_t *dev, const uint8_t *data, size_t len);

/**
 * @brief Initialize SH1106 display hardware
 *
 * @param[in] dev Device descriptor
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t sh1106_init_display(sh1106_t *dev);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize device descriptor
 */
esp_err_t sh1106_init_desc(sh1106_t *dev, uint8_t addr, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio)
{
    CHECK_ARG(dev);

    if (addr != SH1106_I2C_ADDR_DEFAULT)
    {
        ESP_LOGW(TAG, "Using non-default I2C address: 0x%02x (default is 0x3C)", addr);
    }

    ESP_LOGI(TAG, "Initializing SH1106 device descriptor");

    dev->i2c_dev.port = port;
    dev->i2c_dev.addr = addr;
    dev->i2c_dev.sda_io_num = sda_gpio;
    dev->i2c_dev.scl_io_num = scl_gpio;
    dev->i2c_dev.clk_speed = I2C_FREQ_HZ;

    esp_err_t res = i2c_dev_create_mutex(&dev->i2c_dev);
    if (res == ESP_OK)
    {
        ESP_LOGI(TAG, "SH1106 descriptor initialized on port %d (addr: 0x%02x, SDA: GPIO%d, SCL: GPIO%d)",
                 port, addr, sda_gpio, scl_gpio);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to initialize SH1106 descriptor: %s", esp_err_to_name(res));
    }

    return res;
}

/**
 * @brief Free device descriptor
 */
esp_err_t sh1106_free_desc(sh1106_t *dev)
{
    CHECK_ARG(dev);

    ESP_LOGD(TAG, "Freeing SH1106 descriptor");
    esp_err_t ret = i2c_dev_delete_mutex(&dev->i2c_dev);
    if (ret == ESP_OK)
    {
        ESP_LOGD(TAG, "SH1106 descriptor freed successfully");
    }
    return ret;
}

/**
 * @brief Initialize SH1106 display
 */
esp_err_t sh1106_init(sh1106_t *dev)
{
    CHECK_ARG(dev);

    ESP_LOGI(TAG, "Initializing SH1106 display hardware");

    // Clear internal buffer
    memset(dev->buffer, 0, SH1106_BUFFER_SIZE);

    // Initialize display hardware
    esp_err_t res = sh1106_init_display(dev);
    if (res == ESP_OK)
    {
        ESP_LOGI(TAG, "SH1106 display initialized successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to initialize SH1106 display: %s", esp_err_to_name(res));
    }

    return res;
}

void sh1106_set_pixel(sh1106_t *dev, int x, int y, uint8_t color)
{
    if (!dev || x < 0 || x >= SH1106_WIDTH || y < 0 || y >= SH1106_HEIGHT)
        return;

    int byte_index = x + (y / 8) * SH1106_WIDTH;
    int bit_index = y % 8;

    if (color)
    {
        dev->buffer[byte_index] |= (1 << bit_index);
    }
    else
    {
        dev->buffer[byte_index] &= ~(1 << bit_index);
    }
}

void sh1106_draw_horizontal_line(sh1106_t *dev, int y)
{
    if (!dev)
        return;

    for (int x = 0; x < SH1106_WIDTH; x++)
    {
        sh1106_set_pixel(dev, x, y, 1);
    }
}

void sh1106_clear_display(sh1106_t *dev)
{
    if (!dev)
        return;

    memset(dev->buffer, 0, SH1106_BUFFER_SIZE);
}

esp_err_t sh1106_update_display(sh1106_t *dev)
{
    CHECK_ARG(dev);

    // SH1106 has 8 pages (rows of 8 pixels each)
    for (int page = 0; page < 8; page++)
    {
        // Set page address
        CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_PAGE_ADDR | page));

        // Set column address (SH1106 has 132 columns, we use 128, start at column 2)
        CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_COLUMN_ADDR_LOW | 0x02));
        CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_COLUMN_ADDR_HIGH | 0x00));

        // Send data for this page
        CHECK(sh1106_write_data(dev, &dev->buffer[page * SH1106_WIDTH], SH1106_WIDTH));
    }

    return ESP_OK;
}

uint8_t *sh1106_get_buffer(sh1106_t *dev)
{
    return dev ? dev->buffer : NULL;
}

void sh1106_get_dimensions(int *width, int *height)
{
    if (width)
        *width = SH1106_WIDTH;
    if (height)
        *height = SH1106_HEIGHT;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Send a command byte to SH1106
 */
static esp_err_t sh1106_write_cmd(sh1106_t *dev, uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd}; // 0x00 = command mode (Co=0, D/C=0)
    return i2c_dev_write(&dev->i2c_dev, data, 2);
}

/**
 * @brief Send command with parameter to SH1106
 */
static esp_err_t sh1106_write_cmd_param(sh1106_t *dev, uint8_t cmd, uint8_t param)
{
    CHECK(sh1106_write_cmd(dev, cmd));
    return sh1106_write_cmd(dev, param);
}

/**
 * @brief Send data bytes to SH1106 (for pixel data)
 */
static esp_err_t sh1106_write_data(sh1106_t *dev, const uint8_t *data, size_t len)
{
    // Create buffer with data mode prefix (0x40 = data mode, Co=0, D/C=1)
    uint8_t *buffer = malloc(len + 1);
    if (!buffer)
        return ESP_ERR_NO_MEM;

    buffer[0] = 0x40; // Data mode
    memcpy(buffer + 1, data, len);

    esp_err_t ret = i2c_dev_write(&dev->i2c_dev, buffer, len + 1);
    free(buffer);
    return ret;
}

/**
 * @brief Initialize SH1106 display hardware
 */
static esp_err_t sh1106_init_display(sh1106_t *dev)
{
    // Enable charge pump (required for OLED)
    CHECK(sh1106_write_cmd_param(dev, SH1106_CMD_SET_CHARGE_PUMP_CTRL, SH1106_CMD_SET_CHARGE_PUMP_ON));

    // Set segment remap (mirror horizontally)
    CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_SEGMENT_REMAP_INVERSE));

    // Set COM scan direction (mirror vertically)
    CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_COM_SCAN_MODE_REVERSE));

    // Set display start line to 0
    CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_DISPLAY_START_LINE | 0x00));

    // Set display offset to 0
    CHECK(sh1106_write_cmd_param(dev, SH1106_CMD_SET_DISPLAY_OFFSET, 0x00));

    // Set COM pins hardware configuration
    CHECK(sh1106_write_cmd_param(dev, SH1106_CMD_SET_PADS_HW_CONFIG, SH1106_CMD_SET_PADS_HW_ALTERNATIVE));

    // Set multiplex ratio to 64
    CHECK(sh1106_write_cmd_param(dev, SH1106_CMD_SET_MULTIPLEX_RATIO, 0x3F));

    // Set cursor to (0, 0)
    CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_PAGE_ADDR | 0x00));
    CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_COLUMN_ADDR_LOW | 0x00));
    CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_COLUMN_ADDR_HIGH | 0x00));

    // Use RAM for display (not entire display ON)
    CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_ENTIRE_DISPLAY_OFF));

    // Turn on display
    CHECK(sh1106_write_cmd(dev, SH1106_CMD_SET_DISPLAY_ON));

    vTaskDelay(pdMS_TO_TICKS(100)); // Wait for display to stabilize

    ESP_LOGI(TAG, "SH1106 hardware initialized successfully");
    return ESP_OK;
}
