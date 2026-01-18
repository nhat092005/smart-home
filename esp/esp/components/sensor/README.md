# Sensor Component

## Overview

Sensor drivers and management modules for the ESP32 Smart Home system. Provides unified I2C device interface and high-level sensor reading APIs.

## Modules

### Hardware Drivers

- **bh1750** - BH1750 ambient light sensor driver
- **ds3231** - DS3231 real-time clock with temperature compensation
- **sht3x** - SHT3x temperature and humidity sensor driver
- **sh1106** - SH1106 OLED display driver (128x64)
- **i2cdev** - I2C device abstraction layer using ESP-IDF I2C master API

### Management Layers

- **sensor_manager** - Centralized initialization and device descriptor management
- **sensor_reader** - High-level unified sensor reading interface

## Architecture

```
Application
    |
sensor_reader (Unified API)
    |
sensor_manager (Device Management)
    |
+--------+--------+--------+--------+
|        |        |        |        |
ds3231  sht3x   bh1750  sh1106    (Drivers)
|        |        |        |        |
+--------+--------+--------+--------+
              |
           i2cdev (Abstraction Layer)
              |
         ESP-IDF I2C Master
```

## Quick Start

```c
#include "sensor_manager.h"
#include "sensor_reader.h"

// Initialize all sensors
sensor_manager_init_default();

// Read all sensors
sensor_data_t data;
sensor_reader_read_all(&data);

if (data.valid) {
    printf("Temperature: %.2f°C\n", data.temperature);
    printf("Humidity: %.2f%%\n", data.humidity);
    printf("Light: %u lux\n", data.light);
    printf("Timestamp: %lu\n", data.timestamp);
}
```

## I2C Configuration

Default pins defined in `i2cdev_config.h`:
- SDA: GPIO 21
- SCL: GPIO 22
- Frequency: 100 kHz

## Dependencies

- ESP-IDF I2C driver
- FreeRTOS
```

## Dependencies

- ESP-IDF driver component
- FreeRTOS
