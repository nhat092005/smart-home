/**
 * @file device_control.c
 *
 * @brief Device Control Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "device_control.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* Private defines -----------------------------------------------------------*/

static const char *TAG = "DEVICE_CONTROL";

/* Private types -------------------------------------------------------------*/
/**
 * @brief Device structure
 */
typedef struct
{
    gpio_num_t pin;       //!< GPIO pin number
    const char *name;     //!< Device name
    device_state_t state; //!< Current state
} device_t;

/* Private variables ---------------------------------------------------------*/

// Device configuration table
static device_t devices[DEVICE_MAX] = {
    {DEVICE_FAN_PIN, "FAN", DEVICE_OFF},     //!< Fan device
    {DEVICE_LIGHT_PIN, "LIGHT", DEVICE_OFF}, //!< Light device
    {DEVICE_AC_PIN, "AC", DEVICE_OFF}        //!< AC device
};
static bool initialized = false;
static SemaphoreHandle_t mutex = NULL;

/* Private functions prototypes ----------------------------------------------*/

/**
 * @brief Get GPIO level for given device state
 *
 * @param[in] state Device state
 *
 * @return GPIO level corresponding to the state
 */
static inline uint32_t get_gpio_level(device_state_t state);

/**
 * @brief Set device state (ON or OFF)
 *
 * @param[in] device Device type (DEVICE_FAN or DEVICE_LIGHT)
 * @param[in] state Device state (DEVICE_ON or DEVICE_OFF)
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t device_control_set(device_type_t device, device_state_t state);

/* Exported functions ---------------------------------------------------------*/

/**
 * @brief Initialize device control
 */
esp_err_t device_control_init(void)
{
    if (initialized)
    {
        return ESP_OK;
    }

    mutex = xSemaphoreCreateMutex();
    if (!mutex)
    {
        return ESP_FAIL;
    }

    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    for (int i = 0; i < DEVICE_MAX; i++)
    {
        io_conf.pin_bit_mask = (1ULL << devices[i].pin);
        if (gpio_config(&io_conf) != ESP_OK)
        {
            // Cleanup already configured GPIOs
            for (int j = 0; j < i; j++)
            {
                gpio_reset_pin(devices[j].pin);
            }
            vSemaphoreDelete(mutex);
            mutex = NULL;
            return ESP_FAIL;
        }

        gpio_set_level(devices[i].pin, get_gpio_level(DEVICE_OFF));
        ESP_LOGI(TAG, "%s on GPIO%d initialized", devices[i].name, devices[i].pin);
    }

    initialized = true;
    return ESP_OK;
}

/**
 * @brief Set device state
 */
esp_err_t device_control_set_state(device_type_t device, device_state_t state)
{
    return device_control_set(device, state);
}

/**
 * @brief Get current device state
 */
esp_err_t device_control_get_state(device_type_t device, device_state_t *state)
{
    if (!initialized || device >= DEVICE_MAX || !state)
    {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(mutex, portMAX_DELAY);
    *state = devices[device].state;
    xSemaphoreGive(mutex);

    return ESP_OK;
}

/**
 * @brief Toggle device state
 */
esp_err_t device_control_toggle(device_type_t device)
{
    if (!initialized || device >= DEVICE_MAX)
    {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(mutex, portMAX_DELAY);
    device_state_t new_state = (devices[device].state == DEVICE_ON) ? DEVICE_OFF : DEVICE_ON;
    gpio_set_level(devices[device].pin, get_gpio_level(new_state));
    devices[device].state = new_state;
    xSemaphoreGive(mutex);

    return ESP_OK;
}

/**
 * @brief Deinitialize device control system
 */
esp_err_t device_control_deinit(void)
{
    if (!initialized)
    {
        return ESP_OK;
    }

    xSemaphoreTake(mutex, portMAX_DELAY);
    initialized = false; // Set inside mutex to prevent race condition

    for (int i = 0; i < DEVICE_MAX; i++)
    {
        gpio_set_level(devices[i].pin, get_gpio_level(DEVICE_OFF));
        gpio_reset_pin(devices[i].pin);
    }
    xSemaphoreGive(mutex);

    vSemaphoreDelete(mutex);
    mutex = NULL;

    return ESP_OK;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Get GPIO level for given device state
 */
static inline uint32_t get_gpio_level(device_state_t state)
{
    return (state == DEVICE_ON) ? DEVICE_ACTIVE_LEVEL : (1 - DEVICE_ACTIVE_LEVEL);
}

/**
 * @brief Set device state (ON or OFF)
 */
static esp_err_t device_control_set(device_type_t device, device_state_t state)
{
    if (!initialized || device >= DEVICE_MAX)
    {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(mutex, portMAX_DELAY);
    gpio_set_level(devices[device].pin, get_gpio_level(state));
    devices[device].state = state;
    xSemaphoreGive(mutex);

    return ESP_OK;
}