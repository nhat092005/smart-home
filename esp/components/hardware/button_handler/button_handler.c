/**
 * @file button_handler.c
 *
 * @brief Button Handler Component 
 */

/* Includes ------------------------------------------------------------------*/
#include "button_handler.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Private defines -----------------------------------------------------------*/
#define MS_TO_US 1000

/* Private types -------------------------------------------------------------*/
/**
 * @brief Button state structure
 */
typedef struct
{
    gpio_num_t gpio_num;
    button_press_callback_t callback;
    esp_timer_handle_t debounce_timer;
    bool is_pressed;
    bool last_state;
    bool initialized;
    uint32_t debounce_ms;
} button_state_t;

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "BUTTON_HANDLER";
static button_state_t g_buttons[BUTTON_MAX_HANDLERS];
static uint8_t g_button_count = 0;

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief Debounce timer callback
 */
static void debounce_timer_callback(void *arg);

/**
 * @brief GPIO ISR handler for button press/release
 */
static void IRAM_ATTR button_isr_handler(void *arg);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize button handler with default configuration
 */
esp_err_t button_handler_init(gpio_num_t gpio_num, button_press_callback_t callback)
{
    if (g_button_count >= BUTTON_MAX_HANDLERS)
    {
        ESP_LOGE(TAG, "Maximum button handlers reached");
        return ESP_ERR_NO_MEM;
    }

    if (callback == NULL)
    {
        ESP_LOGE(TAG, "Callback is NULL for GPIO_%d", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "Configuring button on GPIO_%d", gpio_num);

    // Configure GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpio_num),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };

    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure GPIO_%d: %s", gpio_num, esp_err_to_name(ret));
        return ret;
    }

    // Create debounce timer
    button_state_t *button = &g_buttons[g_button_count];
    button->gpio_num = gpio_num;
    button->callback = callback;
    button->is_pressed = false;
    button->last_state = true; // Active low, so idle is high
    button->debounce_ms = DEBOUNCE_TIME_MS;
    button->initialized = true;

    const esp_timer_create_args_t timer_args = {
        .callback = debounce_timer_callback,
        .arg = button,
        .name = "btn_debounce",
        .skip_unhandled_events = true,
    };

    ret = esp_timer_create(&timer_args, &button->debounce_timer);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create timer for GPIO_%d: %s", gpio_num, esp_err_to_name(ret));
        button->initialized = false;
        return ret;
    }

    // Install ISR service if not already installed
    static bool isr_service_installed = false;
    if (!isr_service_installed)
    {
        ret = gpio_install_isr_service(0);
        if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
        {
            ESP_LOGE(TAG, "Failed to install ISR service: %s", esp_err_to_name(ret));
            esp_timer_delete(button->debounce_timer);
            button->initialized = false;
            return ret;
        }
        isr_service_installed = true;
    }

    // Add ISR handler
    ret = gpio_isr_handler_add(gpio_num, button_isr_handler, button);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add ISR handler for GPIO_%d: %s", gpio_num, esp_err_to_name(ret));
        esp_timer_delete(button->debounce_timer);
        button->initialized = false;
        return ret;
    }

    g_button_count++;
    ESP_LOGI(TAG, "Button GPIO_%d initialized successfully", gpio_num);
    return ESP_OK;
}

/**
 * @brief Deinitialize button handler and free resources
 */
esp_err_t button_handler_deinit(void)
{
    if (g_button_count == 0)
    {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Deinitializing button handler");

    for (uint8_t i = 0; i < g_button_count; i++)
    {
        if (g_buttons[i].initialized)
        {
            ESP_LOGD(TAG, "Deinitializing button GPIO_%d", g_buttons[i].gpio_num);

            gpio_isr_handler_remove(g_buttons[i].gpio_num);

            if (g_buttons[i].debounce_timer != NULL)
            {
                esp_timer_stop(g_buttons[i].debounce_timer);
                esp_timer_delete(g_buttons[i].debounce_timer);
            }

            g_buttons[i].initialized = false;
        }
    }

    g_button_count = 0;
    ESP_LOGI(TAG, "Button handler deinitialized");
    return ESP_OK;
}

/**
 * @brief Get current button state
 */
bool button_handler_is_pressed(gpio_num_t gpio_num)
{
    for (uint8_t i = 0; i < g_button_count; i++)
    {
        if (g_buttons[i].gpio_num == gpio_num && g_buttons[i].initialized)
        {
            return g_buttons[i].is_pressed;
        }
    }
    return false;
}

/* Private functions --------------------------------------------------------*/
/**
 * @brief GPIO ISR handler for button press/release
 */
static void IRAM_ATTR button_isr_handler(void *arg)
{
    button_state_t *button = (button_state_t *)arg;

    // Start debounce timer (one-shot)
    esp_timer_start_once(button->debounce_timer, button->debounce_ms * MS_TO_US);
}

/**
 * @brief Debounce timer callback
 */
static void debounce_timer_callback(void *arg)
{
    button_state_t *button = (button_state_t *)arg;

    // Read current GPIO state (active low: 0 = pressed, 1 = released)
    int current_level = gpio_get_level(button->gpio_num);
    bool current_pressed = (current_level == 0);

    // Check if state changed from last stable state
    if (current_pressed != button->last_state && current_pressed != button->is_pressed)
    {
        button->is_pressed = current_pressed;

        // If button was pressed (falling edge after debounce)
        if (current_pressed)
        {
            ESP_LOGI(TAG, "Button GPIO_%d pressed", button->gpio_num);

            if (button->callback)
            {
                ESP_LOGD(TAG, "Invoking callback for GPIO_%d", button->gpio_num);
                button->callback(button->gpio_num);
            }
        }
        else
        {
            ESP_LOGD(TAG, "Button GPIO_%d released", button->gpio_num);
        }

        button->last_state = current_pressed;
    }
}