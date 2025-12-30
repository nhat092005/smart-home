# Communication Component

## Overview

The Communication component provides network connectivity and communication capabilities for the ESP32 Smart Home system. This component handles WiFi connection management, MQTT messaging for IoT communication, and HTTP web server for device provisioning.

## Architecture

```
communication/
    CMakeLists.txt
    README.md
    wifi_manager/          # WiFi station/AP management and provisioning
    mqtt_manager/          # MQTT client for IoT messaging
    webserver/             # HTTP server for WiFi configuration
```

## Subcomponents

### wifi_manager

WiFi connection manager supporting both Station (STA) and Access Point (AP) modes. Provides captive portal provisioning with DNS server for automatic redirect.

**Key Features:**
- Station mode connection with automatic reconnection
- Access Point mode for provisioning
- NVS credential storage
- DNS server for captive portal
- Network scanning
- Event callback system

### mqtt_manager

MQTT client implementation using SSL/TLS for secure IoT communication. Supports publish/subscribe with configurable topics.

**Key Features:**
- MQTT over SSL (port 8883)
- Configurable topic structure
- Data/State/Info publishing
- Command subscription with callbacks
- Automatic reconnection
- QoS and retain support

### webserver

HTTP web server providing WiFi configuration interface during provisioning mode.

**Key Features:**
- Embedded HTML/CSS/JS files
- WiFi network scanning endpoint
- Credential configuration endpoint
- Status monitoring endpoint
- JSON API responses

## Dependencies

- **ESP-IDF Components:**
  - `esp_wifi` - WiFi driver
  - `esp_http_server` - HTTP server
  - `mqtt` - MQTT client
  - `esp-tls` - TLS/SSL support
  - `nvs_flash` - Non-volatile storage
  - `lwip` - TCP/IP stack

- **Internal Components:**
  - `utilities/json_helper` - JSON parsing and creation

## Data Flow

```
                    +------------------+
                    |   wifi_manager   |
                    +--------+---------+
                             |
            +----------------+----------------+
            |                                 |
    +-------v-------+               +---------v---------+
    |   webserver   |               |   mqtt_manager    |
    | (provisioning)|               |   (IoT comms)     |
    +---------------+               +-------------------+
            |                                 |
    +-------v-------+               +---------v---------+
    |  HTTP Client  |               |   MQTT Broker     |
    +---------------+               +-------------------+
```

## Configuration

All subcomponents are configurable via Kconfig:

| Category | Options |
|----------|---------|
| WiFi Manager | AP SSID, max retry, scan limit, HTTP/DNS ports |
| MQTT Manager | Broker URI/port, credentials, topics, keep alive |

## Usage Example

```c
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "webserver.h"

// WiFi event callback
static void wifi_event_handler(wifi_manager_event_t event)
{
    switch (event)
    {
        case WIFI_EVENT_CONNECTED:
            mqtt_manager_start();
            break;
        case WIFI_EVENT_DISCONNECTED:
            mqtt_manager_stop();
            break;
        default:
            break;
    }
}

void app_main(void)
{
    // Initialize WiFi
    wifi_manager_init();
    wifi_manager_register_callback(wifi_event_handler);
    
    // Check if provisioned
    if (wifi_manager_is_provisioned())
    {
        wifi_manager_start();
    }
    else
    {
        wifi_manager_start_provisioning();
        webserver_start();
    }
    
    // Initialize MQTT
    mqtt_manager_init();
}
```

## Error Handling

All functions return `esp_err_t`:
- `ESP_OK` - Operation successful
- `ESP_FAIL` - General failure
- `ESP_ERR_INVALID_ARG` - Invalid parameter
- `ESP_ERR_NO_MEM` - Memory allocation failed
- `ESP_ERR_WIFI_*` - WiFi specific errors

## Thread Safety

- **wifi_manager**: Thread-safe with FreeRTOS event groups
- **mqtt_manager**: Thread-safe with internal mutexes
- **webserver**: Single-threaded HTTP server with async handlers

## Related Documentation

- [wifi_manager README](wifi_manager/README.md)
- [mqtt_manager README](mqtt_manager/README.md)
- [webserver README](webserver/README.md)
