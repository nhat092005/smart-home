# Mode Manager

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
