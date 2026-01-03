# Shared Sensor Data

## Overview

Thread-safe sensor data storage module providing a single source of truth for sensor readings. Data is written by task_mode and read by task_display and task_mqtt, ensuring consistent data across all consumers.

## Features

- Mutex-protected read/write operations
- Data validity flag
- Timestamp tracking
- 100ms timeout on mutex acquisition

## File Structure

```
shared_sensor/
    CMakeLists.txt
    shared_sensor.c
    include/
        shared_sensor.h
```

## API Reference

### Data Structure

```c
typedef struct {
    float temperature;      // Temperature in Celsius
    float humidity;         // Humidity in percent
    int light;              // Light intensity in lux
    uint32_t timestamp;     // Unix timestamp when data was read
    bool valid;             // Data validity flag
} shared_sensor_data_t;
```

### Functions

| Function | Return | Description |
|----------|--------|-------------|
| `shared_sensor_data_init()` | `esp_err_t` | Initialize mutex |
| `shared_sensor_data_update(temp, hum, light, ts)` | `esp_err_t` | Update sensor data (thread-safe) |
| `shared_sensor_data_get(data)` | `esp_err_t` | Get sensor data (thread-safe) |
| `shared_sensor_data_is_valid()` | `bool` | Check if data is valid |

## Data Flow

```
+-------------+     update()      +----------------+
|  task_mode  | ----------------> | shared_sensor  |
| (producer)  |                   | (mutex-locked) |
+-------------+                   +-------+--------+
                                          |
                    +---------------------+---------------------+
                    |                                           |
                    v get()                                     v get()
            +---------------+                           +---------------+
            | task_display  |                           |   task_mqtt   |
            |  (consumer)   |                           |  (consumer)   |
            +---------------+                           +---------------+
```

## Usage Example

```c
#include "shared_sensor.h"

// Producer (task_mode)
void update_sensors(void)
{
    float temp = 25.5f;
    float hum = 60.0f;
    int light = 500;
    uint32_t timestamp = get_current_time();
    
    shared_sensor_data_update(temp, hum, light, timestamp);
}

// Consumer (task_mqtt, task_display)
void read_sensors(void)
{
    shared_sensor_data_t data;
    
    if (shared_sensor_data_get(&data) == ESP_OK)
    {
        ESP_LOGI(TAG, "T=%.2f H=%.2f L=%d", 
                 data.temperature, data.humidity, data.light);
    }
}
```

## Error Handling

| Return Value | Description |
|--------------|-------------|
| `ESP_OK` | Operation successful |
| `ESP_ERR_NO_MEM` | Mutex creation failed |
| `ESP_ERR_INVALID_STATE` | Module not initialized or data invalid |
| `ESP_ERR_INVALID_ARG` | NULL pointer argument |
| `ESP_ERR_TIMEOUT` | Mutex acquisition timeout (100ms) |

## Thread Safety

- All public functions are thread-safe
- Uses FreeRTOS mutex with 100ms timeout
- Single writer (task_mode), multiple readers (task_display, task_mqtt)

## Dependencies

- `freertos/semphr.h` - Mutex operations
- `esp_log` - Logging
