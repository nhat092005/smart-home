/**
 * @file wifi_manager.c
 *
 * @brief WiFi Manager Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "esp_log.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "wifi_manager.h"
#include "webserver.h"
#include <string.h>

/* Private defines -----------------------------------------------------------*/

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

/* Private types -------------------------------------------------------------*/

/**
 * @brief WiFi state enumeration
 */
typedef enum
{
    WIFI_STATE_IDLE,         //!< Idle state
    WIFI_STATE_CONNECTING,   //!< Connecting state
    WIFI_STATE_CONNECTED,    //!< Connected state
    WIFI_STATE_DISCONNECTED, //!< Disconnected state
    WIFI_STATE_PROVISIONING  //!< Provisioning state
} wifi_state_t;

/**
 * @brief WiFi manager context structure
 */
typedef struct
{
    wifi_state_t state;             //!< Current WiFi state
    esp_netif_t *sta_netif;         //!< Station network interface
    esp_netif_t *ap_netif;          //!< Access point network interface
    EventGroupHandle_t event_group; //!< Event group for WiFi events
    SemaphoreHandle_t mutex;        //!< Mutex for thread-safe access
    wifi_event_callback_t callback; //!< Event callback
    uint8_t retry_count;            //!< Retry count for connection attempts
    char ssid[32];                  //!< WiFi SSID
    char password[64];              //!< WiFi password
    bool provisioned;               //!< Provisioning status
    bool initialized;               //!< Initialization status
} wifi_manager_context_t;

/* Exported variables --------------------------------------------------------*/

bool isWiFi = false; //!< Global WiFi connection state indicator

/* External variables --------------------------------------------------------*/

extern bool isWiFiConnecting;

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "WIFI_MANAGER";

static wifi_manager_context_t g_wifi_ctx = {0};

static int retry_count = 0;

/* External functions -------------------------------------------------------*/

extern esp_err_t webserver_start(void);
extern esp_err_t webserver_stop(void);
extern esp_err_t dns_server_start(void);
extern esp_err_t dns_server_stop(void);

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief WiFi event handler
 *
 * @param[in] arg Event handler argument
 * @param[in] event_base Event base
 * @param[in] event_id Event ID
 * @param[in] event_data Event data
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/**
 * @brief Load WiFi credentials from NVS
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t load_credentials_from_nvs(void);

/* Exported functions ---------------------------------------------------------*/

/**
 * @brief Initialize WiFi manager
 */
esp_err_t wifi_manager_init(void)
{
    if (g_wifi_ctx.initialized)
    {
        return ESP_OK;
    }

    g_wifi_ctx.mutex = xSemaphoreCreateMutex();
    if (g_wifi_ctx.mutex == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    g_wifi_ctx.event_group = xEventGroupCreate();
    if (g_wifi_ctx.event_group == NULL)
    {
        vSemaphoreDelete(g_wifi_ctx.mutex);
        return ESP_ERR_NO_MEM;
    }

    esp_err_t ret = esp_netif_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init netif: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(ret));
        return ret;
    }

    g_wifi_ctx.sta_netif = esp_netif_create_default_wifi_sta();
    g_wifi_ctx.ap_netif = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init WiFi: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    if (ret != ESP_OK)
    {
        return ret;
    }

    ret = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);
    if (ret != ESP_OK)
    {
        return ret;
    }

    ret = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (ret != ESP_OK)
    {
        return ret;
    }

    load_credentials_from_nvs();

    g_wifi_ctx.state = WIFI_STATE_IDLE;
    g_wifi_ctx.initialized = true;

    ESP_LOGI(TAG, "WiFi Manager initialized");
    return ESP_OK;
}

/**
 * @brief Start WiFi manager
 */
