# Sensor Reader Module

## Overview

High-level unified API for reading sensor data from all managed sensors. Provides single-call access to temperature, humidity, light, and timestamp data.

## Features

- Single function call to read all sensors
- Automatic validity checking
- Continues on partial sensor failures
- Structured data return
- Detailed logging

## API Functions

```c
esp_err_t sensor_reader_read_all(sensor_data_t *data);
```

## Data Types

### sensor_data_t

```c
typedef struct {
    float temperature;  // Temperature in Celsius
    float humidity;     // Relative humidity in percent
    uint16_t light;     // Light intensity in lux
    uint32_t timestamp; // Unix timestamp
    bool valid;         // True if all sensors read successfully
} sensor_data_t;
```

## Usage Example

```c
#include "sensor_manager.h"
#include "sensor_reader.h"

// Initialize sensors first
sensor_manager_init_default();

// Read all sensors
sensor_data_t data;
esp_err_t ret = sensor_reader_read_all(&data);

if (ret == ESP_OK) {
    if (data.valid) {
        // All sensors read successfully
        printf("Temperature: %.2f°C\n", data.temperature);
        printf("Humidity: %.2f%%\n", data.humidity);
        printf("Light: %u lux\n", data.light);
        printf("Timestamp: %lu\n", data.timestamp);
    } else {
        // Partial success - check individual values
        printf("Warning: Some sensors failed\n");
    }
}
```

## Behavior

- Returns `ESP_OK` even if some sensors fail
- Sets `valid` flag to `false` if any sensor fails
- Continues reading remaining sensors after failures
- Logs individual sensor successes and failures
- Updates sensor availability flags in sensor_manager

## Reading Strategy

1. Reads DS3231 timestamp
2. Reads SHT3x temperature and humidity
3. Reads BH1750 light level
4. Sets valid flag based on all three successes
5. Returns data structure with all available readings

## Dependencies

- sensor_manager (must be initialized first)
- Individual sensor drivers (ds3231, sht3x, bh1750)

## Features

- Single function to read all sensors
- Automatic error handling per sensor
- Validity flag for data integrity check
- Graceful degradation if sensor fails

## API Reference

```c
esp_err_t sensor_reader_read_all(sensor_data_t *data);
```

## Data Structure

```c
typedef struct {
    float temperature;   // Temperature in Celsius (SHT3x)
    float humidity;      // Relative humidity % (SHT3x)
    uint16_t light;      // Light intensity lux (BH1750)
    uint32_t timestamp;  // Unix timestamp (DS3231)
    bool valid;          // True if all sensors read OK
} sensor_data_t;
```

## Usage Example

```c
#include "sensor_manager.h"
#include "sensor_reader.h"

// Initialize first
sensor_manager_init_default();

// Read all sensors
sensor_data_t data;
esp_err_t ret = sensor_reader_read_all(&data);

if (ret == ESP_OK) {
    if (data.valid) {
        printf("All sensors OK\n");
        printf("Temp: %.2f C\n", data.temperature);
        printf("Humidity: %.2f%%\n", data.humidity);
        printf("Light: %d lux\n", data.light);
        printf("Time: %lu\n", data.timestamp);
    } else {
        // Partial data available
        printf("Some sensors failed\n");
    }
}
```

## Error Handling

| Scenario | Behavior |
|----------|----------|
| All sensors OK | `valid = true` |
| One sensor fails | `valid = false`, other data available |
| Manager not init | Returns `ESP_ERR_INVALID_STATE` |
| NULL pointer | Returns `ESP_ERR_INVALID_ARG` |

## Dependencies

- sensor_manager (must be initialized first)
- ds3231, sht3x, bh1750 drivers
