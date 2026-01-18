# Shared Sensor Data Module

## Overview

Thread-safe sensor data sharing across FreeRTOS tasks. Provides mutex-protected access to current sensor readings for display, MQTT publishing, and logging.

## Features

- Centralized sensor data storage
- Mutex-based thread synchronization
- Atomic read/write operations
- Latest sensor reading cache
- Task coordination support

## Data Structure

```c
typedef struct {
    float temperature;     // Temperature in °C
    float humidity;        // Relative humidity in %
    uint16_t light;        // Light level in lux
    char datetime[32];     // ISO 8601 datetime string
} sensor_data_t;
```

## API Functions

### Initialization

```c
void shared_sensor_init(void);
```

### Data Access

```c
void shared_sensor_set_data(sensor_data_t *data);
void shared_sensor_get_data(sensor_data_t *data);
```

## Usage Example

```c
#include "shared_sensor.h"

// Task 1: Sensor reading task
void sensor_reading_task(void *pvParameters) {
    sensor_data_t readings;
    
    while (1) {
        // Read sensors
        readings.temperature = read_temperature();
        readings.humidity = read_humidity();
        readings.light = read_light();
        snprintf(readings.datetime, sizeof(readings.datetime), 
                 "%04d-%02d-%02d %02d:%02d:%02d",
                 year, month, day, hour, minute, second);
        
        // Update shared data (thread-safe)
        shared_sensor_set_data(&readings);
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// Task 2: Display task
void display_task(void *pvParameters) {
    sensor_data_t current_data;
    
    while (1) {
        // Get current readings (thread-safe)
        shared_sensor_get_data(&current_data);
        
        // Update display
        display_temperature(current_data.temperature);
        display_humidity(current_data.humidity);
        display_light(current_data.light);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Task 3: MQTT publishing task
void mqtt_task(void *pvParameters) {
    sensor_data_t data;
    
    while (1) {
        if (is_connected()) {
            // Get latest data (thread-safe)
            shared_sensor_get_data(&data);
            
            // Publish to MQTT
            publish_sensor_data(&data);
        }
        
        vTaskDelay(pdMS_TO_TICKS(interval_seconds * 1000));
    }
}

void app_main(void) {
    // Initialize shared data module
    shared_sensor_init();
    
    // Create tasks
    xTaskCreate(sensor_reading_task, "sensor", 4096, NULL, 5, NULL);
    xTaskCreate(display_task, "display", 3072, NULL, 4, NULL);
    xTaskCreate(mqtt_task, "mqtt", 5120, NULL, 4, NULL);
}
```

## Thread Safety

- All operations protected by FreeRTOS mutex
- Blocking access with timeout
- Copy-based data transfer
- No shared pointers

## Synchronization Details

```c
static SemaphoreHandle_t data_mutex = NULL;

void shared_sensor_set_data(sensor_data_t *data) {
    if (xSemaphoreTake(data_mutex, portMAX_DELAY)) {
        memcpy(&shared_data, data, sizeof(sensor_data_t));
        xSemaphoreGive(data_mutex);
    }
}
```

## Dependencies

- FreeRTOS (mutex support)
- Standard library (string.h)

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