esp_err_t wifi_manager_start(void)
{
    if (!g_wifi_ctx.initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (!g_wifi_ctx.provisioned || strlen(g_wifi_ctx.ssid) == 0)
    {
        ESP_LOGW(TAG, "Not provisioned, starting provisioning mode");
        return wifi_manager_start_provisioning();
    }

    return wifi_manager_connect(g_wifi_ctx.ssid, g_wifi_ctx.password);
}

/**
 * @brief Connect to WiFi network
 */
esp_err_t wifi_manager_connect(const char *ssid, const char *password)
{
    if (!g_wifi_ctx.initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (ssid == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(g_wifi_ctx.mutex, portMAX_DELAY);

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    if (password)
    {
        strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    }

    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK)
    {
        xSemaphoreGive(g_wifi_ctx.mutex);
        return ret;
    }

    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK)
    {
        xSemaphoreGive(g_wifi_ctx.mutex);
        return ret;
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK)
    {
        xSemaphoreGive(g_wifi_ctx.mutex);
        return ret;
    }

    g_wifi_ctx.state = WIFI_STATE_CONNECTING;
    g_wifi_ctx.retry_count = 0;

    if (g_wifi_ctx.callback)
    {
        g_wifi_ctx.callback(WIFI_EVENT_CONNECTING, NULL);
    }

    xSemaphoreGive(g_wifi_ctx.mutex);

    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid);
    return ESP_OK;
}

/**
 * @brief Disconnect from WiFi network
 */
esp_err_t wifi_manager_disconnect(void)
{
    return esp_wifi_disconnect();
}

/**
 * @brief Start WiFi provisioning
 */
esp_err_t wifi_manager_start_provisioning(void)
{
    if (!g_wifi_ctx.initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    xSemaphoreTake(g_wifi_ctx.mutex, portMAX_DELAY);

    wifi_config_t ap_config = {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .ssid_len = strlen(WIFI_AP_SSID),
            .channel = WIFI_AP_CHANNEL,
            .password = WIFI_AP_PASSWORD,
            .max_connection = WIFI_AP_MAX_CONN,
            .authmode = (strlen(WIFI_AP_PASSWORD) > 0) ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN,
        },
    };

    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if (ret != ESP_OK)
    {
        xSemaphoreGive(g_wifi_ctx.mutex);
        return ret;
    }

    ret = esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    if (ret != ESP_OK)
    {
        xSemaphoreGive(g_wifi_ctx.mutex);
        return ret;
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK)
    {
        xSemaphoreGive(g_wifi_ctx.mutex);
        return ret;
    }

    g_wifi_ctx.state = WIFI_STATE_PROVISIONING;

    webserver_start();
    dns_server_start();

    if (g_wifi_ctx.callback)
    {
        g_wifi_ctx.callback(WIFI_EVENT_PROVISIONING_STARTED, NULL);
    }

    xSemaphoreGive(g_wifi_ctx.mutex);

    ESP_LOGI(TAG, "Provisioning started: AP SSID=%s", WIFI_AP_SSID);
    return ESP_OK;
}

/**
 * @brief Stop WiFi provisioning
 */
esp_err_t wifi_manager_stop_provisioning(void)
{
    webserver_stop();
    dns_server_stop();

    g_wifi_ctx.state = WIFI_STATE_IDLE;
    return esp_wifi_stop();
}

/**
 * @brief Check if WiFi is connected
 */
bool wifi_manager_is_connected(void)
{
    xSemaphoreTake(g_wifi_ctx.mutex, portMAX_DELAY);
    bool connected = (g_wifi_ctx.state == WIFI_STATE_CONNECTED);
    xSemaphoreGive(g_wifi_ctx.mutex);
    return connected;
}

/**
 * @brief Check if WiFi is provisioned
 */
bool wifi_manager_is_provisioned(void)
{
    xSemaphoreTake(g_wifi_ctx.mutex, portMAX_DELAY);
    bool provisioned = g_wifi_ctx.provisioned;
    xSemaphoreGive(g_wifi_ctx.mutex);
    return provisioned;
}

/**
 * @brief Get IP information
 */
esp_err_t wifi_manager_get_ip_info(esp_netif_ip_info_t *ip_info)
{
    if (ip_info == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (!wifi_manager_is_connected())
    {
        return ESP_ERR_INVALID_STATE;
    }

    return esp_netif_get_ip_info(g_wifi_ctx.sta_netif, ip_info);
}

/**
 * @brief Get current RSSI
 */
int8_t wifi_manager_get_rssi(void)
{
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK)
    {
        return ap_info.rssi;
    }
    return -128; // Return minimum RSSI value on error
}

