# Task Manager

## Overview

Global configuration and header aggregation module. Provides centralized access to all task headers, manager headers, and global configuration variables. Acts as a single include point for application layer.

## Features

- Aggregates all task headers
- Aggregates all manager headers
- Defines global configuration from Kconfig
- Exposes global state flags

## File Structure

```
task_manager/
    CMakeLists.txt
    Kconfig
    task_manager.c
    include/
        task_manager.h
```

## Configuration (Kconfig)

```
VERSION_APP           # Application version (default: "1.0")
INTERVAL_TIME_MS      # Task interval in ms (default: 5000)
```

## API Reference

### Defines

```c
#define VERSION_APP         CONFIG_VERSION_APP
#define INTERVAL_TIME_MS    CONFIG_INTERVAL_TIME_MS
```

### Exported Variables

| Variable | Type | Source | Description |
|----------|------|--------|-------------|
| `isModeON` | bool | mode_manager.c | Device mode state |
| `isWiFi` | bool | wifi_manager.c | WiFi connection state |
| `isMQTT` | bool | mqtt_manager.c | MQTT connection state |
| `g_app_version` | char[16] | task_manager.c | Version string |
| `g_interval_time_ms` | uint32_t | task_manager.c | Publish interval |

### Included Headers

**Task Headers:**
- task_init.h
- task_button.h
- task_status.h
- task_wifi.h
- task_mqtt.h
- task_mode.h
- task_display.h

**Manager Headers:**
- button_handler.h
- device_control.h
- status_led.h
- mode_manager.h
- sensor_manager.h
- sensor_reader.h
- wifi_manager.h
- mqtt_manager.h

## Usage Example

```c
#include "task_manager.h"

void app_main(void)
{
    // Access global config
    ESP_LOGI(TAG, "Version: %s", g_app_version);
    ESP_LOGI(TAG, "Interval: %lu ms", g_interval_time_ms);
    
    // Access state flags
    if (isWiFi && isMQTT)
    {
        ESP_LOGI(TAG, "Connected to cloud");
    }
}
```

## Dependencies

- All application task components
- All manager components