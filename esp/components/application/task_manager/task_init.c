/**
 * @file task_init.c
 *
 * @brief Task Init Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "task_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_manager.h"

/* PRIVATE VARIABLES --------------------------------------------------------*/

static const char *TAG = "TASK_INIT";

/* External button callback functions (implemented in main.c) ---------------*/

extern void task_button_mode_pressed(button_type_t button);
extern void task_button_wifi_pressed(button_type_t button);
extern void task_button_light_pressed(button_type_t button);
extern void task_button_fan_pressed(button_type_t button);

extern void wifi_event_callback(wifi_manager_event_t event, void *data);
extern void task_mode_change_event_callback(device_mode_t old_mode, device_mode_t new_mode);

/* Private functions Prototype -----------------------------------------------*/

/**
 * @brief Initialize NVS
 */
static void task_init_nvs(void);

/**
 * @brief Initialize hardware components
 */
static void task_init_status_led(void);

/**
 * @brief Initialize hardware components
 */
static void task_init_button(void);

/**
 * @brief Initialize hardware protocol components
 */
static void task_init_hardware_protocol(void);

/**
 * @brief Initialize WiFi components
 */
static void task_init_wifi(void);

/**
 * @brief Initialize Mode Manager
 */
static void task_init_mode_manager(void);

/* Exported functions --------------------------------------------------------*/

void task_init(void)
{
    // Initialize NVS
    task_init_nvs();

    // Initialize status LEDs
    task_init_status_led();

    // Initialize button handler
    task_init_button();

    // Initialize hardware protocol components
    task_init_hardware_protocol();

    // Initialize WiFi components
    task_init_wifi();

    // Initialize Mode Manager
    task_init_mode_manager();
}

/* Private functions --------------------------------------------------------*/

/**
 * @brief Initialize NVS
 */
static void task_init_nvs(void)
{
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS initialized");
}

/**
 * @brief Initialize hardware components
 */
static void task_init_status_led(void)
{
    // Initialize Status LEDs
    status_led_init();

    // Initialize Task Status LED Polling
    task_status_set_init();
}

/**
 * @brief Initialize hardware components
 */
static void task_init_button(void)
{
    // Initialize Device Control
    device_control_init();

    // Initialize Button Handler
    button_handler_init();

    // Set button callbacks
    button_handler_set_callback(BUTTON_MODE, task_button_mode_pressed);
    button_handler_set_callback(BUTTON_WIFI, task_button_wifi_pressed);
    button_handler_set_callback(BUTTON_LIGHT, task_button_light_pressed);
    button_handler_set_callback(BUTTON_FAN, task_button_fan_pressed);
}

/**
 * @brief Initialize hardware protocol components
 */
static void task_init_hardware_protocol(void)
{
    // Initialize Sensor Manager
    sensor_manager_init(I2C_MASTER_SDA_PIN, I2C_MASTER_SCL_PIN);
}

/**
 * @brief Initialize WiFi components
 */
static void task_init_wifi(void)
{
    esp_err_t ret;

    // Initialize WiFi Manager
    ret = wifi_manager_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "WiFi Manager initialize failed: %s", esp_err_to_name(ret));
    }

    // Register WiFi event callback
    wifi_manager_register_callback(task_wifi_event_callback);

    // Start WiFi Manager (auto-provisioning if not configured)
    ret = wifi_manager_start();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "WiFi Manager start failed: %s", esp_err_to_name(ret));
    }

    ret = task_wifi_set_wifi_connecting_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Task WiFi connecting init failed: %s", esp_err_to_name(ret));
    }

    // Check provisioning status
    if (wifi_manager_is_provisioned())
    {
        ESP_LOGI(TAG, "WiFi credentials found, connecting to AP...");
    }
    else
    {
        ESP_LOGI(TAG, "No WiFi credentials found");
        ESP_LOGI(TAG, "Open browser: http://192.168.4.1");
    }
}

/**
 * @brief Initialize Mode Manager
 */
static void task_init_mode_manager(void)
{
    esp_err_t ret;

    // Initialize Mode Manager
    ret = mode_manager_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Mode Manager initialize failed: %s", esp_err_to_name(ret));
    }

    mode_manager_register_change_callback(task_mode_change_event_callback);
}