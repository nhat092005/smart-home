# Status LED Task

## Overview

FreeRTOS task for system status indication via RGB LED patterns. Provides visual feedback for WiFi, MQTT, and operation mode states.

## Features

- Multi-state LED indication
- WiFi connection status
- MQTT connection status
- Operation mode display
- LED blink patterns
- Priority-based indication

## Task Function

```c
void status_task(void *pvParameters);
```

## LED Status Patterns

| State | LED Color | Pattern | Meaning |
|-------|-----------|---------|----------|
| Initializing | Yellow | Solid | System startup |
| WiFi Disconnected | Red | Slow blink (1Hz) | No WiFi connection |
| WiFi Connected | Green | Solid | WiFi connected, MQTT pending |
| MQTT Connected | Blue | Solid | Full connectivity |
| Mode OFF | Off | - | Device idle |
| Error | Red | Fast blink (5Hz) | System error |

## Usage Example

```c
#include "task_status.h"
#include "task_manager.h"

void app_main(void) {
    // Initialize status LED hardware
    status_led_init();
    wifi_manager_init();
    mqtt_manager_init();
    mode_manager_init();
    
    // Create status task (typically done by task_manager)
    xTaskCreate(status_task, "status", 2048, NULL, 1, NULL);
    
    // Task continuously updates LED based on system state
}
```

## Task Loop

```c
void status_task(void *pvParameters) {
    while (1) {
        // Check mode first (highest priority)
        if (!isModeON) {
            status_led_set_color(LED_OFF);
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        
        // Check WiFi connection
        if (!wifi_manager_is_connected()) {
            status_led_blink(LED_RED, 500, 500);  // Slow blink
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        
        // Check MQTT connection
        if (!mqtt_manager_is_connected()) {
            status_led_set_color(LED_GREEN);  // WiFi only
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        
        // Full connectivity
        status_led_set_color(LED_BLUE);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## LED Colors

```c
typedef enum {
    LED_OFF = 0,
    LED_RED,      // WiFi error
    LED_GREEN,    // WiFi connected
    LED_BLUE,     // MQTT connected
    LED_YELLOW,   // Initializing
    LED_MAGENTA,  // Custom state
    LED_CYAN,     // Custom state
    LED_WHITE     // Custom state
} led_color_t;
```

## Blink Functions

```c
// Solid color
status_led_set_color(LED_BLUE);

// Blink pattern
status_led_blink(LED_RED, 100, 100);  // Fast blink
status_led_blink(LED_GREEN, 500, 500);  // Slow blink

// Turn off
status_led_set_color(LED_OFF);
```

## State Priority

1. **Mode OFF** (highest) - Turn off LED
2. **WiFi Disconnected** - Red slow blink
3. **MQTT Disconnected** - Green solid
4. **Full Connected** (lowest) - Blue solid

## Task Configuration

- Task name: "status"
- Stack size: 2048 bytes
- Priority: 1 (low)
- Update interval: 1 second

## Hardware Mapping

| LED | GPIO | Function |
|-----|------|----------|
| Red | 27 | WiFi/error indication |
| Green | 14 | WiFi connected |
| Blue | 13 | MQTT connected |

## Power Considerations

- LED current: ~20mA per color
- Duty cycle: 50% for blink patterns
- Sleep mode: LEDs off when mode is OFF

## Integration Points

```c
// WiFi status check
if (wifi_manager_is_connected()) {
    // Proceed with green/blue indication
}

// MQTT status check
if (mqtt_manager_is_connected()) {
    status_led_set_color(LED_BLUE);
}

// Mode status check
if (!isModeON) {
    status_led_set_color(LED_OFF);
}
```

## Dependencies

- status_led
- wifi_manager
- mqtt_manager
- mode_manager
- FreeRTOS

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