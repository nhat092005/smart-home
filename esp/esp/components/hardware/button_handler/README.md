# Button Handler Module

## Overview

Physical button input manager with software debouncing and event-driven callbacks. Handles 5 tactile buttons for user interaction.

## Features

- Five button inputs with configurable GPIO pins
- Software debouncing using polling mechanism
- Event callback system for button press detection
- Active-low input configuration with internal pull-up
- Thread-safe button state access
- Dedicated FreeRTOS task for polling

## Supported Buttons

- **BUTTON_MODE**: System mode selection
- **BUTTON_WIFI**: WiFi provisioning trigger
- **BUTTON_LIGHT**: Light device toggle
- **BUTTON_FAN**: Fan device toggle
- **BUTTON_AC**: AC device toggle

## API Functions

### Initialization

```c
esp_err_t button_handler_init(void);
esp_err_t button_handler_deinit(void);
```

### Event Handling

```c
esp_err_t button_handler_set_callback(button_type_t button, button_callback_t callback);
```

### State Query

```c
bool button_handler_is_pressed(button_type_t button);
```

## Usage Example

```c
#include "button_handler.h"

void on_fan_pressed(button_type_t button) {
    printf("Fan button pressed\n");
    device_control_toggle(DEVICE_FAN);
}

void on_mode_pressed(button_type_t button) {
    printf("Mode button pressed\n");
    // Toggle system mode
}

// Initialize
button_handler_init();

// Register callbacks
button_handler_set_callback(BUTTON_FAN, on_fan_pressed);
button_handler_set_callback(BUTTON_MODE, on_mode_pressed);

// Query state
if (button_handler_is_pressed(BUTTON_LIGHT)) {
    printf("Light button is pressed\n");
}
```

## Configuration

GPIO pins configurable via Kconfig:
- BUTTON_MODE_PIN: Default GPIO 32
- BUTTON_WIFI_PIN: Default GPIO 33
- BUTTON_LIGHT_PIN: Default GPIO 25
- BUTTON_FAN_PIN: Default GPIO 26
- BUTTON_AC_PIN: Default GPIO 23

## Debouncing

Uses software debouncing with polling interval of 50ms. Requires multiple consecutive reads to register button press.

## Dependencies

- ESP-IDF GPIO driver
- FreeRTOS

## Features

- 5 configurable button inputs
- Software debounce filtering
- Callback function per button
- Polling-based detection
- Thread-safe operation

## Buttons

| Button | Type | Default GPIO |
|--------|------|--------------|
| `BUTTON_MODE` | Mode selection | GPIO32 |
| `BUTTON_WIFI` | WiFi control | GPIO33 |
| `BUTTON_LIGHT` | Light toggle | GPIO25 |
| `BUTTON_FAN` | Fan toggle | GPIO26 |
| `BUTTON_AC` | AC toggle | GPIO23 |

## API Reference

### Initialization

```c
esp_err_t button_handler_init(void);
esp_err_t button_handler_deinit(void);
```

### Callback Registration

```c
esp_err_t button_handler_set_callback(button_type_t button, button_callback_t callback);
```

### State Query

```c
bool button_handler_is_pressed(button_type_t button);
```

## Callback Type

```c
typedef void (*button_callback_t)(button_type_t button);
```

## Configuration via Menuconfig

Run `idf.py menuconfig` and navigate to `Button Handler Configuration`:

| Option | Default | Description |
|--------|---------|-------------|
| BUTTON_MODE GPIO | 32 | Mode button pin |
| BUTTON_WIFI GPIO | 33 | WiFi button pin |
| BUTTON_LIGHT GPIO | 25 | Light button pin |
| BUTTON_FAN GPIO | 26 | Fan button pin |
| BUTTON_AC GPIO | 23 | AC button pin |
| Poll Interval | 10ms | Button scan interval |
| Debounce Time | 50ms | Debounce filter time |

## Usage Example

```c
#include "button_handler.h"

void on_button_press(button_type_t button) {
    switch (button) {
        case BUTTON_MODE:
            printf("Mode button pressed\n");
            break;
        case BUTTON_LIGHT:
            printf("Light button pressed\n");
            break;
        default:
            break;
    }
}

// Initialize
button_handler_init();

// Register callbacks
button_handler_set_callback(BUTTON_MODE, on_button_press);
button_handler_set_callback(BUTTON_LIGHT, on_button_press);

// Check state directly
if (button_handler_is_pressed(BUTTON_FAN)) {
    printf("Fan button is pressed\n");
}
```

## Notes

- Buttons use internal pull-up resistors
- Active LOW configuration (pressed = LOW)
- Callbacks execute in polling task context