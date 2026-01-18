# WiFi Management Task

## Overview

FreeRTOS task for WiFi connection lifecycle management, including automatic connection, reconnection, and captive portal provisioning.

## Features

- Automatic WiFi connection
- Connection state monitoring
- Automatic reconnection on failure
- Captive portal provisioning
- RSSI monitoring
- Event-driven architecture

## Task Function

```c
void wifi_task(void *pvParameters);
```

## Task Workflow

1. **Check Credentials**: Verify WiFi credentials in NVS
2. **Connect to AP**: Attempt connection to saved network
3. **Monitor Connection**: Track connection state
4. **Handle Disconnection**: Automatic reconnection attempts
5. **Fallback to AP Mode**: Start captive portal if connection fails

## Usage Example

```c
#include "task_wifi.h"
#include "task_manager.h"

void app_main(void) {
    // Initialize WiFi manager
    nvs_flash_init();
    wifi_manager_init();
    
    // Create WiFi task (typically done by task_manager)
    xTaskCreate(wifi_task, "wifi", 4096, NULL, 3, NULL);
    
    // Task manages WiFi connection lifecycle
}
```

## Task Loop

```c
void wifi_task(void *pvParameters) {
    uint8_t retry_count = 0;
    const uint8_t MAX_RETRY = 5;
    
    while (1) {
        // Check connection status
        if (!wifi_manager_is_connected()) {
            ESP_LOGI(TAG, "WiFi disconnected, attempting reconnection...");
            
            esp_err_t ret = wifi_manager_connect();
            if (ret == ESP_OK) {
                ESP_LOGI(TAG, "WiFi connected successfully");
                retry_count = 0;
            } else {
                retry_count++;
                ESP_LOGE(TAG, "Connection failed, retry %d/%d", retry_count, MAX_RETRY);
                
                if (retry_count >= MAX_RETRY) {
                    ESP_LOGW(TAG, "Starting captive portal...");
                    wifi_manager_start_ap_mode();
                    retry_count = 0;
                }
            }
            
            vTaskDelay(pdMS_TO_TICKS(5000));
        } else {
            // Monitor connection quality
            int8_t rssi = wifi_manager_get_rssi();
            if (rssi < -80) {
                ESP_LOGW(TAG, "Weak signal: %d dBm", rssi);
            }
            
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
    }
}
```

## Connection States

```
[Disconnected] -> [Connecting] -> [Connected]
       ^              |                |
       |              v                |
       |         [Failed]              |
       |              |                |
       |              v                |
       +-------- [AP Mode] <-----------+
```

## Connection Strategies

### Strategy 1: Saved Credentials
1. Read SSID/password from NVS
2. Attempt connection
3. Wait for success/failure event

### Strategy 2: Captive Portal
1. Start AP mode (SSID: "SmartHome_Setup")
2. Run HTTP server on 192.168.4.1
3. User connects and enters credentials
4. Save to NVS and reconnect

## Task Configuration

- Task name: "wifi"
- Stack size: 4096 bytes
- Priority: 3 (high)
- Check interval: 10 seconds (connected), 5 seconds (disconnected)

## WiFi Configuration

```c
// STA Mode defaults
#define WIFI_SSID_MAX_LEN     32
#define WIFI_PASSWORD_MAX_LEN 64
#define WIFI_CONNECT_TIMEOUT  10000  // 10 seconds

// AP Mode defaults
#define AP_SSID          "SmartHome_Setup"
#define AP_PASSWORD      "12345678"
#define AP_MAX_CONN      4
#define AP_CHANNEL       1
```

## Event Handling

```c
// WiFi event callbacks
void on_wifi_connected(void) {
    ESP_LOGI(TAG, "WiFi connected");
    // Trigger MQTT connection
}

void on_wifi_disconnected(void) {
    ESP_LOGW(TAG, "WiFi disconnected");
    // Disconnect MQTT
}

void on_wifi_got_ip(esp_ip4_addr_t ip) {
    ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&ip));
}
```

## RSSI Monitoring

```c
int8_t rssi = wifi_manager_get_rssi();

if (rssi > -50) {
    ESP_LOGI(TAG, "Excellent signal: %d dBm", rssi);
} else if (rssi > -70) {
    ESP_LOGI(TAG, "Good signal: %d dBm", rssi);
} else if (rssi > -80) {
    ESP_LOGW(TAG, "Fair signal: %d dBm", rssi);
} else {
    ESP_LOGE(TAG, "Poor signal: %d dBm", rssi);
}
```

## Captive Portal Interface

- URL: http://192.168.4.1
- Pages: login.html, index.html
- API: POST /api/wifi (SSID, password)
- Response: JSON status message

## Integration Points

```c
// Check connection before MQTT
if (wifi_manager_is_connected()) {
    mqtt_manager_connect();
}

// Reset WiFi from button
if (button_long_press) {
    wifi_manager_reset();
}
```

## Dependencies

- wifi_manager
- webserver
- NVS Flash
- ESP-IDF WiFi
- FreeRTOS

## Overview

WiFi event handling task that processes WiFi manager events and triggers MQTT connection. Also provides WiFi connecting LED blink task for visual feedback during connection.

## Features

- WiFi event callback handling
- MQTT auto-start on IP acquisition
- LED blink during connection
- Provisioning mode support

## File Structure

```
task_wifi/
    CMakeLists.txt
    task_wifi.c
    include/
        task_wifi.h
```

## API Reference

### Functions

| Function | Return | Description |
|----------|--------|-------------|
| `task_wifi_event_callback(event, data)` | `void` | Handle WiFi events |
| `task_wifi_set_wifi_connecting_init()` | `esp_err_t` | Start LED blink task |

## WiFi Events Handled

| Event | Action |
|-------|--------|
| WIFI_EVENT_DISCONNECTED | Log disconnection |
| WIFI_EVENT_CONNECTING | Set isWiFiConnecting, start LED blink |
| WIFI_EVENT_CONNECTED | Log connection |
| WIFI_EVENT_GOT_IP | Stop LED blink, start MQTT client |
| WIFI_EVENT_PROVISIONING_STARTED | Log AP info |
| WIFI_EVENT_PROVISIONING_FAILED | Log error |
| WIFI_EVENT_PROVISIONING_SUCCESS | Log success, notify restart |

## Task Configuration

| Parameter | Value |
|-----------|-------|
| Task Name | `wifi_connecting` |
| Stack Size | 2048 bytes |
| Priority | 5 |
| Blink Interval | 250ms |

## LED Blink Behavior

```
WiFi Connecting:
    isWiFiConnecting = true
    |
    +-- Every 250ms:
            isWiFi = !isWiFi  (toggle LED)
    |
Got IP:
    isWiFiConnecting = false
    isWiFi = true (LED stays ON)
```

## Event Flow

```
wifi_manager
    |
    +-- WIFI_EVENT_CONNECTING
    |       isWiFiConnecting = true
    |       LED starts blinking
    |
    +-- WIFI_EVENT_GOT_IP
            isWiFiConnecting = false
            LED stays ON
            mqtt_manager_start()
```

## Usage Example

```c
#include "task_wifi.h"

void app_main(void)
{
    wifi_manager_init();
    
    // Register event callback
    wifi_manager_register_event_callback(task_wifi_event_callback);
    
    // Start LED blink task
    task_wifi_set_wifi_connecting_init();
    
    // Start WiFi
    wifi_manager_start();
}
```

## Dependencies

- `wifi_manager` - WiFi events
- `mqtt_manager` - MQTT client start
- `status_led` - LED control (via isWiFi flag)