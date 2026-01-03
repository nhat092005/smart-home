/**
 * @file task_wifi.c
 *
 * @brief Task WiFi Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "task_wifi.h"
#include "task_manager.h"
#include "mqtt_manager.h"
#include "esp_log.h"

/* PRIVATE VARIABLES --------------------------------------------------------*/

static const char *TAG = "TASK_WIFI";

bool isWiFiConnecting = false;

static TaskHandle_t task_handle = NULL;
static bool running = false;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief WiFi connecting task
 */
static void task_wifi_set_wifi_connecting_task(void *pvParameters);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief WiFi event callback implementation
 */
void task_wifi_event_callback(wifi_manager_event_t event, void *data)
{
    switch (event)
    {
    case WIFI_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected from network");
        break;

    case WIFI_EVENT_CONNECTING:
        ESP_LOGI(TAG, "Connecting to network...");
        isWiFiConnecting = true;
        break;

    case WIFI_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to network");
        break;

    case WIFI_EVENT_GOT_IP:
    {
        esp_netif_ip_info_t ip_info;
        if (wifi_manager_get_ip_info(&ip_info) == ESP_OK)
        {
            isWiFiConnecting = false;
            ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&ip_info.ip));
            ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&ip_info.gw));
            ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&ip_info.netmask));

            // Start MQTT client after getting IP
            ESP_LOGI(TAG, "MQTT Client starting...");
            esp_err_t ret = mqtt_manager_start();
            if (ret == ESP_OK)
            {
                ESP_LOGI(TAG, "MQTT client started successfully");
            }
            else
            {
                ESP_LOGE(TAG, "Failed to start MQTT client: %d", ret);
            }
        }
        break;
    }

    case WIFI_EVENT_PROVISIONING_STARTED:
        isWiFiConnecting = false;
        ESP_LOGI(TAG, "Provisioning started");
        ESP_LOGI(TAG, "AP SSID: %s", WIFI_AP_SSID);
        ESP_LOGI(TAG, "AP IP: 192.168.4.1");
        ESP_LOGI(TAG, "Open browser to configure WiFi");
        break;

    case WIFI_EVENT_PROVISIONING_FAILED:
        ESP_LOGE(TAG, "Provisioning failed");
        break;

    case WIFI_EVENT_PROVISIONING_SUCCESS:
        ESP_LOGI(TAG, "Provisioning successful!");
        ESP_LOGI(TAG, "Device will restart to connect...");
        break;

    default:
        ESP_LOGW(TAG, "Unknown event: %d", event);
        break;
    }
}

/**
 * @brief Initialize WiFi connecting task
 */
esp_err_t task_wifi_set_wifi_connecting_init(void)
{
    if (running)
    {
        ESP_LOGW(TAG, "Task WiFi connecting already initialized");
        return ESP_OK;
    }

    // Start WiFi connecting task
    running = true;
    BaseType_t result = xTaskCreate(
        task_wifi_set_wifi_connecting_task,
        "wifi_connecting",
        2048,
        NULL,
        5,
        &task_handle);

    if (result != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create WiFi connecting task");
        running = false;
        task_handle = NULL;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Task WiFi connecting initialized");
    return ESP_OK;
}

/* Private functions ----------------------------------------------------------*/

/**
 * @brief WiFi connecting task
 */
static void task_wifi_set_wifi_connecting_task(void *pvParameters)
{
    while (running)
    {
        if (isWiFiConnecting)
        {
            // Toggle LED blink state
            isWiFi = !isWiFi;
        }

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}