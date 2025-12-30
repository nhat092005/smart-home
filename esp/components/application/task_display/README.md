# Task Display

## Overview

OLED display rendering module for SH1106 128x64 display. Provides UI rendering with time, sensor data, and system information. Includes embedded 5x7 bitmap fonts for digits and letters.

## Features

- Full UI rendering with time, sensors, version, interval
- Partial time-only update for efficiency
- Centered message display
- Built-in 5x7 pixel fonts (digits 0-9, letters A-Z)
- Configurable display layout

## File Structure

```
task_display/
    CMakeLists.txt
    task_display.c
    include/
        task_display.h
```

## API Reference

### Data Structure

```c
typedef struct {
    int hour;               // Time hour (0-23)
    int minute;             // Time minute (0-59)
    int second;             // Time second (0-59)
    float temperature;      // Temperature in Celsius
    float humidity;         // Humidity in percent
    float light;            // Light level in lux
    const char *version;    // Firmware version string
    int interval;           // Update interval in seconds
} display_data_t;
```

### Functions

| Function | Return | Description |
|----------|--------|-------------|
| `task_display_init()` | `esp_err_t` | Initialize display interface |
| `task_display_render_full_ui(data)` | `void` | Render complete UI |
| `task_display_update_time(h, m, s)` | `void` | Update time only (faster) |
| `task_display_show_message(msg)` | `void` | Show centered message |

## Display Layout

```
+---------------------------+
|        HH:MM:SS           |  Y=4  (Time, size 2)
+---------------------------+
|  ----------------------   |  Y=22 (Separator)
|   25C    60%    500lux    |  Y=32 (Sensors)
|  ----------------------   |  Y=44 (Separator)
|  V1.0           INT:5s    |  Y=52 (Info)
+---------------------------+
```

## Font Data

- `font_5x7[]` - Digits 0-9 and colon
- `font_5x7_alpha[]` - Letters A-Z
- `font_dot[]` - Decimal point

## Usage Example

```c
#include "task_display.h"

void app_main(void)
{
    task_display_init();
    
    display_data_t data = {
        .hour = 14,
        .minute = 30,
        .second = 45,
        .temperature = 25.5f,
        .humidity = 60.0f,
        .light = 500.0f,
        .version = "1.0",
        .interval = 5
    };
    
    task_display_render_full_ui(&data);
}
```

## Dependencies

- `sensor_manager` - Get display device handle
- `sh1106` - OLED driver
- FreeRTOS - Task operations