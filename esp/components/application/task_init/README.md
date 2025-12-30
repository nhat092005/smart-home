# Task Init

## Overview

System initialization orchestrator that initializes all hardware and software components in the correct order. Called once from app_main() at system startup.

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