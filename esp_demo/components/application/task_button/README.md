# Task Button

## Overview

Button event processing task that handles button press callbacks through a FreeRTOS queue. Callbacks from button_handler ISR are queued and processed in a separate task context, enabling safe execution of blocking operations like MQTT publish.

## Features

- Event queue for deferred button processing
- Safe execution of blocking operations
- Handles 5 buttons: MODE, WIFI, LIGHT, FAN, AC
- Automatic MQTT state publish after device changes

## File Structure

```
task_button/
    CMakeLists.txt
    task_button.c
    include/
        task_button.h
```

## API Reference

### Functions

| Function | Return | Description |
|----------|--------|-------------|
| `task_button_init()` | `esp_err_t` | Create queue and processing task |
| `task_button_wifi_pressed(button)` | `void` | Queue WIFI button event |
| `task_button_mode_pressed(button)` | `void` | Queue MODE button event |
| `task_button_light_pressed(button)` | `void` | Queue LIGHT button event |
| `task_button_fan_pressed(button)` | `void` | Queue FAN button event |
| `task_button_ac_pressed(button)` | `void` | Queue AC button event |

## Button Actions

| Button | Action |
|--------|--------|
| BUTTON_WIFI | Clear WiFi credentials and restart to provisioning |
| BUTTON_MODE | Toggle device mode (ON/OFF) |
| BUTTON_LIGHT | Toggle light output |
| BUTTON_FAN | Toggle fan output |
| BUTTON_AC | Toggle AC output |

## Task Configuration

| Parameter | Value |
|-----------|-------|
| Task Name | `button_proc` |
| Stack Size | 4096 bytes |
| Priority | 6 (high) |
| Queue Size | 10 events |

## Architecture

```
+----------------+     callback      +-------------+
| button_handler | ----------------> | task_button |
|     (ISR)      |    (queue send)   |  callbacks  |
+----------------+                   +------+------+
                                            |
                                     xQueueSend()
                                            |
                                            v
                                    +---------------+
                                    |  Event Queue  |
                                    | (10 events)   |
                                    +-------+-------+
                                            |
                                     xQueueReceive()
                                            |
                                            v
                                    +---------------+
                                    | button_proc   |
                                    | task (pri 6)  |
                                    +-------+-------+
                                            |
                    +-----------------------+-----------------------+
                    |           |           |           |           |
                    v           v           v           v           v
                wifi_clear  mode_toggle  light_tog   fan_tog    ac_tog
```

## Usage Example

```c
#include "task_button.h"
#include "button_handler.h"

void app_main(void)
{
    // Initialize button processing
    task_button_init();
    
    // Register callbacks with button_handler
    button_handler_set_callback(BUTTON_MODE, task_button_mode_pressed);
    button_handler_set_callback(BUTTON_WIFI, task_button_wifi_pressed);
    button_handler_set_callback(BUTTON_LIGHT, task_button_light_pressed);
    button_handler_set_callback(BUTTON_FAN, task_button_fan_pressed);
    button_handler_set_callback(BUTTON_AC, task_button_ac_pressed);
}
```

## Dependencies

- `button_handler` - Button callback registration
- `device_control` - Device state control
- `mode_manager` - Mode toggle
- `wifi_manager` - WiFi credential clearing
- `task_mqtt` - State publishing
