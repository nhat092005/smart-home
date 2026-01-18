# Mode Manager Module

## Overview

Device operation mode management with NVS persistence. Controls system-wide ON/OFF state affecting sensor reading and data publishing behavior.

## Features

- Two operation modes: MODE_ON and MODE_OFF
- NVS-based persistence across reboots
- Global state variable export (isModeON)
- Mode change callback system
- Configurable data publish interval
- Thread-safe mode operations

## Operation Modes

- **MODE_ON**: Normal operation with sensor reading and MQTT publishing
- **MODE_OFF**: Idle state with minimal activity

## API Functions

### Initialization

```c
esp_err_t mode_manager_init(void);
```

### Mode Control

```c
esp_err_t mode_manager_set_mode(device_mode_t mode);
esp_err_t mode_manager_toggle_mode(void);
device_mode_t mode_manager_get_mode(void);
```

### Event Callbacks

```c
void mode_manager_register_change_callback(mode_change_callback_t callback);
```

## Usage Example

```c
#include "mode_manager.h"

void on_mode_changed(device_mode_t old_mode, device_mode_t new_mode) {
    printf("Mode changed: %s -> %s\n",
           old_mode == MODE_ON ? "ON" : "OFF",
           new_mode == MODE_ON ? "ON" : "OFF");
}

// Initialize
mode_manager_init();
mode_manager_register_change_callback(on_mode_changed);

// Set mode
mode_manager_set_mode(MODE_ON);

// Toggle mode
mode_manager_toggle_mode();

// Check current mode
if (mode_manager_get_mode() == MODE_ON) {
    printf("System is active\n");
}

// Access global state
if (isModeON) {
    // Perform sensor operations
}
```

## Global Variables

```c
extern bool isModeON;                  // Device mode state
extern unsigned int interval_seconds;  // Publish interval
```

## Callback Type

```c
typedef void (*mode_change_callback_t)(device_mode_t old_mode, device_mode_t new_mode);
```

## Configuration

Defaults in mode_manager.h:
- DEFAULT_INTERVAL: 5 seconds
- MIN_INTERVAL: 1 second
- MAX_INTERVAL: 3600 seconds (1 hour)
- STATE_BACKUP_INTERVAL: 60 seconds

## Dependencies

- NVS Flash
- FreeRTOS

## Overview

The Mode Manager component handles device operation mode (ON/OFF) with NVS persistence. It provides mode state management, change callbacks, and exposes the global `isModeON` flag for other components.

## Features

- Two modes: MODE_OFF (0) and MODE_ON (1)
- NVS persistence across reboots
- Mode change callback notification
- Global `isModeON` flag for LED status
- Configurable data publish interval

## File Structure

```
mode_manager/
    CMakeLists.txt
    mode_manager.c
    include/
        mode_manager.h
```

## API Reference

### Types

```c
typedef enum {
    MODE_OFF = 0,    // Device off
    MODE_ON = 1      // Device on (normal operation)
} device_mode_t;

typedef void (*mode_change_callback_t)(device_mode_t old_mode, device_mode_t new_mode);
```

### Functions

| Function | Return | Description |
|----------|--------|-------------|
| `mode_manager_init()` | `esp_err_t` | Initialize and load mode from NVS |
| `mode_manager_set_mode(mode)` | `esp_err_t` | Set mode and save to NVS |
| `mode_manager_toggle_mode()` | `esp_err_t` | Toggle between ON/OFF |
| `mode_manager_get_mode()` | `device_mode_t` | Get current mode |
| `mode_manager_register_change_callback(cb)` | `void` | Register mode change callback |

### Exported Variables

| Variable | Type | Description |
|----------|------|-------------|
| `isModeON` | bool | Global mode state (true=ON, false=OFF) |
| `interval_seconds` | unsigned int | Data publish interval |

### Defines

```c
#define DEFAULT_INTERVAL        5       // Default publish interval (seconds)
#define MIN_INTERVAL            1       // Minimum interval
#define MAX_INTERVAL            3600    // Maximum interval (1 hour)
#define STATE_BACKUP_INTERVAL   60      // State publish interval
```

## NVS Storage

| Namespace | Key | Type | Description |
|-----------|-----|------|-------------|
| `mode_config` | `device_mode` | uint8_t | Current device mode |

## Usage Example

```c
#include "mode_manager.h"

// Mode change callback
void on_mode_change(device_mode_t old_mode, device_mode_t new_mode)
{
    ESP_LOGI(TAG, "Mode: %s -> %s",
             old_mode == MODE_ON ? "ON" : "OFF",
             new_mode == MODE_ON ? "ON" : "OFF");
}

void app_main(void)
{
    mode_manager_init();
    mode_manager_register_change_callback(on_mode_change);
    
    // Toggle mode on button press
    mode_manager_toggle_mode();
    
    // Check current mode
    if (mode_manager_get_mode() == MODE_ON)
    {
        // Normal operation
    }
}
```

## Dependencies

- `nvs_flash` - Non-volatile storage
- `esp_log` - Logging
