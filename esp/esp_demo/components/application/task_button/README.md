# Button Handling Task

## Overview

FreeRTOS task for button event monitoring and command execution. Implements non-blocking button state checking with action dispatch.

## Features

- Continuous button monitoring loop
- Integration with button_handler module
- Relay control commands
- Mode toggle support
- WiFi reset functionality
- Configurable polling interval

## Task Function

```c
void button_task(void *pvParameters);
```

## Button Mapping

| Button | GPIO | Primary Action | Long Press (if applicable) |
|--------|------|----------------|---------------------------|
| BTN1 | 32 | Toggle Light Relay | - |
| BTN2 | 33 | Toggle Fan Relay | - |
| BTN3 | 25 | Toggle AC Relay | - |
| BTN4 | 26 | Toggle Operation Mode | - |
| BTN5 | 23 | - | WiFi Reset (>3s) |

## Usage Example

```c
#include "task_button.h"
#include "task_manager.h"

void app_main(void) {
    // Initialize hardware first
    button_handler_init();
    device_control_init();
    mode_manager_init();
    
    // Create button task (typically done by task_manager)
    xTaskCreate(button_task, "button", 3072, NULL, 3, NULL);
    
    // Task continuously monitors button states and executes commands
}
```

## Task Loop

```c
void button_task(void *pvParameters) {
    while (1) {
        // Process all button events
        button_handler_process();
        
        // Check individual buttons
        if (button_handler_is_pressed(BUTTON_1)) {
            device_control_toggle(DEVICE_LIGHT);
            ESP_LOGI(TAG, "Light toggled");
        }
        
        if (button_handler_is_pressed(BUTTON_2)) {
            device_control_toggle(DEVICE_FAN);
            ESP_LOGI(TAG, "Fan toggled");
        }
        
        if (button_handler_is_pressed(BUTTON_3)) {
            device_control_toggle(DEVICE_AC);
            ESP_LOGI(TAG, "AC toggled");
        }
        
        if (button_handler_is_pressed(BUTTON_4)) {
            mode_manager_toggle_mode();
            ESP_LOGI(TAG, "Mode toggled");
        }
        
        if (button_handler_is_long_pressed(BUTTON_5)) {
            wifi_manager_reset();
            ESP_LOGI(TAG, "WiFi reset triggered");
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
```

## Button Events

- **Short Press**: Immediate action on button release
- **Long Press**: Action after 3 seconds hold (BTN5 only)
- **Debouncing**: Handled by button_handler module

## Task Configuration

- Task name: "button"
- Stack size: 3072 bytes
- Priority: 3 (high)
- Polling interval: 50ms

## State Diagram

```
[Idle] -> [Button Pressed] -> [Debounce] -> [Action] -> [Idle]
                                   |
                                   v
                            [Long Press Detected] -> [WiFi Reset]
```

## Integration Points

```c
// Device control
device_control_toggle(DEVICE_LIGHT);
device_control_set(DEVICE_FAN, 1);

// Mode management
mode_manager_toggle_mode();
mode_manager_set_mode(MODE_ON);

// WiFi management
wifi_manager_reset();
```

## Dependencies

- button_handler
- device_control
- mode_manager
- wifi_manager
- FreeRTOS

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
