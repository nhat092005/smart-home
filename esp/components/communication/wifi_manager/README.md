# WiFi Manager

## Overview

The WiFi Manager component provides comprehensive WiFi connectivity management for ESP32. It supports Station (STA) mode for connecting to access points, Access Point (AP) mode for provisioning, credential storage in NVS, and includes a captive portal DNS server for automatic redirect during provisioning.

## Features

- Station mode with automatic reconnection
- Access Point mode for device provisioning
- NVS-based credential persistence
- Captive portal DNS server
- Network scanning with RSSI
- Event callback notification system
- Configurable via Kconfig

## File Structure

```
wifi_manager/
    CMakeLists.txt
    Kconfig
    wifi_manager.c         # Main WiFi management implementation
    dns_server.c           # DNS server for captive portal
    include/
        wifi_manager.h     # Public API
        wifi_config.h      # Configuration defines
```

## Dependencies

- `esp_wifi` - ESP-IDF WiFi driver
- `esp_netif` - Network interface
- `nvs_flash` - Non-volatile storage
- `esp_event` - Event loop
- `lwip` - TCP/IP stack (for DNS server)

## API Reference

### Types

```c
// WiFi manager events
typedef enum {
    WIFI_EVENT_DISCONNECTED,       // Disconnected from AP
    WIFI_EVENT_CONNECTING,         // Attempting to connect
    WIFI_EVENT_CONNECTED,          // Connected to AP
    WIFI_EVENT_GOT_IP,             // Got IP address
    WIFI_EVENT_PROVISIONING_START, // Provisioning mode started
    WIFI_EVENT_PROVISIONING_STOP,  // Provisioning mode stopped
} wifi_manager_event_t;

// Event callback function type
typedef void (*wifi_manager_callback_t)(wifi_manager_event_t event);
```

### Initialization Functions

| Function | Description |
|----------|-------------|
| `wifi_manager_init(void)` | Initialize WiFi manager, NVS, and event handlers |
| `wifi_manager_deinit(void)` | Deinitialize and release resources |

### Connection Functions

| Function | Description |
|----------|-------------|
| `wifi_manager_start(void)` | Start WiFi in station mode using saved credentials |
| `wifi_manager_connect(ssid, password)` | Connect to specific AP |
| `wifi_manager_disconnect(void)` | Disconnect from current AP |

### Provisioning Functions

| Function | Description |
|----------|-------------|
| `wifi_manager_start_provisioning(void)` | Start AP mode with DNS server |
| `wifi_manager_stop_provisioning(void)` | Stop AP mode and DNS server |

### Status Functions

| Function | Return Type | Description |
|----------|-------------|-------------|
| `wifi_manager_is_connected(void)` | `bool` | Check if connected to AP |
| `wifi_manager_is_provisioned(void)` | `bool` | Check if credentials exist |
| `wifi_manager_get_ip_info(ip_info)` | `esp_err_t` | Get current IP information |
| `wifi_manager_get_rssi(void)` | `int8_t` | Get current signal strength |

### Network Functions

| Function | Description |
|----------|-------------|
| `wifi_manager_scan_networks(ap_list, ap_count)` | Scan for available networks |

### Credential Functions

| Function | Description |
|----------|-------------|
| `wifi_manager_save_credentials(ssid, password)` | Save credentials to NVS |
| `wifi_manager_clear_credentials(void)` | Clear credentials from NVS |

### Callback Functions

| Function | Description |
|----------|-------------|
| `wifi_manager_register_callback(callback)` | Register event callback |

## Configuration (Kconfig)

```
WIFI_MANAGER_AP_SSID          # AP mode SSID (default: "SmartHome_Setup")
WIFI_MANAGER_MAX_RETRY        # Max connection retries (default: 5)
WIFI_MANAGER_SCAN_MAX_AP      # Max APs in scan list (default: 16)
WIFI_MANAGER_HTTP_PORT        # HTTP server port (default: 80)
WIFI_MANAGER_DNS_PORT         # DNS server port (default: 53)
```

## Configuration Defines (wifi_config.h)

