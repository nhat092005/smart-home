# Task Mode

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