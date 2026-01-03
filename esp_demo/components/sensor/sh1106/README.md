# SH1106 OLED Display Driver

128x64 monochrome OLED display driver for ESP32.

## Specifications

- Resolution: 128x64 pixels
- Interface: I2C
- Buffer size: 1024 bytes (128 x 64 / 8)

## I2C Address

Default address: 0x3C

## API Reference

### Initialization

```c
esp_err_t sh1106_init_desc(sh1106_t *dev, uint8_t addr, i2c_port_t port,
                           gpio_num_t sda_gpio, gpio_num_t scl_gpio);
esp_err_t sh1106_init(sh1106_t *dev);
esp_err_t sh1106_free_desc(sh1106_t *dev);
```

### Drawing Functions

```c
void sh1106_set_pixel(sh1106_t *dev, int x, int y, uint8_t color);
void sh1106_draw_horizontal_line(sh1106_t *dev, int y);
void sh1106_clear_display(sh1106_t *dev);
esp_err_t sh1106_update_display(sh1106_t *dev);
```

### Buffer Access

```c
uint8_t *sh1106_get_buffer(sh1106_t *dev);
void sh1106_get_dimensions(int *width, int *height);
```

## Buffer Format

- Each byte represents 8 vertical pixels
- `buffer[x + (y/8) * 128]` contains pixels at (x, y) to (x, y+7)
- Bit 0 = top pixel, Bit 7 = bottom pixel of the byte

## Usage Example

```c
#include "sh1106.h"

sh1106_t dev;
sh1106_init_desc(&dev, 0x3C, I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);
i2c_dev_init(&dev.i2c_dev);
sh1106_init(&dev);

// Clear and draw
sh1106_clear_display(&dev);
sh1106_set_pixel(&dev, 64, 32, 1);  // Center pixel on
sh1106_draw_horizontal_line(&dev, 16);
sh1106_update_display(&dev);  // Send buffer to display

// Direct buffer access for custom graphics
uint8_t *buffer = sh1106_get_buffer(&dev);
```

## Notes

- Always call `sh1106_update_display()` after modifying buffer
- `sh1106_clear_display()` only clears buffer, not hardware
- Color: 1 = white/on, 0 = black/off
