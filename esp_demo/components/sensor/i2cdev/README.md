# I2C Device Abstraction Layer

Thread-safe I2C communication layer using ESP-IDF I2C Master API.

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
