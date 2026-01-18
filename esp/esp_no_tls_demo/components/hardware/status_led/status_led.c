/**
 * @file status_led.c
 *
 * @brief Status LED Control Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "status_led.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* Private defines -----------------------------------------------------------*/

static const char *TAG = "STATUS_LED";

/* Private types -------------------------------------------------------------*/

/**
 * @brief LED structure
 */
typedef struct
{
    gpio_num_t pin;    //!< GPIO pin number
    const char *name;  //!< LED name
    led_state_t state; //!< Current LED state
} led_t;

/* Private variables ---------------------------------------------------------*/
// LED configuration table
static led_t leds[LED_MAX] = {
    {LED_DEVICE_PIN, "DEVICE", LED_OFF}, //!< Device LED
    {LED_WIFI_PIN, "WIFI", LED_OFF},     //!< WiFi LED
    {LED_MQTT_PIN, "MQTT", LED_OFF}};    //!< MQTT LED
static bool initialized = false;
static SemaphoreHandle_t led_mutex = NULL;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Convert LED state to GPIO level
 *
 * @param[in] state LED state (LED_ON or LED_OFF)
 *
 * @return GPIO level corresponding to the LED state
 */
static inline int state_to_gpio_level(led_state_t state);

/* Exported functions ---------------------------------------------------------*/

/**
 * @brief Initialize all status LEDs
 */
esp_err_t status_led_init(void)
{
    if (initialized)
    {
        return ESP_OK;
    }

    // Create mutex for thread safety
    led_mutex = xSemaphoreCreateMutex();
    if (!led_mutex)
    {
        ESP_LOGE(TAG, "Failed to create mutex");
        return ESP_FAIL;
    }

    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    for (int i = 0; i < LED_MAX; i++)
    {
        io_conf.pin_bit_mask = (1ULL << leds[i].pin);
        if (gpio_config(&io_conf) != ESP_OK)
        {
            // Cleanup already configured GPIOs
            for (int j = 0; j < i; j++)
            {
                gpio_reset_pin(leds[j].pin);
            }
            if (led_mutex)
            {
                vSemaphoreDelete(led_mutex);
                led_mutex = NULL;
            }
            return ESP_FAIL;
        }

        gpio_set_level(leds[i].pin, state_to_gpio_level(LED_OFF));
        ESP_LOGI(TAG, "%s LED on GPIO%d initialized", leds[i].name, leds[i].pin);
    }

    initialized = true;
    return ESP_OK;
}

/**
 * @brief Set LED state
 */
esp_err_t status_led_set_state(led_type_t led, led_state_t state)
{
    if (!initialized || led >= LED_MAX)
    {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(led_mutex, portMAX_DELAY);
    gpio_set_level(leds[led].pin, state_to_gpio_level(state));
    leds[led].state = state;
    xSemaphoreGive(led_mutex);

    return ESP_OK;
}

/**
 * @brief Get LED state
 */
esp_err_t status_led_get_state(led_type_t led, led_state_t *state)
{
    if (!initialized || led >= LED_MAX || state == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(led_mutex, portMAX_DELAY);
    *state = leds[led].state;
    xSemaphoreGive(led_mutex);

    return ESP_OK;
}

/**
 * @brief Toggle LED state
 */
esp_err_t status_led_toggle(led_type_t led)
{
    if (!initialized || led >= LED_MAX)
    {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(led_mutex, portMAX_DELAY);
    led_state_t new_state = (leds[led].state == LED_ON) ? LED_OFF : LED_ON;
    gpio_set_level(leds[led].pin, state_to_gpio_level(new_state));
    leds[led].state = new_state;
    xSemaphoreGive(led_mutex);

    return ESP_OK;
}

/**
 * @brief Deinitialize all status LEDs
 */
esp_err_t status_led_deinit(void)
{
    if (!initialized)
    {
        return ESP_OK;
    }

    for (int i = 0; i < LED_MAX; i++)
    {
        gpio_set_level(leds[i].pin, state_to_gpio_level(LED_OFF));
        gpio_reset_pin(leds[i].pin);
    }

    if (led_mutex)
    {
        vSemaphoreDelete(led_mutex);
        led_mutex = NULL;
    }

    initialized = false;
    return ESP_OK;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Convert LED state to GPIO level
 */
static inline int state_to_gpio_level(led_state_t state)
{
    return (state == LED_ON) ? LED_ACTIVE_LEVEL : (1 - LED_ACTIVE_LEVEL);
}
