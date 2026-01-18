# BH1750 Light Sensor Driver

## Overview

Driver for BH1750 digital ambient light sensor with I2C interface. Supports multiple measurement modes and resolutions.

## Features

- One-time and continuous measurement modes
- Three resolution levels: low (4 lx), high (1 lx), high2 (0.5 lx)
- Measurement time adjustment support
- Thread-safe operations with mutex protection
- Automatic power management

## Hardware Specifications

- Measurement range: 1 - 65535 lux
- I2C addresses: 0x23 (ADDR pin low/floating), 0x5C (ADDR pin high)
- Supply voltage: 2.4V - 3.6V
- Active current: 120 µA

## API Functions

### Initialization

```c
esp_err_t bh1750_init_desc(bh1750_t *dev, uint8_t addr, i2c_port_t port, 
                           gpio_num_t sda_gpio, gpio_num_t scl_gpio);
esp_err_t bh1750_free_desc(bh1750_t *dev);
```

### Configuration

```c
esp_err_t bh1750_setup(bh1750_t *dev, bh1750_mode_t mode, bh1750_resolution_t resolution);
```

### Measurement

```c
esp_err_t bh1750_read_light(bh1750_t *dev, uint16_t *lux);
esp_err_t bh1750_read_light_basic(bh1750_t *dev, uint16_t *lux);
```

## Usage Example

```c
#include "bh1750.h"

bh1750_t sensor;

// Initialize
bh1750_init_desc(&sensor, BH1750_ADDR_LO, I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);
i2c_dev_init(&sensor.i2c_dev);

// Configure for high resolution continuous mode
bh1750_setup(&sensor, BH1750_MODE_CONTINUOUS, BH1750_RES_HIGH);

// Read light level
uint16_t lux;
bh1750_read_light(&sensor, &lux);
printf("Light: %u lux\n", lux);

// Cleanup
bh1750_free_desc(&sensor);
```

## Measurement Modes

- **BH1750_MODE_ONE_TIME**: Single measurement, auto power-down
- **BH1750_MODE_CONTINUOUS**: Continuous measurement, stays powered

## Resolution Levels

- **BH1750_RES_LOW**: 4 lx resolution, ~16ms measurement
- **BH1750_RES_HIGH**: 1 lx resolution, ~120ms measurement
- **BH1750_RES_HIGH2**: 0.5 lx resolution, ~120ms measurement

## Dependencies

- i2cdev abstraction layer
- FreeRTOS
