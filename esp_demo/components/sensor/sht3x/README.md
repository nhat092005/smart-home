# SHT3x Temperature and Humidity Sensor Driver

Sensirion SHT3x series sensor driver for ESP32.

## Specifications

- Temperature: -40 to +125 C (accuracy +/- 0.2 C)
- Humidity: 0-100% RH (accuracy +/- 2%)
- I2C interface with CRC verification

## I2C Address

| ADDR Pin | Address |
|----------|---------|
| GND | 0x44 |
| VDD | 0x45 |

## API Reference

### Initialization

```c
esp_err_t sht3x_init_desc(sht3x_t *dev, uint8_t addr, i2c_port_t port,
                          gpio_num_t sda_gpio, gpio_num_t scl_gpio);
esp_err_t sht3x_init(sht3x_t *dev);
esp_err_t sht3x_free_desc(sht3x_t *dev);
```

### Measurement - High Level

```c
// Simple one-shot measurement (recommended)
esp_err_t sht3x_measure(sht3x_t *dev, float *temperature, float *humidity);
```

### Measurement - Low Level

```c
// Start measurement manually
esp_err_t sht3x_start_measurement(sht3x_t *dev, sht3x_mode_t mode, sht3x_repeat_t repeat);

// Get measurement duration in ticks
uint8_t sht3x_get_measurement_duration(sht3x_repeat_t repeat);

// Read raw data (6 bytes with CRC)
esp_err_t sht3x_get_raw_data(sht3x_t *dev, sht3x_raw_data_t raw_data);

// Convert raw data to values
esp_err_t sht3x_compute_values(sht3x_raw_data_t raw_data, float *temperature, float *humidity);

// Read and convert in one call
esp_err_t sht3x_get_results(sht3x_t *dev, float *temperature, float *humidity);
```

### Control

```c
esp_err_t sht3x_set_heater(sht3x_t *dev, bool enable);
esp_err_t sht3x_stop_periodic_measurement(sht3x_t *dev);
```

## Measurement Modes

| Mode | Rate |
|------|------|
| `SHT3X_SINGLE_SHOT` | On demand |
| `SHT3X_PERIODIC_05MPS` | 0.5/sec |
| `SHT3X_PERIODIC_1MPS` | 1/sec |
| `SHT3X_PERIODIC_2MPS` | 2/sec |
| `SHT3X_PERIODIC_4MPS` | 4/sec |
| `SHT3X_PERIODIC_10MPS` | 10/sec |

## Repeatability Options

| Level | Accuracy | Duration |
|-------|----------|----------|
| `SHT3X_HIGH` | Best | 15ms |
| `SHT3X_MEDIUM` | Balanced | 6ms |
| `SHT3X_LOW` | Fastest | 4ms |

## Usage Example

```c
#include "sht3x.h"

sht3x_t dev;
sht3x_init_desc(&dev, SHT3X_I2C_ADDR_GND, I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);
i2c_dev_init(&dev.i2c_dev);
sht3x_init(&dev);

float temp, hum;
sht3x_measure(&dev, &temp, &hum);
printf("Temp: %.2f C, Humidity: %.2f%%\n", temp, hum);
```

## Advanced Usage - Periodic Mode

```c
// Start periodic measurement at 1 Hz
sht3x_start_measurement(&dev, SHT3X_PERIODIC_1MPS, SHT3X_HIGH);

// Read results periodically
while (1) {
    float temp, hum;
    if (sht3x_get_results(&dev, &temp, &hum) == ESP_OK) {
        printf("Temp: %.2f C\n", temp);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
}

// Stop periodic mode
sht3x_stop_periodic_measurement(&dev);
```
