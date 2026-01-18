# SH1106 OLED Display Driver

## Overview

Driver for SH1106 128x64 monochrome OLED display controller with I2C interface.

## Features

- 128x64 pixel resolution
- Monochrome display (1-bit per pixel)
- Buffered graphics operations
- Pixel-level drawing control
- I2C communication
- Internal charge pump

## Hardware Specifications

- Resolution: 128x64 pixels
- Display colors: Monochrome (white/blue on black)
- I2C address: 0x3C (default)
- Supply voltage: 3.3V - 5V
- Active current: ~20mA

## API Functions

### Initialization

```c
esp_err_t sh1106_init_desc(sh1106_t *dev, uint8_t addr, i2c_port_t port, 
                           gpio_num_t sda_gpio, gpio_num_t scl_gpio);
esp_err_t sh1106_free_desc(sh1106_t *dev);
esp_err_t sh1106_init(sh1106_t *dev);
```

### Drawing Operations

```c
void sh1106_set_pixel(sh1106_t *dev, int x, int y, uint8_t color);
void sh1106_draw_horizontal_line(sh1106_t *dev, int y);
void sh1106_clear_display(sh1106_t *dev);
esp_err_t sh1106_update_display(sh1106_t *dev);
```

## Usage Example

```c
#include "sh1106.h"

sh1106_t display;

// Initialize
sh1106_init_desc(&display, SH1106_I2C_ADDR_DEFAULT, I2C_NUM_0, 
                 GPIO_NUM_21, GPIO_NUM_22);
i2c_dev_init(&display.i2c_dev);
sh1106_init(&display);

// Clear screen
sh1106_clear_display(&display);

// Draw pixels
sh1106_set_pixel(&display, 10, 10, 1);  // White pixel
sh1106_set_pixel(&display, 20, 20, 1);

// Draw horizontal line at y=32
sh1106_draw_horizontal_line(&display, 32);

// Update display to show changes
sh1106_update_display(&display);

// Cleanup
sh1106_free_desc(&display);
```

## Display Buffer

The driver uses a 1024-byte buffer (128x64/8) for graphics operations. All drawing functions modify this buffer. Call `sh1106_update_display()` to transfer the buffer to the display hardware.

## Coordinate System

- Origin (0,0) is at top-left corner
- X-axis: 0 to 127 (left to right)
- Y-axis: 0 to 63 (top to bottom)

## Color Values

- 0: Pixel off (black)
- 1: Pixel on (white/blue depending on display)

## Dependencies

- i2cdev abstraction layer
- FreeRTOS

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
