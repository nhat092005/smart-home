/**
 * @file task_display.c
 *
 * @brief Task Display Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "task_display.h"
#include "sensor_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>

/* Private defines -----------------------------------------------------------*/

/* Display layout constants */
#define DISPLAY_TIME_Y 4
#define DISPLAY_SEPARATOR1_Y 22
#define DISPLAY_SENSORS_Y 32
#define DISPLAY_SEPARATOR2_Y 44
#define DISPLAY_INFO_Y 52

/* Sensor X coordinates */
#define SENSOR_TEMP_X 6
#define SENSOR_HUM_X 50
#define SENSOR_LIGHT_X 94

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "TASK_DISPLAY";

/* Global device pointer */
static sh1106_t *display_device = NULL;

/* Font data */
// Font 5x7 for digits 0-9 and colon
static const uint8_t font_5x7[][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, //!< 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, //!< 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, //!< 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, //!< 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, //!< 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, //!< 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, //!< 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, //!< 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, //!< 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, //!< 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, //!< :
};

// Font 5x7 for letters A-Z
static const uint8_t font_5x7_alpha[][5] = {
    {0x7C, 0x12, 0x11, 0x12, 0x7C}, //!< A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, //!< B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, //!< C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, //!< D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, //!< E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, //!< F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, //!< G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, //!< H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, //!< I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, //!< J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, //!< K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, //!< L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, //!< M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, //!< N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, //!< O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, //!< P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, //!< Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, //!< R
    {0x46, 0x49, 0x49, 0x49, 0x31}, //!< S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, //!< T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, //!< U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, //!< V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, //!< W
    {0x63, 0x14, 0x08, 0x14, 0x63}, //!< X
    {0x07, 0x08, 0x70, 0x08, 0x07}, //!< Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, //!< Z
};

// Dot character
static const uint8_t font_dot[5] = {0x00, 0x60, 0x60, 0x00, 0x00};

/* Private functions prototypes ----------------------------------------------*/

/**
 * @brief Draw a single character on display
 *
 * @param[in] x X coordinate
 * @param[in] y Y coordinate
 * @param[in] c Character to draw
 * @param[in] size Character size multiplier (1, 2, 3...)
 */
static void draw_char(int x, int y, char c, uint8_t size);

/**
 * @brief Draw text string on display
 *
 * @param[in] x X coordinate
 * @param[in] y Y coordinate
 * @param[in] text Text string to draw
 * @param[in] size Character size multiplier
 */
static void draw_text(int x, int y, const char *text, uint8_t size);

/**
 * @brief Calculate text width in pixels
 *
 * @param[in] text Text string
 * @param[in] size Character size multiplier
 */
static int calculate_text_width(const char *text, uint8_t size);

/**
 * @brief Calculate X coordinate to center text
 *
 * @param[in] text Text string
 * @param[in] size Character size multiplier
 */
static int center_text_x(const char *text, uint8_t size);

/**
 * @brief Draw time display (HH:MM:SS)
 *
 * @param[in] hour Time hour (0-23)
 * @param[in] minute Time minute (0-59)
 * @param[in] second Time second (0-59)
 */
static void draw_time_display(int hour, int minute, int second);

/**
 * @brief Draw temperature value
 *
 * @param[in] temperature Temperature in °C
 */
static void draw_temperature(float temperature);

/**
 * @brief Draw humidity value
 *
 * @param[in] humidity Humidity in %
 */
static void draw_humidity(float humidity);

/**
 * @brief Draw light value
 *
 * @param[in] light Light level in lux
 */
static void draw_light(float light);

/**
 * @brief Draw version info
 *
 * @param[in] version Version string
 */
static void draw_version_info(const char *version);

/**
 * @brief Draw update interval info
 *
 * @param[in] interval Update interval in seconds
 */
static void draw_interval_info(int interval);

/**
 * @brief Draw horizontal separator lines
 *
 * @param[in] void
 */
static void draw_separators(void);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize display task and hardware
 */
esp_err_t task_display_init(void)
{
    ESP_LOGI(TAG, "Initializing display interface");

    // Get display device from sensor_manager
    display_device = sensor_manager_get_display_device();
    if (!display_device)
    {
        ESP_LOGW(TAG, "[DEMO MODE] SH1106 display not available - display functions will be no-op");
        ESP_LOGI(TAG, "[DEMO MODE] Display interface initialized in no-display mode");
        return ESP_OK; // Continue without display in demo mode
    }

    ESP_LOGI(TAG, "Got display device from sensor_manager");

    // Clear display on init
    sh1106_clear_display(display_device);
    esp_err_t ret = sh1106_update_display(display_device);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to update display: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Display interface initialized successfully");
    return ESP_OK;
}

/**
 * @brief Render complete UI with all elements
 */
void task_display_render_full_ui(const display_data_t *data)
{
    if (!data)
    {
        ESP_LOGE(TAG, "NULL data pointer");
        return;
    }

    if (!display_device)
    {
        ESP_LOGD(TAG, "[DEMO MODE] No display - skipping render");
        return;
    }

    sh1106_clear_display(display_device);

    draw_time_display(data->hour, data->minute, data->second);
    draw_separators();
    draw_temperature(data->temperature);
    draw_humidity(data->humidity);
    draw_light(data->light);
    draw_version_info(data->version);
    draw_interval_info(data->interval);

    sh1106_update_display(display_device);
}

