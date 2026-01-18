# Hardware Component

## Overview

GPIO hardware control drivers for physical inputs and outputs in the ESP32 Smart Home system.

## Modules

### button_handler

Physical button input handling with debouncing and event callbacks. Supports 5 buttons for device control and system functions.

### device_control

Relay output control for home appliances. Manages 3 devices (fan, light, AC) with ON/OFF/Toggle operations and state tracking.

### status_led

Status indicator LEDs for visual feedback. Controls 3 LEDs showing device mode, WiFi connection, and MQTT connection status.

## Hardware Configuration

| Module | Count | Default GPIOs | Function |
|--------|-------|---------------|----------|
| Buttons | 5 | 32, 33, 25, 26, 23 | Mode, WiFi, Light, Fan, AC |
| Devices | 3 | 16, 17, 19 | Fan, Light, AC relays |
| LEDs | 3 | 27, 14, 13 | Device, WiFi, MQTT status |

## Quick Start

```c
#include "button_handler.h"
#include "device_control.h"
#include "status_led.h"

// Initialize all hardware
button_handler_init();
device_control_init();
status_led_init();

// Set button callback
button_handler_set_callback(BUTTON_FAN, on_fan_button_pressed);

// Control device
device_control_set_state(DEVICE_FAN, DEVICE_ON);

// Update LED
status_led_set_state(LED_DEVICE, LED_ON);
```

## Dependencies

- ESP-IDF GPIO driver
- FreeRTOS