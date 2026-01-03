# BH1750 Light Sensor Driver

Digital ambient light sensor driver for ESP32.

## Specifications

- Measurement range: 1-65535 lux
- I2C interface
- Resolution: 1 lux (high), 0.5 lux (high2), 4 lux (low)

## I2C Address

| ADDR Pin | Address |
|----------|---------|
| Low/Float | 0x23 |
| High | 0x5C |

## API Reference

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

## Measurement Modes

| Mode | Description |
|------|-------------|
| `BH1750_MODE_ONE_TIME` | Single measurement, then power down |
| `BH1750_MODE_CONTINUOUS` | Continuous measurement |

## Resolution Options

| Resolution | Accuracy | Time |
|------------|----------|------|
| `BH1750_RES_LOW` | 4 lux | 16ms |
| `BH1750_RES_HIGH` | 1 lux | 120ms |
| `BH1750_RES_HIGH2` | 0.5 lux | 120ms |

## Usage Example

```c
#include "bh1750.h"

bh1750_t dev;
bh1750_init_desc(&dev, BH1750_ADDR_LO, I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);
i2c_dev_init(&dev.i2c_dev);
bh1750_setup(&dev, BH1750_MODE_CONTINUOUS, BH1750_RES_HIGH);

uint16_t lux;
bh1750_read_light(&dev, &lux);
printf("Light: %d lux\n", lux);
```
