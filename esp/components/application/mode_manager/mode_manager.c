/**
 * @file mode_manager.c
 *
 * @brief Mode Manager Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "mode_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>

/* Private defines  ----------------------------------------------------------*/

// NVS storage
#define NVS_NAMESPACE "mode_config"
#define NVS_KEY_MODE "device_mode"

// Default mode
#define DEFAULT_MODE MODE_ON

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "MODE_MANAGER";

static device_mode_t current_mode = DEFAULT_MODE;
static mode_change_callback_t change_callback = NULL;
static bool initialized = false;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Load mode from NVS
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t mode_manager_load_mode_from_nvs(void);

/**
 * @brief Save mode to NVS
 *
 * @param[in] mode Mode to save
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t mode_manager_save_mode_to_nvs(device_mode_t mode);

/**
 * @brief Get mode name as string
 *
 * @param[in] mode Mode to convert
 *
 * @return String representation ("OFF" or "ON")
 */
static const char *mode_manager_get_mode_name(device_mode_t mode);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize mode manager
 */
esp_err_t mode_manager_init(void)
{
    if (initialized)
    {
        ESP_LOGW(TAG, "Mode manager already initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing mode manager");

    esp_err_t ret = mode_manager_load_mode_from_nvs();
    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to load mode, using default");
        current_mode = DEFAULT_MODE;
    }

    initialized = true;

    if (current_mode == MODE_ON)
    {
        isModeON = true;
    }
    else
    {
        isModeON = false;
    }

    ESP_LOGI(TAG, "Mode manager initialized successfully, current mode: %s",
             mode_manager_get_mode_name(current_mode));

    return ESP_OK;
}

/**
 * @brief Set device mode
 */
esp_err_t mode_manager_set_mode(device_mode_t mode)
{
    if (!initialized)
    {
        ESP_LOGE(TAG, "Mode manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (mode != MODE_OFF && mode != MODE_ON)
    {
        ESP_LOGE(TAG, "Invalid mode: %d", mode);
        return ESP_ERR_INVALID_ARG;
    }

    if (current_mode == mode)
    {
        isModeON = (mode == MODE_ON) ? true : false;
        ESP_LOGI(TAG, "Mode already set to: %s", mode_manager_get_mode_name(mode));
        return ESP_OK;
    }

    device_mode_t old_mode = current_mode;
    current_mode = mode;

    isModeON = (current_mode == MODE_ON) ? true : false;

    ESP_LOGI(TAG, "Mode changed from %s to %s",
             mode_manager_get_mode_name(old_mode),
             mode_manager_get_mode_name(current_mode));

    esp_err_t ret = mode_manager_save_mode_to_nvs(mode);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to save mode to NVS");
    }

    if (change_callback != NULL)
    {
        change_callback(old_mode, current_mode);
    }

    return ret;
}

esp_err_t mode_manager_toggle_mode(void)
{
    if (!initialized)
    {
        ESP_LOGE(TAG, "Mode manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    device_mode_t new_mode = (current_mode == MODE_ON) ? MODE_OFF : MODE_ON;

    return mode_manager_set_mode(new_mode);
}

/**
 * @brief Get current device mode
 */
device_mode_t mode_manager_get_mode(void)
{
    return current_mode;
}

/**
 * @brief Register mode change callback
 */
void mode_manager_register_change_callback(mode_change_callback_t callback)
{
    change_callback = callback;
    ESP_LOGI(TAG, "Mode change callback registered");
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Load mode from NVS
 */
static esp_err_t mode_manager_load_mode_from_nvs(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);

    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Cannot open NVS (%s), using default mode: %s",
                 esp_err_to_name(ret), mode_manager_get_mode_name(DEFAULT_MODE));
        return ESP_OK;
    }

    uint8_t mode_val = DEFAULT_MODE;
    ret = nvs_get_u8(nvs_handle, NVS_KEY_MODE, &mode_val);
    nvs_close(nvs_handle);

    if (ret == ESP_OK)
    {
        current_mode = (device_mode_t)mode_val;
        ESP_LOGI(TAG, "Loaded mode: %s", mode_manager_get_mode_name(current_mode));
    }
    else
    {
        ESP_LOGW(TAG, "Cannot read mode (%s), using default: %s",
                 esp_err_to_name(ret), mode_manager_get_mode_name(DEFAULT_MODE));
        ret = ESP_OK;
    }

    return ret;
}

/**
 * @brief Save mode to NVS
 */
static esp_err_t mode_manager_save_mode_to_nvs(device_mode_t mode)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to open NVS for writing: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_set_u8(nvs_handle, NVS_KEY_MODE, (uint8_t)mode);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write mode to NVS: %s", esp_err_to_name(ret));
        nvs_close(nvs_handle);
        return ret;
    }

    ret = nvs_commit(nvs_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to commit NVS: %s", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Mode saved to NVS: %s", mode_manager_get_mode_name(mode));
    }

    nvs_close(nvs_handle);
    return ret;
}

/**
 * @brief Get mode name as string
 */
static const char *mode_manager_get_mode_name(device_mode_t mode)
{
    switch (mode)
    {
    case MODE_OFF:
        return "OFF";
    case MODE_ON:
        return "ON";
    default:
        return "UNKNOWN";
    }
}