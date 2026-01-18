# Display Update Task

## Overview

FreeRTOS task for periodic OLED display updates with sensor data, system status, and network information.

## Features

- Real-time sensor data display
- WiFi connection status
- MQTT broker connection status
- Device operation mode indicator
- Multi-page display rotation
- Automatic refresh cycle

## Task Function

```c
void display_task(void *pvParameters);
```

## Display Layout

### Page 1: Sensor Data
```
+--------------------+
|  Smart Home        |
|  Temp: 25.5°C      |
|  Hum:  65.2%       |
|  Light: 450 lux    |
|  Mode: ON          |
+--------------------+
```

### Page 2: Network Status
```
+--------------------+
|  Network Status    |
|  WiFi: Connected   |
|  MQTT: Connected   |
|  IP: 192.168.1.100 |
|  Time: 14:30:25    |
+--------------------+
```

## Usage Example

```c
#include "task_display.h"
#include "task_manager.h"

void app_main(void) {
    // Initialize display hardware
    i2cdev_init();
    sh1106_init();
    shared_sensor_init();
    
    // Create display task (typically done by task_manager)
    xTaskCreate(display_task, "display", 3072, NULL, 2, NULL);
    
    // Task continuously updates display with latest data
}
```

## Task Loop

```c
void display_task(void *pvParameters) {
    sensor_data_t data;
    
    while (1) {
        // Get latest sensor readings
        shared_sensor_get_data(&data);
        
        // Clear display
        sh1106_clear();
        
        // Display sensor data
        sh1106_set_cursor(0, 0);
        sh1106_printf("Temp: %.1f°C", data.temperature);
        
        sh1106_set_cursor(0, 16);
        sh1106_printf("Hum: %.1f%%", data.humidity);
        
        sh1106_set_cursor(0, 32);
        sh1106_printf("Light: %d lux", data.light);
        
        // Display mode
        sh1106_set_cursor(0, 48);
        sh1106_printf("Mode: %s", isModeON ? "ON" : "OFF");
        
        // Update display
        sh1106_refresh();
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Display Modes

- **Normal Mode**: Continuous sensor data updates
- **Error Mode**: Display error messages
- **Setup Mode**: Show configuration information

## Task Configuration

- Task name: "display"
- Stack size: 3072 bytes
- Priority: 2 (medium)
- Update interval: 1 second

## Font and Graphics

- Font: 8x8 pixel characters
- Display: 128x64 pixels (16 columns × 8 rows)
- Graphics: Line drawing, rectangles, icons

## Power Management

```c
// Display sleep when idle
if (idle_timeout > 300) {
    sh1106_display_off();
}

// Wake on activity
if (button_pressed || new_data) {
    sh1106_display_on();
    idle_timeout = 0;
}
```

## Data Refresh Strategy

1. Read shared sensor data (thread-safe)
2. Check WiFi connection status
3. Check MQTT connection status
4. Update display buffer
5. Refresh screen
6. Sleep 1 second

## Dependencies

- sh1106 (OLED driver)
- shared_sensor
- mode_manager
- wifi_manager
- mqtt_manager
- FreeRTOS

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