/**
 * @brief Scan for available WiFi networks
 */
esp_err_t wifi_manager_scan_networks(wifi_ap_record_t *ap_list, uint16_t *count)
{
    if (ap_list == NULL || count == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    wifi_scan_config_t scan_config = {
        .show_hidden = false,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    };

    esp_err_t ret = esp_wifi_scan_start(&scan_config, true);
    if (ret != ESP_OK)
    {
        return ret;
    }

    ret = esp_wifi_scan_get_ap_records(count, ap_list);
    if (ret != ESP_OK)
    {
        return ret;
    }

    ESP_LOGI(TAG, "Scan complete, found %d networks", *count);
    return ESP_OK;
}

/**
 * @brief Register WiFi event callback
 */
esp_err_t wifi_manager_register_callback(wifi_event_callback_t callback)
{
    if (callback == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(g_wifi_ctx.mutex, portMAX_DELAY);
    g_wifi_ctx.callback = callback;
    xSemaphoreGive(g_wifi_ctx.mutex);

    return ESP_OK;
}

/**
 * @brief Save WiFi credentials to NVS
 */
esp_err_t wifi_manager_save_credentials(const char *ssid, const char *password)
{
    if (ssid == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (ret != ESP_OK)
    {
        return ret;
    }

    ret = nvs_set_str(handle, NVS_KEY_SSID, ssid);
    if (ret != ESP_OK)
    {
        nvs_close(handle);
        return ret;
    }

    ret = nvs_set_str(handle, NVS_KEY_PASSWORD, password ? password : "");
    if (ret != ESP_OK)
    {
        nvs_close(handle);
        return ret;
    }

    ret = nvs_set_u8(handle, NVS_KEY_PROVISIONED, 1);
    if (ret != ESP_OK)
    {
        nvs_close(handle);
        return ret;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    if (ret == ESP_OK)
    {
        xSemaphoreTake(g_wifi_ctx.mutex, portMAX_DELAY);
        strncpy(g_wifi_ctx.ssid, ssid, sizeof(g_wifi_ctx.ssid) - 1);
        strncpy(g_wifi_ctx.password, password ? password : "", sizeof(g_wifi_ctx.password) - 1);
        g_wifi_ctx.provisioned = true;
        xSemaphoreGive(g_wifi_ctx.mutex);

        ESP_LOGI(TAG, "Credentials saved: SSID=%s", ssid);
    }

    return ret;
}

/**
 * @brief Clear WiFi credentials from NVS
 */
esp_err_t wifi_manager_clear_credentials(void)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (ret != ESP_OK)
    {
        return ret;
    }

    nvs_erase_key(handle, NVS_KEY_SSID);
    nvs_erase_key(handle, NVS_KEY_PASSWORD);
    nvs_erase_key(handle, NVS_KEY_PROVISIONED);
    ret = nvs_commit(handle);
    nvs_close(handle);

    if (ret == ESP_OK)
    {
        xSemaphoreTake(g_wifi_ctx.mutex, portMAX_DELAY);
        memset(g_wifi_ctx.ssid, 0, sizeof(g_wifi_ctx.ssid));
        memset(g_wifi_ctx.password, 0, sizeof(g_wifi_ctx.password));
        g_wifi_ctx.provisioned = false;
        xSemaphoreGive(g_wifi_ctx.mutex);

        ESP_LOGI(TAG, "Credentials cleared");

        isWiFi = false;
    }

    return ret;
}

/**
 * @brief Deinitialize WiFi manager
 */
esp_err_t wifi_manager_deinit(void)
{
    if (!g_wifi_ctx.initialized)
    {
        return ESP_OK;
    }

    wifi_manager_stop_provisioning();
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();

    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler);

    if (g_wifi_ctx.mutex)
    {
        vSemaphoreDelete(g_wifi_ctx.mutex);
    }

    if (g_wifi_ctx.event_group)
    {
        vEventGroupDelete(g_wifi_ctx.event_group);
    }

    g_wifi_ctx.initialized = false;

    ESP_LOGI(TAG, "WiFi Manager deinitialized");
    return ESP_OK;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief WiFi event handler
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    // Handle STA_START event
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
        return;
    }

    // Lock for state changes
    xSemaphoreTake(g_wifi_ctx.mutex, portMAX_DELAY);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        // Ignore disconnection events if already in provisioning mode
        if (g_wifi_ctx.state == WIFI_STATE_PROVISIONING)
        {
            xSemaphoreGive(g_wifi_ctx.mutex);
            return;
        }

        g_wifi_ctx.state = WIFI_STATE_DISCONNECTED;
        isWiFi = false; // Update global state

        // Check retry count and increment atomically
        bool should_retry = (g_wifi_ctx.retry_count < WIFI_RECONNECT_MAX);
        if (should_retry)
        {
            g_wifi_ctx.retry_count++;
        }
        uint8_t current_retry = g_wifi_ctx.retry_count;

        wifi_event_callback_t callback = g_wifi_ctx.callback;
        xSemaphoreGive(g_wifi_ctx.mutex);

        // Call callback outside mutex
        if (callback)
        {
            callback(WIFI_EVENT_DISCONNECTED, NULL);
        }

        // Retry logic without holding mutex
        if (should_retry)
        {
            retry_count += 1;
            ESP_LOGI(TAG, "Retry connecting (%d/%d)", current_retry, WIFI_RECONNECT_MAX);

            isWiFiConnecting = true;

            if (retry_count > WIFI_RECONNECT_MAX - 1)
            {
                isWiFiConnecting = false;
                retry_count = 0;
            }
            esp_wifi_connect();
        }
        else
        {
            xEventGroupSetBits(g_wifi_ctx.event_group, WIFI_FAIL_BIT);
            ESP_LOGE(TAG, "Failed to connect after %d attempts", WIFI_RECONNECT_MAX);
            ESP_LOGW(TAG, "Clearing credentials and starting provisioning mode...");

            // Clear failed credentials to avoid retry loop
            wifi_manager_clear_credentials();

            // Stop STA mode and start provisioning
            esp_wifi_disconnect();
            esp_wifi_stop();

            // Wait for clean stop
            vTaskDelay(pdMS_TO_TICKS(500));

            wifi_manager_start_provisioning();
        }
        return;
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        g_wifi_ctx.retry_count = 0;
        g_wifi_ctx.state = WIFI_STATE_CONNECTED;

        isWiFi = true; // Update global state

        xEventGroupSetBits(g_wifi_ctx.event_group, WIFI_CONNECTED_BIT);
        wifi_event_callback_t callback = g_wifi_ctx.callback;
        xSemaphoreGive(g_wifi_ctx.mutex);

        // Call callback outside mutex
        if (callback)
        {
            callback(WIFI_EVENT_GOT_IP, event_data);
        }
        return;
    }

    xSemaphoreGive(g_wifi_ctx.mutex);
}

/**
 * @brief Load WiFi credentials from NVS
 */
static esp_err_t load_credentials_from_nvs(void)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (ret != ESP_OK)
    {
        return ret;
    }

    size_t ssid_len = sizeof(g_wifi_ctx.ssid);
    size_t pass_len = sizeof(g_wifi_ctx.password);

    ret = nvs_get_str(handle, NVS_KEY_SSID, g_wifi_ctx.ssid, &ssid_len);
    if (ret != ESP_OK)
    {
        nvs_close(handle);
        return ret;
    }

    ret = nvs_get_str(handle, NVS_KEY_PASSWORD, g_wifi_ctx.password, &pass_len);
    if (ret != ESP_OK)
    {
        nvs_close(handle);
        return ret;
    }

    uint8_t provisioned = 0;
    ret = nvs_get_u8(handle, NVS_KEY_PROVISIONED, &provisioned);
    g_wifi_ctx.provisioned = (provisioned == 1);

    nvs_close(handle);
    ESP_LOGI(TAG, "Loaded credentials from NVS: SSID=%s", g_wifi_ctx.ssid);
    return ESP_OK;
}