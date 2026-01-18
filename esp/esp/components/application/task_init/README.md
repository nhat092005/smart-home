# Initialization Task Module

## Overview

System initialization orchestrator running as high-priority FreeRTOS task. Coordinates startup sequence for NVS, WiFi, I2C, sensors, peripherals, and MQTT callbacks.

## Features

- Sequential initialization flow
- Error handling and recovery
- Component dependency management
- MQTT command registration
- Self-deleting task pattern
- Initialization status logging

## Initialization Sequence

1. **NVS Flash**: Non-volatile storage initialization
2. **WiFi**: Network stack and WiFi manager
3. **Mode Manager**: Device operation mode
4. **Shared Sensor**: Inter-task data structure
5. **I2C Bus**: Hardware communication initialization
6. **Sensor Manager**: All I2C sensor setup
7. **Button Handler**: GPIO input configuration
8. **Device Control**: Relay GPIO setup
9. **Status LED**: LED GPIO setup
10. **MQTT Callbacks**: Command handler registration
11. **Task Cleanup**: Self-deletion after completion

## Task Function

```c
void init_task(void *pvParameters);
```

## Usage Example

```c
#include "task_init.h"
#include "task_manager.h"

void app_main(void) {
    // Create initialization task (typically done by task_manager)
    xTaskCreate(init_task, "init", 4096, NULL, 4, NULL);
    
    // Task will:
    // 1. Initialize all components
    // 2. Register MQTT callbacks
    // 3. Delete itself when complete
}
```

## MQTT Command Registration

Registered command handlers:

| Topic | Handler | Function |
|-------|---------|----------|
| device/mode | on_mode_command | Toggle operation mode |
| device/light | on_light_command | Control light relay |
| device/fan | on_fan_command | Control fan relay |
| device/ac | on_ac_command | Control AC relay |

## Command Handler Example

```c
void on_mode_command(const char *topic, const char *data, int len) {
    if (len == 2 && strncmp(data, "ON", 2) == 0) {
        mode_manager_set_mode(MODE_ON);
    } else if (len == 3 && strncmp(data, "OFF", 3) == 0) {
        mode_manager_set_mode(MODE_OFF);
    }
}

void on_light_command(const char *topic, const char *data, int len) {
    if (len == 2 && strncmp(data, "ON", 2) == 0) {
        device_control_set(DEVICE_LIGHT, 1);
    } else if (len == 3 && strncmp(data, "OFF", 3) == 0) {
        device_control_set(DEVICE_LIGHT, 0);
    }
}
```

## Error Handling

Each initialization step checks return codes:
```c
ret = wifi_manager_init();
if (ret != ESP_OK) {
    ESP_LOGE(TAG, "WiFi manager init failed: %s", esp_err_to_name(ret));
    // Continue with other components
}
```

## Task Lifecycle

1. **Creation**: Spawned by task_manager with priority 4
2. **Execution**: Sequential initialization of all components
3. **Registration**: MQTT callback setup
4. **Deletion**: Self-terminates with vTaskDelete(NULL)

## Stack Requirements

- Stack size: 4096 bytes
- Priority: 4 (highest in system)
- Execution: One-time at startup
- Duration: 2-5 seconds typical

## Configuration

No runtime configuration required. Initialization parameters defined in component headers:
- I2C: Pins 21 (SDA), 22 (SCL)
- WiFi: SSID/password from NVS
- MQTT: Broker from NVS or defaults

## Dependencies

- NVS Flash
- wifi_manager
- mode_manager
- shared_sensor
- i2cdev
- sensor_manager
- button_handler
- device_control
- status_led
- mqtt_callback
- FreeRTOS

## Features

- NVS flash initialization with error recovery
- Hardware component initialization (LEDs, buttons, I2C)
- Sensor and display initialization
- Mode manager initialization
- WiFi and MQTT initialization
- Callback registration

## File Structure

```
task_init/
    CMakeLists.txt
    task_init.c
    include/
        task_init.h
```

## API Reference

### Functions

| Function | Return | Description |
|----------|--------|-------------|
| `task_init()` | `void` | Initialize all system components |

## Initialization Sequence

```
task_init()
    |
    +-- 1. task_init_nvs()
    |       Initialize NVS flash (erase if corrupted)
    |
    +-- 2. task_init_status_led()
    |       status_led_init()
    |       task_status_set_init()
    |
    +-- 3. task_init_button()
    |       device_control_init()
    |       button_handler_init()
    |       task_button_init()
    |       Register button callbacks
    |
    +-- 4. task_init_hardware_protocol()
    |       sensor_manager_init(SDA, SCL)
    |
    +-- 5. shared_sensor_data_init()
    |       Initialize shared sensor storage
    |
    +-- 6. task_init_mode_manager()
    |       mode_manager_init()
    |       Register mode change callback
    |
    +-- 7. task_init_display()
    |       task_display_init()
    |
    +-- 8. task_init_wifi()
    |       wifi_manager_init()
    |       Register WiFi event callback
    |       Start WiFi or provisioning
    |
    +-- 9. task_init_mqtt()
            mqtt_manager_init()
            mqtt_callback_init()
            task_mqtt_init()
```

## External Callbacks Registered

| Callback | Target | Description |
|----------|--------|-------------|
| `task_button_mode_pressed` | BUTTON_MODE | Mode button handler |
| `task_button_wifi_pressed` | BUTTON_WIFI | WiFi button handler |
| `task_button_light_pressed` | BUTTON_LIGHT | Light button handler |
| `task_button_fan_pressed` | BUTTON_FAN | Fan button handler |
| `task_button_ac_pressed` | BUTTON_AC | AC button handler |
| `task_mode_change_event_callback` | mode_manager | Mode change handler |
| `task_wifi_event_callback` | wifi_manager | WiFi event handler |

## Usage Example

```c
#include "task_init.h"
#include "task_mode.h"

void app_main(void)
{
    // Initialize all components
    task_init();
    
    // Start operational tasks
    task_mode_init();
}
```

## Dependencies

- All hardware components
- All communication components
- All application tasks