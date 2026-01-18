/**
 * @file task_display.h
 *
 * @brief Task Display Header API
 */

#ifndef TASK_DISPLAY_H
#define TASK_DISPLAY_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "esp_err.h"

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Complete display data structure
 */
typedef struct
{
    int hour;            //!< Time hour (0-23)
    int minute;          //!< Time minute (0-59)
    int second;          //!< Time second (0-59)
    float temperature;   //!< Temperature in °C
    float humidity;      //!< Humidity in %
    float light;         //!< Light level in lux
    const char *version; //!< Firmware version string
    int interval;        //!< Update interval in seconds
} display_data_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize display task and hardware
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t task_display_init(void);

/**
 * @brief Render complete UI with all elements
 *
 * @param[in] data Pointer to display data structure
 */
void task_display_render_full_ui(const display_data_t *data);

/**
 * @brief Update only time display (faster partial update)
 *
 * @param[in] hour Time hour (0-23)
 * @param[in] minute Time minute (0-59)
 * @param[in] second Time second (0-59)
 */
void task_display_update_time(int hour, int minute, int second);

/**
 * @brief Show a centered message on display
 *
 * @param[in] message Message string to display
 */
void task_display_show_message(const char *message);

#endif /* TASK_DISPLAY_H */
