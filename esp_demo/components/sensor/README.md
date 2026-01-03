# Sensor Component

I2C sensor drivers and management layer for Smart Home project.

## Structure

```
sensor/
  i2cdev/          - I2C bus abstraction layer
  bh1750/          - Light sensor driver (0-65535 lux)
  sht3x/           - Temperature and humidity sensor
  ds3231/          - Real-time clock module
  sh1106/          - OLED display driver (128x64)
  sensor_manager/  - Unified sensor initialization
  sensor_reader/   - Unified sensor reading
```

## Hardware Configuration

| Sensor | I2C Address | Function |
|--------|-------------|----------|
| DS3231 | 0x68 | RTC timestamp |
| SHT3x | 0x44 or 0x45 | Temperature, Humidity |
| BH1750 | 0x23 or 0x5C | Light intensity |
| SH1106 | 0x3C | OLED display |

## Default I2C Pins

Configured in `i2cdev_config.h`:
- SDA: GPIO21
- SCL: GPIO22
- Speed: 100kHz

## Quick Start

```c
#include "sensor_manager.h"
#include "sensor_reader.h"

// Initialize all sensors
sensor_manager_init_default();

// Read all sensor data
sensor_data_t data;
sensor_reader_read_all(&data);

if (data.valid) {
    printf("Temp: %.2f C, Humidity: %.2f%%, Light: %d lux\n",
           data.temperature, data.humidity, data.light);
}
```

## Dependencies

- ESP-IDF driver component
- FreeRTOS
