/**
 * @file webserver.c
 *
 * @brief WiFi Manager Web Server Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_system.h"
#include "json_helper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_manager.h"
#include <string.h>

/* Private defines -----------------------------------------------------------*/

#define HTTP_POST_BUFFER_SIZE 512
#define IP_STRING_MAX_SIZE 16

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "WEB_SERVER";
static httpd_handle_t g_server = NULL;

/* Embedded files from EMBED_FILES in CMakeLists.txt */
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

extern const uint8_t style_css_start[] asm("_binary_style_css_start");
extern const uint8_t style_css_end[] asm("_binary_style_css_end");

extern const uint8_t script_js_start[] asm("_binary_script_js_start");
extern const uint8_t script_js_end[] asm("_binary_script_js_end");

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief HTTP GET handler for root page
 *
 * @param[in] req Pointer to HTTP request
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t root_handler(httpd_req_t *req);

/**
 * @brief HTTP GET handler for CSS content
 *
 * @param[in] req Pointer to HTTP request
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t css_handler(httpd_req_t *req);

/**
 * @brief HTTP GET handler for JavaScript content
 *
 * @param[in] req Pointer to HTTP request
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t js_handler(httpd_req_t *req);

/**
 * @brief HTTP GET handler for WiFi scan
 *
 * @param[in] req Pointer to HTTP request
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t scan_handler(httpd_req_t *req);

/**
 * @brief HTTP POST handler for WiFi connect
 *
 * @param[in] req Pointer to HTTP request
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t connect_handler(httpd_req_t *req);

/**
 * @brief HTTP GET handler for WiFi status
 *
 * @param[in] req Pointer to HTTP request
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t status_handler(httpd_req_t *req);

/**
 * @brief HTTP POST handler for resetting WiFi credentials
 *
 * @param[in] req Pointer to HTTP request
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t reset_handler(httpd_req_t *req);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Start the web server
 */
esp_err_t webserver_start(void)
{
    if (g_server != NULL)
    {
        return ESP_OK;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = HTTP_SERVER_PORT;
    config.max_uri_handlers = 8;
    config.stack_size = 8192;

    esp_err_t ret = httpd_start(&g_server, &config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start HTTP server: %s", esp_err_to_name(ret));
        return ret;
    }

    httpd_uri_t root_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_handler,
    };
    httpd_register_uri_handler(g_server, &root_uri);

    httpd_uri_t css_uri = {
        .uri = "/style.css",
        .method = HTTP_GET,
        .handler = css_handler,
    };
    httpd_register_uri_handler(g_server, &css_uri);

    httpd_uri_t js_uri = {
        .uri = "/script.js",
        .method = HTTP_GET,
        .handler = js_handler,
    };
    httpd_register_uri_handler(g_server, &js_uri);

    httpd_uri_t scan_uri = {
        .uri = "/scan",
        .method = HTTP_GET,
        .handler = scan_handler,
    };
    httpd_register_uri_handler(g_server, &scan_uri);

    httpd_uri_t connect_uri = {
        .uri = "/connect",
        .method = HTTP_POST,
        .handler = connect_handler,
    };
    httpd_register_uri_handler(g_server, &connect_uri);

    httpd_uri_t status_uri = {
        .uri = "/status",
        .method = HTTP_GET,
        .handler = status_handler,
    };
    httpd_register_uri_handler(g_server, &status_uri);

    httpd_uri_t reset_uri = {
        .uri = "/reset",
        .method = HTTP_POST,
        .handler = reset_handler,
    };
    httpd_register_uri_handler(g_server, &reset_uri);

    ESP_LOGI(TAG, "HTTP server started on port 80");
    return ESP_OK;
}

/**
 * @brief Stop the web server
 */
esp_err_t webserver_stop(void)
{
    if (g_server == NULL)
    {
        return ESP_OK;
    }

    esp_err_t ret = httpd_stop(g_server);
    g_server = NULL;

    ESP_LOGI(TAG, "HTTP server stopped");
    return ret;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief HTTP GET handler for root page
 */
static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}

/**
 * @brief HTTP GET handler for CSS content
 */
static esp_err_t css_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)style_css_start, style_css_end - style_css_start);
    return ESP_OK;
}

/**
 * @brief HTTP GET handler for JavaScript content
 */
static esp_err_t js_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)script_js_start, script_js_end - script_js_start);
    return ESP_OK;
}

/**
 * @brief HTTP GET handler for WiFi scan
 */
static esp_err_t scan_handler(httpd_req_t *req)
{
    wifi_ap_record_t ap_list[WIFI_SCAN_MAX_AP];
    uint16_t ap_count = WIFI_SCAN_MAX_AP;

    esp_err_t ret = wifi_manager_scan_networks(ap_list, &ap_count);
    if (ret != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Scan failed");
        return ESP_FAIL;
    }

    char *json_str = json_helper_create_wifi_scan_result(ap_list, ap_count);
    if (json_str == NULL)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    free(json_str);

    return ESP_OK;
}

/**
 * @brief HTTP POST handler for WiFi connect
 */
static esp_err_t connect_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Connect request received");

    char content[HTTP_POST_BUFFER_SIZE];
    int ret = httpd_req_recv(req, content, sizeof(content));
    if (ret <= 0)
    {
        ESP_LOGE(TAG, "Failed to receive POST data: %d", ret);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid request");
        return ESP_FAIL;
    }
    content[ret] = '\0';

    ESP_LOGI(TAG, "Received data (%d bytes): %s", ret, content);

    char ssid[32];
    char password[64];

    esp_err_t parse_ret = json_helper_parse_wifi_credentials(content, ssid, sizeof(ssid),
                                                             password, sizeof(password));
    if (parse_ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to parse credentials");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid credentials");
        return ESP_FAIL;
    }

    esp_err_t save_ret = wifi_manager_save_credentials(ssid, password);
    if (save_ret != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to save");
        return ESP_FAIL;
    }

    char *json_str = json_helper_create_simple_response("ok", "Connecting...");

    if (json_str == NULL)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));
    free(json_str);

    ESP_LOGI(TAG, "Received credentials, restarting to connect");

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();

    return ESP_OK;
}

/**
 * @brief HTTP GET handler for WiFi status
 */
static esp_err_t status_handler(httpd_req_t *req)
{
    bool connected = wifi_manager_is_connected();
    bool provisioned = wifi_manager_is_provisioned();
    char ip_str[IP_STRING_MAX_SIZE] = {0};
    int8_t rssi = 0;

    if (connected)
    {
        esp_netif_ip_info_t ip_info;
        if (wifi_manager_get_ip_info(&ip_info) == ESP_OK)
        {
            snprintf(ip_str, sizeof(ip_str), IPSTR, IP2STR(&ip_info.ip));
        }
        rssi = wifi_manager_get_rssi();
    }

    char *json_str = json_helper_create_wifi_status(connected, provisioned,
                                                    connected ? ip_str : NULL, rssi);
    if (json_str == NULL)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));
    free(json_str);

    return ESP_OK;
}

/**
 * @brief HTTP POST handler for resetting WiFi credentials
 */
static esp_err_t reset_handler(httpd_req_t *req)
{
    wifi_manager_clear_credentials();

    char *json_str = json_helper_create_simple_response("ok", "Credentials cleared");
    if (json_str == NULL)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));
    free(json_str);

    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();

    return ESP_OK;
}