# Sensor Manager Module

## Overview

Centralized initialization and management system for all I2C sensors in the Smart Home project. Handles device descriptor lifecycle and sensor availability tracking.

## Features

- Unified initialization for all sensors
- Automatic I2C bus configuration
- Device health monitoring
- Sensor availability flags
- Graceful degradation on sensor failures
- RTC timestamp management
- Display device access

## Managed Sensors

- DS3231 RTC
- SHT3x temperature/humidity sensor
- BH1750 light sensor
- SH1106 OLED display

## API Functions

### Initialization

```c
esp_err_t sensor_manager_init_default(void);
esp_err_t sensor_manager_init(gpio_num_t sda, gpio_num_t scl);
```

### Status

```c
esp_err_t sensor_manager_get_status(sensor_status_t *status);
```

### Timestamp Management

```c
esp_err_t sensor_manager_get_timestamp(uint32_t *timestamp);
esp_err_t sensor_manager_set_timestamp(uint32_t timestamp);
```

### Display Access

```c
sh1106_t *sensor_manager_get_display(void);
```

## Usage Example

```c
#include "sensor_manager.h"

// Initialize all sensors with default pins
esp_err_t ret = sensor_manager_init_default();

// Check sensor status
sensor_status_t status;
sensor_manager_get_status(&status);
printf("DS3231: %s\n", status.ds3231_ok ? "OK" : "FAIL");
printf("SHT3x: %s\n", status.sht3x_ok ? "OK" : "FAIL");
printf("BH1750: %s\n", status.bh1750_ok ? "OK" : "FAIL");

// Get current timestamp
uint32_t timestamp;
sensor_manager_get_timestamp(&timestamp);

// Set RTC time
sensor_manager_set_timestamp(1700000000);

// Access display
sh1106_t *display = sensor_manager_get_display();
if (display) {
    sh1106_clear_display(display);
    sh1106_update_display(display);
}
```

## Initialization Behavior

The manager initializes all sensors but continues operation even if some sensors fail. Use `sensor_manager_get_status()` to check which sensors are available.

## Data Types

### sensor_status_t

```c
typedef struct {
    bool ds3231_ok;
    bool sht3x_ok;
    bool bh1750_ok;
    bool sh1106_ok;
} sensor_status_t;
```

## Dependencies

- i2cdev abstraction layer
- Individual sensor drivers (ds3231, sht3x, bh1750, sh1106)
- FreeRTOS

## Features

- Single-call initialization for all sensors
- Automatic I2C bus setup
- Per-sensor health tracking
- Display device access
- Clean shutdown support

## Managed Devices

| Device | Type | Status Flag |
|--------|------|-------------|
| DS3231 | RTC | `ds3231_ready` |
| SHT3x | Temp/Humidity | `sht3x_ready` |
| BH1750 | Light | `bh1750_ready` |
| SH1106 | OLED Display | `sh1106_ready` |

## API Reference

### Initialization

```c
esp_err_t sensor_manager_init_default(void);  // Use default pins
esp_err_t sensor_manager_init(gpio_num_t sda, gpio_num_t scl);  // Custom pins
esp_err_t sensor_manager_deinit(void);
```

### Status

```c
esp_err_t sensor_manager_get_status(sensor_status_t *status);
```

### RTC Access

```c
esp_err_t sensor_manager_get_timestamp(uint32_t *timestamp);
esp_err_t sensor_manager_set_timestamp(uint32_t timestamp);
```

### Display Access

```c
sh1106_t *sensor_manager_get_display_device(void);
```

## Status Structure

```c
typedef struct {
    bool ds3231_ok;  // RTC responding
    bool sht3x_ok;   // Temp sensor responding
    bool bh1750_ok;  // Light sensor responding
    bool sh1106_ok;  // Display responding
} sensor_status_t;
```

## Usage Example

```c
#include "sensor_manager.h"
#include "sensor_reader.h"

// Initialize with default pins (GPIO21/22)
esp_err_t ret = sensor_manager_init_default();

// Or with custom pins
// sensor_manager_init(GPIO_NUM_18, GPIO_NUM_19);

// Check sensor status
sensor_status_t status;
sensor_manager_get_status(&status);
printf("DS3231: %s\n", status.ds3231_ok ? "OK" : "FAIL");
printf("SHT3x: %s\n", status.sht3x_ok ? "OK" : "FAIL");
printf("BH1750: %s\n", status.bh1750_ok ? "OK" : "FAIL");
printf("SH1106: %s\n", status.sh1106_ok ? "OK" : "FAIL");

// Set RTC time (Unix timestamp)
sensor_manager_set_timestamp(1703980800);

// Get display for custom drawing
sh1106_t *display = sensor_manager_get_display_device();
if (display) {
    sh1106_clear_display(display);
    sh1106_update_display(display);
}

// Cleanup
sensor_manager_deinit();
```

## Initialization Flow

1. Initialize I2C bus
2. For each sensor:
   - Initialize descriptor
   - Add device to I2C bus
   - Verify hardware communication
   - Set ready flag

## Notes

- Partial initialization allowed - failed sensors are skipped
- Check status flags before using specific sensors
- Must call init before using sensor_reader functions