/**
 * @brief Update only time display (faster partial update)
 */
void task_display_update_time(int hour, int minute, int second)
{
    if (!display_device)
    {
        ESP_LOGD(TAG, "[DEMO MODE] No display - skipping time update");
        return;
    }

    int display_width, display_height;
    sh1106_get_dimensions(&display_width, &display_height);

    // Clear only time area
    for (int y = 0; y < DISPLAY_SEPARATOR1_Y; y++)
    {
        for (int x = 0; x < display_width; x++)
        {
            sh1106_set_pixel(display_device, x, y, 0);
        }
    }

    draw_time_display(hour, minute, second);
    sh1106_update_display(display_device);
}

/**
 * @brief Show a centered message on display
 */
void task_display_show_message(const char *message)
{
    if (!message)
    {
        ESP_LOGE(TAG, "NULL message pointer");
        return;
    }

    if (!display_device)
    {
        ESP_LOGD(TAG, "[DEMO MODE] No display - message: %s", message);
        return;
    }

    sh1106_clear_display(display_device);

    int x = center_text_x(message, 1);
    int y = 28; // Center vertically
    draw_text(x, y, message, 1);

    sh1106_update_display(display_device);
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Draw text string on display
 */
static void draw_char(int x, int y, char c, uint8_t size)
{
    const uint8_t *font_data = NULL;

    if (c >= '0' && c <= '9')
    {
        font_data = font_5x7[c - '0'];
    }
    else if (c == ':')
    {
        font_data = font_5x7[10];
    }
    else if (c >= 'A' && c <= 'Z')
    {
        font_data = font_5x7_alpha[c - 'A'];
    }
    else if (c >= 'a' && c <= 'z')
    {
        font_data = font_5x7_alpha[c - 'a'];
    }
    else if (c == '.')
    {
        font_data = font_dot;
    }
    else if (c == ' ')
    {
        return; // Space - just skip
    }
    else
    {
        return; // Unsupported character
    }

    // Render character
    for (int i = 0; i < 5; i++)
    {
        uint8_t line = font_data[i];
        for (int j = 0; j < 7; j++)
        {
            if (line & (1 << j))
            {
                for (int sx = 0; sx < size; sx++)
                {
                    for (int sy = 0; sy < size; sy++)
                    {
                        sh1106_set_pixel(display_device, x + i * size + sx, y + j * size + sy, 1);
                    }
                }
            }
        }
    }
}

/**
 * @brief Draw text string on display
 */
static void draw_text(int x, int y, const char *text, uint8_t size)
{
    int cursor_x = x;
    while (*text)
    {
        draw_char(cursor_x, y, *text, size);
        cursor_x += 6 * size; // 5 pixels + 1 pixel spacing
        text++;
    }
}

/**
 * @brief Calculate text width in pixels
 */
static int calculate_text_width(const char *text, uint8_t size)
{
    return strlen(text) * 6 * size;
}

/**
 * @brief Calculate X coordinate to center text
 */
static int center_text_x(const char *text, uint8_t size)
{
    int width = calculate_text_width(text, size);
    int display_width, display_height;
    sh1106_get_dimensions(&display_width, &display_height);
    return (display_width - width) / 2;
}

/**
 * @brief Draw time display (HH:MM:SS)
 */
static void draw_time_display(int hour, int minute, int second)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hour, minute, second);

    int x = center_text_x(buffer, 2);
    draw_text(x, DISPLAY_TIME_Y, buffer, 2);
}

/**
 * @brief Draw temperature value
 */
static void draw_temperature(float temperature)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.2f", temperature);
    draw_text(SENSOR_TEMP_X, DISPLAY_SENSORS_Y, buffer, 1);
}

/**
 * @brief Draw humidity value
 */
static void draw_humidity(float humidity)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.2f", humidity);
    draw_text(SENSOR_HUM_X, DISPLAY_SENSORS_Y, buffer, 1);
}

/**
 * @brief Draw light value
 */
static void draw_light(float light)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.0f", light);
    draw_text(SENSOR_LIGHT_X, DISPLAY_SENSORS_Y, buffer, 1);
}

/**
 * @brief Draw version info
 */
static void draw_version_info(const char *version)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "VER:%s", version);
    draw_text(5, DISPLAY_INFO_Y, buffer, 1);
}

/**
 * @brief Draw update interval info
 */
static void draw_interval_info(int interval)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "INT:%ds", interval);
    draw_text(75, DISPLAY_INFO_Y, buffer, 1);
}

/**
 * @brief Draw horizontal separator lines
 */
static void draw_separators(void)
{
    sh1106_draw_horizontal_line(display_device, DISPLAY_SEPARATOR1_Y);
    sh1106_draw_horizontal_line(display_device, DISPLAY_SEPARATOR2_Y);
}