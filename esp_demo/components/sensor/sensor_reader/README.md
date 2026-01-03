# Sensor Reader

Unified sensor reading interface for all connected sensors.

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
