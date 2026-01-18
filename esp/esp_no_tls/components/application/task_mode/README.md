# Mode Control Task

## Overview

FreeRTOS task for managing device operation mode lifecycle and coordinating mode-dependent behaviors across system components.

## Features

- Mode state monitoring
- Mode change coordination
- Component notification
- Interval adjustment
- State persistence
- Event-driven architecture

## Task Function

```c
void mode_task(void *pvParameters);
```

## Mode States

- **MODE_OFF**: Minimal activity, sensors idle, no MQTT publishing
- **MODE_ON**: Full operation, sensor reading active, data publishing enabled

## Usage Example

```c
#include "task_mode.h"
#include "task_manager.h"

void app_main(void) {
    // Initialize mode manager
    mode_manager_init();
    
    // Create mode task (typically done by task_manager)
    xTaskCreate(mode_task, "mode", 2048, NULL, 2, NULL);
    
    // Task monitors mode changes and coordinates system response
}
```

## Task Loop

```c
void mode_task(void *pvParameters) {
    device_mode_t current_mode = mode_manager_get_mode();
    device_mode_t previous_mode = current_mode;
    
    while (1) {
        current_mode = mode_manager_get_mode();
        
        // Detect mode change
        if (current_mode != previous_mode) {
            ESP_LOGI(TAG, "Mode changed: %s -> %s",
                     previous_mode == MODE_ON ? "ON" : "OFF",
                     current_mode == MODE_ON ? "ON" : "OFF");
            
            if (current_mode == MODE_ON) {
                // Activate components
                enable_sensor_reading();
                enable_mqtt_publishing();
            } else {
                // Deactivate components
                disable_sensor_reading();
                disable_mqtt_publishing();
            }
            
            previous_mode = current_mode;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Mode Transitions

```
[MODE_OFF] <---> [MODE_ON]
    |                |
    v                v
[Idle State]  [Active State]
```

### Transition Actions

**OFF → ON:**
- Enable sensor reading task
- Enable MQTT publishing task
- Activate status LED
- Start data collection

**ON → OFF:**
- Suspend sensor reading
- Suspend MQTT publishing
- Update status LED
- Preserve current state

## Task Configuration

- Task name: "mode"
- Stack size: 2048 bytes
- Priority: 2 (medium)
- Check interval: 1 second

## Mode Change Triggers

1. **Button Press**: BTN4 toggles mode
2. **MQTT Command**: "device/mode" topic
3. **Scheduled Event**: Timer-based automation
4. **Error Recovery**: Automatic fallback to OFF

## Integration Points

```c
// Check global mode flag
if (isModeON) {
    // Perform sensor reading
    read_sensors();
    publish_data();
}

// Register mode change callback
void on_mode_changed(device_mode_t old, device_mode_t new) {
    if (new == MODE_ON) {
        start_operations();
    } else {
        stop_operations();
    }
}

mode_manager_register_change_callback(on_mode_changed);
```

## State Persistence

Mode state stored in NVS:
- Key: "device_mode"
- Value: 0 (OFF) or 1 (ON)
- Restored on boot

## Dependencies

- mode_manager
- FreeRTOS

## Overview

Display update and sensor reading task. Runs continuously to update the OLED display every second and read sensors at configured intervals. Manages display content based on current device mode (ON/OFF).

## Features

- 1-second display update loop
- Configurable sensor read interval
- Mode-aware display rendering
- Updates shared_sensor data for other tasks
- Reads time from DS3231 RTC

## File Structure

```
task_mode/
    CMakeLists.txt
    task_mode.c
    include/
        task_mode.h
```

## API Reference

### Functions

| Function | Return | Description |
|----------|--------|-------------|
| `task_mode_init()` | `esp_err_t` | Create display update task |
| `task_mode_change_event_callback(old, new)` | `void` | Handle mode changes |
| `task_mode_stop()` | `void` | Stop display task |

## Task Configuration

| Parameter | Value |
|-----------|-------|
| Task Name | `display_task` |
| Stack Size | 6144 bytes |
| Priority | 4 |
| Update Interval | 1000ms |

## Operation Modes

### MODE_ON (Normal)
- Read sensors at `g_interval_time_ms` interval
- Update shared_sensor with new readings
- Render full UI (time + sensors + info)

### MODE_OFF
- Skip sensor reading
- Display time only

## Task Flow

```
display_update_task()
    |
    +-- Every 1 second:
    |       |
    |       +-- Read time from DS3231
    |       |
    |       +-- if (MODE_ON):
    |       |       |
    |       |       +-- if (interval elapsed):
    |       |       |       Read sensors
    |       |       |       Update shared_sensor
    |       |       |
    |       |       +-- Render full UI
    |       |
    |       +-- else (MODE_OFF):
    |               |
    |               +-- Render time only
    |
    +-- vTaskDelayUntil(1000ms)
```

## Usage Example

```c
#include "task_mode.h"

void app_main(void)
{
    // After task_init()
    task_mode_init();
    
    // Task runs automatically
}

// Mode change callback (registered in task_init)
void task_mode_change_event_callback(device_mode_t old_mode, device_mode_t new_mode)
{
    ESP_LOGI(TAG, "Mode: %s -> %s",
             old_mode == MODE_ON ? "ON" : "OFF",
             new_mode == MODE_ON ? "ON" : "OFF");
}
```

## Dependencies

- `task_display` - Display rendering
- `sensor_manager` - Get timestamp
- `sensor_reader` - Read sensor values
- `shared_sensor` - Store sensor data
- `mode_manager` - Get current mode