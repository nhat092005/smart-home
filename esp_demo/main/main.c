/**
 * @file main.c
 *
 * @brief Smart Home Main Application
 */

/* Includes ------------------------------------------------------------------*/

#include "main.h"

/* Private defines ----------------------------------------------------------*/

static const char *TAG = "MAIN";

/* Private variables --------------------------------------------------------*/

/* Private functions prototypes ----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  The application entry point.
 */
void app_main(void)
{
    ESP_LOGI(TAG, "Smart Home Application");

    // Initialize system components
    task_init();

    // Main loop
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Private functions ---------------------------------------------------------*/
