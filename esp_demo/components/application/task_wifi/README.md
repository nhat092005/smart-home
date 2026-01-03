# Task WiFi

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