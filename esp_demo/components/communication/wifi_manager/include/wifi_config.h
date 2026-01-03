/**
 * @file wifi_config.h
 * 
 * @brief WiFi Manager Configuration from Kconfig
 */

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#include "sdkconfig.h"

/* Exported defines ----------------------------------------------------------*/

/* WiFi Access Point Configuration */
#define WIFI_AP_SSID           CONFIG_WIFI_AP_SSID
#define WIFI_AP_PASSWORD       ""
#define WIFI_AP_CHANNEL        1
#define WIFI_AP_MAX_CONN       4

/* WiFi Station Configuration */
#define WIFI_RECONNECT_MAX     CONFIG_WIFI_MAX_RETRY
#define WIFI_SCAN_MAX_AP       CONFIG_SCAN_MAX_AP

/* Server Configuration */
#define HTTP_SERVER_PORT       CONFIG_HTTP_SERVER_PORT
#define DNS_SERVER_PORT        CONFIG_DNS_SERVER_PORT

/* NVS Storage Configuration */
#define NVS_NAMESPACE          "wifi_config"
#define NVS_KEY_SSID           "ssid"
#define NVS_KEY_PASSWORD       "password"
#define NVS_KEY_PROVISIONED    "provisioned"

#endif /* WIFI_CONFIG_H */