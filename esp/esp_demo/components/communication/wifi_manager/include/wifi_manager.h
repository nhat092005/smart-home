/**
 * @file wifi_manager.h
 *
 * @brief WiFi Manager Header File
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

/* Includes ------------------------------------------------------------------*/

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "wifi_config.h"
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

typedef enum
{
    WIFI_EVENT_DISCONNECTED,         //!< WiFi disconnected event
    WIFI_EVENT_CONNECTING,           //!< WiFi connecting event
    WIFI_EVENT_CONNECTED,            //!< WiFi connected event
    WIFI_EVENT_GOT_IP,               //!< WiFi got IP event
    WIFI_EVENT_PROVISIONING_STARTED, //!< WiFi provisioning started event
    WIFI_EVENT_PROVISIONING_FAILED,  //!< WiFi provisioning failed event
    WIFI_EVENT_PROVISIONING_SUCCESS  //!< WiFi provisioning success event
} wifi_manager_event_t;

/* Exported function prototypes ----------------------------------------------*/

typedef void (*wifi_event_callback_t)(wifi_manager_event_t event, void *data);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize WiFi Manager
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_init(void);

/**
 * @brief Start WiFi Manager
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_start(void);

/**
 * @brief Connect to a WiFi network
 *
 * @param[in] ssid SSID of the network
 * @param[in] password Password of the network
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_connect(const char *ssid, const char *password);

/**
 * @brief Disconnect from the current WiFi network
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_disconnect(void);

/**
 * @brief Start WiFi provisioning
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_start_provisioning(void);

/**
 * @brief Stop WiFi provisioning
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_stop_provisioning(void);

/**
 * @brief Check if WiFi is connected
 *
 * @return true if connected, false otherwise
 */
bool wifi_manager_is_connected(void);

/**
 * @brief Check if WiFi is provisioned
 *
 * @return true if provisioned, false otherwise
 */
bool wifi_manager_is_provisioned(void);

/**
 * @brief Get IP information
 *
 * @param[in] ip_info Pointer to esp_netif_ip_info_t structure to hold IP info
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_get_ip_info(esp_netif_ip_info_t *ip_info);

/**
 * @brief Get current RSSI value
 *
 * @return RSSI value in dBm, or -128 on error
 */
int8_t wifi_manager_get_rssi(void);

/**
 * @brief Scan for available WiFi networks
 *
 * @param[out] ap_list Pointer to an array of wifi_ap_record_t to hold scan results
 * @param[in,out] count Pointer to the number of entries in ap_list
 *                      Updated with actual number found
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_scan_networks(wifi_ap_record_t *ap_list, uint16_t *count);

/**
 * @brief Register a callback for WiFi events
 *
 * @param[in] callback Function pointer to the callback
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_register_callback(wifi_event_callback_t callback);

/**
 * @brief Save WiFi credentials
 *
 * @param[in] ssid SSID of the network
 * @param[in] password Password of the network
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_save_credentials(const char *ssid, const char *password);

/**
 * @brief Clear saved WiFi credentials
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_clear_credentials(void);

/**
 * @brief Deinitialize WiFi Manager
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_manager_deinit(void);

#endif /* WIFI_MANAGER_H */