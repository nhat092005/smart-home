# Task Manager Module

## Overview

Centralized FreeRTOS task creation and lifecycle management. Provides coordinated startup of application tasks with proper priority assignment and stack allocation.

## Features

- Centralized task creation
- Priority level management
- Stack size configuration
- Task handle management
- Creation error handling
- Clean initialization sequence

## Task Architecture

| Task Name | Priority | Stack Size | Function |
|-----------|----------|------------|----------|
| init | 4 | 4096 | System initialization |
| wifi | 3 | 4096 | WiFi management |
| mqtt | 3 | 5120 | MQTT communication |
| display | 2 | 3072 | OLED display updates |
| button | 3 | 3072 | Button input handling |
| mode | 2 | 2048 | Mode control logic |
| status | 1 | 2048 | Status LED control |

## API Functions

### Task Management

```c
esp_err_t task_manager_init(void);
```

## Task Handles

```c
extern TaskHandle_t task_init_handle;
extern TaskHandle_t task_wifi_handle;
extern TaskHandle_t task_mqtt_handle;
extern TaskHandle_t task_display_handle;
extern TaskHandle_t task_button_handle;
extern TaskHandle_t task_mode_handle;
extern TaskHandle_t task_status_handle;
```

## Usage Example

```c
#include "task_manager.h"
#include "esp_log.h"

static const char *TAG = "APP";

void app_main(void) {
    // Initialize task manager (creates all tasks)
    esp_err_t ret = task_manager_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Task manager init failed: %s", esp_err_to_name(ret));
        return;
    }
    
    ESP_LOGI(TAG, "All tasks created successfully");
    
    // Task handles are now available for control
    if (task_wifi_handle != NULL) {
        ESP_LOGI(TAG, "WiFi task running");
    }
    
    // Main function can return; tasks continue running
}
```

## Task Creation Sequence

1. **init_task**: System initialization and configuration
2. **wifi_task**: WiFi connection management
3. **mqtt_task**: MQTT broker connection
4. **display_task**: OLED display updates
5. **button_task**: Input event handling
6. **mode_task**: Operation mode control
7. **status_task**: Status indicator management

## Priority Assignment

Priority levels (0-4, higher = more urgent):
- **Priority 4**: Critical initialization
- **Priority 3**: Network communication and input
- **Priority 2**: Display and mode control
- **Priority 1**: Status indicators

## Stack Sizing Guidelines

- **5120 bytes**: MQTT task (TLS/SSL overhead)
- **4096 bytes**: WiFi and init tasks (network stack)
- **3072 bytes**: Display and button tasks (moderate operations)
- **2048 bytes**: Mode and status tasks (lightweight logic)

## Error Handling

Returns:
- ESP_OK: All tasks created successfully
- ESP_FAIL: One or more task creations failed

Task creation failures logged with ESP_LOGE.

## Configuration

Defaults in task_manager.c:
```c
#define TASK_INIT_STACK_SIZE      4096
#define TASK_WIFI_STACK_SIZE      4096
#define TASK_MQTT_STACK_SIZE      5120
#define TASK_DISPLAY_STACK_SIZE   3072
#define TASK_BUTTON_STACK_SIZE    3072
#define TASK_MODE_STACK_SIZE      2048
#define TASK_STATUS_STACK_SIZE    2048
```

## Dependencies

- FreeRTOS
- task_init
- task_wifi
- task_mqtt
- task_display
- task_button
- task_mode
- task_status

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