```c
// Access Point Configuration
#define WIFI_AP_SSID              CONFIG_WIFI_MANAGER_AP_SSID
#define WIFI_AP_CHANNEL           1
#define WIFI_AP_MAX_CONNECTIONS   4

// Station Configuration
#define WIFI_STA_MAX_RETRY        CONFIG_WIFI_MANAGER_MAX_RETRY
#define WIFI_SCAN_MAX_AP          CONFIG_WIFI_MANAGER_SCAN_MAX_AP

// Server Ports
#define HTTP_SERVER_PORT          CONFIG_WIFI_MANAGER_HTTP_PORT
#define DNS_SERVER_PORT           CONFIG_WIFI_MANAGER_DNS_PORT

// NVS Keys
#define NVS_NAMESPACE             "wifi_creds"
#define NVS_KEY_SSID              "ssid"
#define NVS_KEY_PASSWORD          "password"
```

## State Machine

```
    +-------+
    | IDLE  |<------------------+
    +---+---+                   |
        |                       |
        | start()               | disconnect()
        v                       |
    +----------+                |
    |CONNECTING|----------------+
    +----+-----+                |
         |                      |
         | got_ip               | fail (max_retry)
         v                      |
    +-----------+               |
    | CONNECTED |---------------+
    +-----------+

    Provisioning (parallel state):
    +---------------+
    | PROVISIONING  |  (AP + DNS running)
    +---------------+
```

## Usage Examples

### Basic Initialization

```c
#include "wifi_manager.h"

static void wifi_callback(wifi_manager_event_t event)
{
    switch (event)
    {
        case WIFI_EVENT_GOT_IP:
            ESP_LOGI(TAG, "Connected with IP");
            break;
        case WIFI_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected");
            break;
        default:
            break;
    }
}

void app_main(void)
{
    wifi_manager_init();
    wifi_manager_register_callback(wifi_callback);
    
    if (wifi_manager_is_provisioned())
    {
        wifi_manager_start();
    }
    else
    {
        wifi_manager_start_provisioning();
    }
}
```

### Network Scanning

```c
wifi_ap_record_t ap_list[16];
uint16_t ap_count = 16;

esp_err_t ret = wifi_manager_scan_networks(ap_list, &ap_count);
if (ret == ESP_OK)
{
    for (int i = 0; i < ap_count; i++)
    {
        ESP_LOGI(TAG, "SSID: %s, RSSI: %d", 
                 ap_list[i].ssid, ap_list[i].rssi);
    }
}
```

### Manual Connection

```c
esp_err_t ret = wifi_manager_connect("MyNetwork", "MyPassword");
if (ret == ESP_OK)
{
    // Connection initiated, wait for callback
}
```

### Get Status Information

```c
if (wifi_manager_is_connected())
{
    esp_netif_ip_info_t ip_info;
    wifi_manager_get_ip_info(&ip_info);
    
    int8_t rssi = wifi_manager_get_rssi();
    
    ESP_LOGI(TAG, "IP: " IPSTR ", RSSI: %d dBm",
             IP2STR(&ip_info.ip), rssi);
}
```

## DNS Server

The WiFi Manager includes a captive portal DNS server that redirects all DNS queries to the ESP32 IP address during provisioning mode. This enables automatic redirect to the configuration page when users connect to the provisioning AP.

**Features:**
- UDP socket on port 53
- Responds to all DNS queries with AP IP (192.168.4.1)
- Automatic start/stop with provisioning mode
- FreeRTOS task with 4KB stack

## NVS Storage

Credentials are stored in NVS under the `wifi_creds` namespace:

| Key | Type | Description |
|-----|------|-------------|
| `ssid` | String | WiFi network SSID |
| `password` | String | WiFi network password |

## Error Handling

All functions return `esp_err_t`:

| Return Value | Description |
|--------------|-------------|
| `ESP_OK` | Operation successful |
| `ESP_FAIL` | General failure |
| `ESP_ERR_INVALID_ARG` | Invalid parameter |
| `ESP_ERR_NVS_*` | NVS operation error |
| `ESP_ERR_WIFI_*` | WiFi driver error |

## Thread Safety

- Event callbacks run in WiFi task context
- All public functions are thread-safe
- Uses FreeRTOS event groups for synchronization
- DNS server runs in separate FreeRTOS task

## Notes

- Maximum SSID length: 32 characters
- Maximum password length: 64 characters
- Reconnection uses exponential backoff
- Provisioning AP has no password (open network)
- DNS server only active during provisioning
