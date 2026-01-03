/**
 * @file button_handler.c
 *
 * @brief Button Handler Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "button_handler.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Private defines -----------------------------------------------------------*/
static const char *TAG = "BUTTON_HANDLER";

/* Private types -------------------------------------------------------------*/

/**
 * @brief Button structure
 */
typedef struct
{
    gpio_num_t pin;                      //!< GPIO pin number
    const char *name;                    //!< Button name
    volatile button_callback_t callback; //!< Callback function
    volatile bool pressed;               //!< Pressed state
    uint8_t debounce_count;              //!< Debounce counter
} button_t;

/* Private variables ---------------------------------------------------------*/

// Button configuration table
static button_t buttons[BUTTON_MAX] = {
    {BUTTON_MODE_PIN, "MODE", NULL, false, 0},   //!< Mode button
    {BUTTON_WIFI_PIN, "WIFI", NULL, false, 0},   //!< WiFi button
    {BUTTON_LIGHT_PIN, "LIGHT", NULL, false, 0}, //!< Light button
    {BUTTON_FAN_PIN, "FAN", NULL, false, 0},     //!< Fan button
    {BUTTON_AC_PIN, "AC", NULL, false, 0}        //!< AC button
};

static volatile bool initialized = false;
static TaskHandle_t poll_task_handle = NULL;

/* Private function prototypes ------------------------------------------------*/

/**
 * @brief Button polling task
 *
 * @param[in] arg Task argument (unused)
 */
static void button_poll_task(void *arg);

/* Exported functions ---------------------------------------------------------*/

/**

 * @brief Initialize button handler
 */
esp_err_t button_handler_init(void)
{
    if (initialized)
    {
        return ESP_OK;
    }

    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    for (int i = 0; i < BUTTON_MAX; i++)
    {
        io_conf.pin_bit_mask = (1ULL << buttons[i].pin);
        if (gpio_config(&io_conf) != ESP_OK)
        {
            // Cleanup already configured GPIOs
            for (int j = 0; j < i; j++)
            {
                gpio_reset_pin(buttons[j].pin);
            }
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "%s button on GPIO%d initialized", buttons[i].name, buttons[i].pin);
    }

    initialized = true;

    // Create polling task after setting initialized flag
    if (xTaskCreate(button_poll_task, "button_poll", 2048, NULL, 5, &poll_task_handle) != pdPASS)
    {
        initialized = false;
        for (int i = 0; i < BUTTON_MAX; i++)
        {
            gpio_reset_pin(buttons[i].pin);
        }
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Set callback for specific button
 */
esp_err_t button_handler_set_callback(button_type_t button, button_callback_t callback)
{
    if (!initialized || button >= BUTTON_MAX)
    {
        return ESP_ERR_INVALID_ARG;
    }

    // Safe to set callback - polling task only reads it
    buttons[button].callback = callback;
    return ESP_OK;
}

/**
 * @brief Check if button is currently pressed
 */
bool button_handler_is_pressed(button_type_t button)
{
    if (button >= BUTTON_MAX)
    {
        return false;
    }
    return buttons[button].pressed;
}

/**
 * @brief Deinitialize button handler
 */
esp_err_t button_handler_deinit(void)
{
    if (!initialized)
    {
        return ESP_OK;
    }

    // Set flag first to stop task loop
    initialized = false;

    // Give task time to exit gracefully
    if (poll_task_handle)
    {
        // Wait max 100ms for task to finish
        for (int i = 0; i < 10; i++)
        {
            if (eTaskGetState(poll_task_handle) == eDeleted)
            {
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        poll_task_handle = NULL;
    }

    for (int i = 0; i < BUTTON_MAX; i++)
    {
        gpio_reset_pin(buttons[i].pin);
        buttons[i].pressed = false;
        buttons[i].debounce_count = 0;
    }

    return ESP_OK;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Button polling task
 */
static void button_poll_task(void *arg)
{
    const uint8_t debounce_threshold = DEBOUNCE_TIME_MS / BUTTON_POLL_INTERVAL_MS;

    // Subscribe to task watchdog
    esp_task_wdt_add(NULL);

    ESP_LOGI(TAG, "Button poll task subscribed to watchdog");

    while (initialized)
    {
        // Reset watchdog at start of each iteration
        esp_task_wdt_reset();

        for (int i = 0; i < BUTTON_MAX; i++)
        {
            bool current = (gpio_get_level(buttons[i].pin) == 0); // Active low

            if (current)
            {
                // Button is pressed
                if (buttons[i].debounce_count < debounce_threshold)
                {
                    buttons[i].debounce_count++;
                }

                // Stable pressed state reached
                if (buttons[i].debounce_count >= debounce_threshold && !buttons[i].pressed)
                {
                    buttons[i].pressed = true;
                    ESP_LOGI(TAG, "%s button pressed", buttons[i].name);

                    if (buttons[i].callback)
                    {
                        buttons[i].callback(i);
                    }
                }
            }
            else
            {
                // Button is released
                if (buttons[i].debounce_count > 0)
                {
                    buttons[i].debounce_count--;
                }

                // Stable released state reached
                if (buttons[i].debounce_count == 0 && buttons[i].pressed)
                {
                    buttons[i].pressed = false;
                    ESP_LOGD(TAG, "%s button released", buttons[i].name);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_INTERVAL_MS));
    }

    // Unsubscribe from watchdog before deleting task
    esp_task_wdt_delete(NULL);
    vTaskDelete(NULL);
}