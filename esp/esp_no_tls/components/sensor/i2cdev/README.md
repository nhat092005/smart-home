# I2C Device Abstraction Layer

## Overview

Hardware abstraction layer for I2C devices using ESP-IDF I2C Master API. Provides thread-safe device management and simplified communication interface.

## Features

- ESP-IDF I2C Master API support
- Device descriptor management
- Mutex-based thread safety
- Bus initialization and device registration
- Multi-device bus sharing
- Error handling and logging

## Architecture

```
Application Driver
       |
   i2cdev API
       |
ESP-IDF I2C Master
       |
   Hardware I2C
```

## API Functions

### Bus Management

```c
esp_err_t i2c_bus_init(int port, gpio_num_t sda_gpio, gpio_num_t scl_gpio, uint32_t clk_speed);
```

### Device Management

```c
esp_err_t i2c_dev_init(i2c_dev_t *dev);
esp_err_t i2c_dev_create_mutex(i2c_dev_t *dev);
esp_err_t i2c_dev_delete_mutex(i2c_dev_t *dev);
```

### Communication

```c
esp_err_t i2c_dev_read(const i2c_dev_t *dev, const void *out_data, size_t out_size, 
                       void *in_data, size_t in_size);
esp_err_t i2c_dev_write(const i2c_dev_t *dev, const void *out_reg, size_t out_reg_size, 
                        const void *out_data, size_t out_data_size);
esp_err_t i2c_dev_read_reg(const i2c_dev_t *dev, uint8_t reg, void *in_data, size_t in_size);
esp_err_t i2c_dev_write_reg(const i2c_dev_t *dev, uint8_t reg, const void *out_data, 
                            size_t out_data_size);
```

## Usage Example

```c
#include "i2cdev.h"

// Initialize I2C bus once
i2c_bus_init(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22, 100000);

// Setup device descriptor
i2c_dev_t device = {
    .port = I2C_NUM_0,
    .addr = 0x44,
    .sda_io_num = GPIO_NUM_21,
    .scl_io_num = GPIO_NUM_22,
    .clk_speed = 100000
};

// Add device to bus
i2c_dev_init(&device);
i2c_dev_create_mutex(&device);

// Read from device
uint8_t data[2];
i2c_dev_read_reg(&device, 0x00, data, 2);

// Write to device
uint8_t value = 0x42;
i2c_dev_write_reg(&device, 0x10, &value, 1);

// Cleanup
i2c_dev_delete_mutex(&device);
```

## Thread Safety

Use provided macros for mutex management in drivers:

```c
I2C_DEV_TAKE_MUTEX(dev);
// Critical section
I2C_DEV_GIVE_MUTEX(dev);

// With error checking
I2C_DEV_CHECK(dev, i2c_dev_read_reg(dev, reg, data, size));
```

## Configuration

Default settings in `i2cdev_config.h`:
- I2C Master Port: 0
- SDA Pin: GPIO 21
- SCL Pin: GPIO 22
- Frequency: 100 kHz

## Dependencies

- ESP-IDF I2C Master driver
- FreeRTOS

## Features

- Single I2C bus initialization shared by all devices
- Per-device mutex for thread-safe access
- Register read/write operations
- Raw data read/write operations
- Kconfig menu for pin configuration

## API Reference

### Bus Initialization

```c
esp_err_t i2c_bus_init(int port, gpio_num_t sda_gpio, gpio_num_t scl_gpio, uint32_t clk_speed);
```

### Device Operations

```c
esp_err_t i2c_dev_init(i2c_dev_t *dev);           // Add device to bus
esp_err_t i2c_dev_create_mutex(i2c_dev_t *dev);   // Create thread mutex
esp_err_t i2c_dev_delete_mutex(i2c_dev_t *dev);   // Delete thread mutex
```

### Data Transfer

```c
esp_err_t i2c_dev_read_reg(i2c_dev_t *dev, uint8_t reg, void *data, size_t len);
esp_err_t i2c_dev_write_reg(i2c_dev_t *dev, uint8_t reg, const void *data, size_t len);
esp_err_t i2c_dev_read(i2c_dev_t *dev, void *data, size_t len);
esp_err_t i2c_dev_write(i2c_dev_t *dev, const void *data, size_t len);
```

## Device Descriptor

```c
typedef struct {
    int port;                // I2C port number (0 or 1)
    uint8_t addr;            // 7-bit I2C address
    gpio_num_t sda_io_num;   // SDA GPIO pin
    gpio_num_t scl_io_num;   // SCL GPIO pin
    uint32_t clk_speed;      // Clock speed in Hz
    SemaphoreHandle_t mutex; // Thread-safe mutex
    void *dev_handle;        // Internal device handle
} i2c_dev_t;
```

## Thread Safety Macros

```c
I2C_DEV_TAKE_MUTEX(dev)   // Acquire mutex before operation
I2C_DEV_GIVE_MUTEX(dev)   // Release mutex after operation
I2C_DEV_CHECK(dev, func)  // Check result and release mutex on error
```

## Configuration via Menuconfig

Run `idf.py menuconfig` and navigate to `I2C Device Configuration`:

| Option | Default | Description |
|--------|---------|-------------|
| I2C Master SDA GPIO | 21 | Data line pin |
| I2C Master SCL GPIO | 22 | Clock line pin |
| I2C Master Frequency | 100000 | Clock speed (Hz) |
| I2C Master Port | 0 | Port number (0 or 1) |
| I2C Transaction Timeout | 1000 | Timeout in ms |
| I2C Debug Logging | disabled | Enable verbose logging |

## Configuration Macros

Defined in `i2cdev_config.h`:

```c
I2C_MASTER_SDA_PIN   // From CONFIG_I2C_MASTER_SDA_PIN
I2C_MASTER_SCL_PIN   // From CONFIG_I2C_MASTER_SCL_PIN
I2C_MASTER_FREQ_HZ   // From CONFIG_I2C_MASTER_FREQ_HZ
I2C_TIMEOUT_MS       // From CONFIG_I2CDEV_TIMEOUT_MS
I2CDEV_DEBUG         // From CONFIG_I2CDEV_DEBUG
```
