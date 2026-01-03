# Task Status

## Overview

Status LED polling task that monitors global state flags and updates corresponding status LEDs. Polls at 50ms intervals for responsive LED updates.

## Features

- Monitors isModeON, isWiFi, isMQTT flags
- Updates LED_DEVICE, LED_WIFI, LED_MQTT
- Change detection to minimize updates
- Low overhead polling task

## File Structure

```
task_status/
    CMakeLists.txt
    task_status.c
    include/
        task_status.h
```

## API Reference

### Functions

| Function | Return | Description |
|----------|--------|-------------|
| `task_status_set_init()` | `esp_err_t` | Create LED polling task |

## Task Configuration

| Parameter | Value |
|-----------|-------|
| Task Name | `led_polling` |
| Stack Size | 2048 bytes |
| Priority | 5 |
| Poll Interval | 50ms |

## LED Mapping

| Global Flag | LED | Description |
|-------------|-----|-------------|
| `isModeON` | LED_DEVICE | Device mode indicator |
| `isWiFi` | LED_WIFI | WiFi connection indicator |
| `isMQTT` | LED_MQTT | MQTT connection indicator |

## Task Flow

```
led_polling_task()
    |
    +-- Every 50ms:
            |
            +-- Check isModeON
            |       if changed: status_led_set_state(LED_DEVICE, state)
            |
            +-- Check isWiFi
            |       if changed: status_led_set_state(LED_WIFI, state)
            |
            +-- Check isMQTT
                    if changed: status_led_set_state(LED_MQTT, state)
```

## Usage Example

```c
#include "task_status.h"

void app_main(void)
{
    // Initialize status LEDs first
    status_led_init();
    
    // Start polling task
    task_status_set_init();
    
    // LEDs now automatically reflect state flags
}
```

## Dependencies

- `status_led` - LED control
- `mode_manager` - isModeON flag
- `wifi_manager` - isWiFi flag
- `mqtt_manager` - isMQTT flag