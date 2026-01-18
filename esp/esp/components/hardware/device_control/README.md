# Device Control Module

## Overview

Output device control module for managing home appliances via relay switches. Provides thread-safe control for fan, light, and AC devices.

## Features

- Three independent device channels
- ON/OFF/Toggle operations
- Thread-safe state management with mutex
- State persistence and query
- Configurable GPIO pins and active levels

## Controlled Devices

- **DEVICE_FAN**: Fan relay control
- **DEVICE_LIGHT**: Light relay control
- **DEVICE_AC**: Air conditioner relay control

## API Functions

### Initialization

```c
esp_err_t device_control_init(void);
esp_err_t device_control_deinit(void);
```

### Control Operations

```c
esp_err_t device_control_set_state(device_type_t device, device_state_t state);
esp_err_t device_control_get_state(device_type_t device, device_state_t *state);
esp_err_t device_control_toggle(device_type_t device);
```

## Usage Example

```c
#include "device_control.h"

// Initialize
device_control_init();

// Turn on fan
device_control_set_state(DEVICE_FAN, DEVICE_ON);

// Toggle light
device_control_toggle(DEVICE_LIGHT);

// Check AC state
device_state_t state;
device_control_get_state(DEVICE_AC, &state);
if (state == DEVICE_ON) {
    printf("AC is running\n");
}

// Turn off all devices
device_control_set_state(DEVICE_FAN, DEVICE_OFF);
device_control_set_state(DEVICE_LIGHT, DEVICE_OFF);
device_control_set_state(DEVICE_AC, DEVICE_OFF);
```

## Data Types

### device_type_t

```c
typedef enum {
    DEVICE_FAN = 0,
    DEVICE_LIGHT,
    DEVICE_AC,
    DEVICE_MAX
} device_type_t;
```

### device_state_t

```c
typedef enum {
    DEVICE_OFF = 0,
    DEVICE_ON = 1
} device_state_t;
```

## Configuration

GPIO pins configurable via Kconfig:
- DEVICE_FAN_PIN: Default GPIO 16
- DEVICE_LIGHT_PIN: Default GPIO 17
- DEVICE_AC_PIN: Default GPIO 19

## Dependencies

- ESP-IDF GPIO driver
- FreeRTOS

## Features

- 3 output device channels
- ON/OFF/Toggle operations
- Thread-safe with mutex protection
- Configurable active level (HIGH/LOW)

## Devices

| Device | Type | Default GPIO |
|--------|------|--------------|
| `DEVICE_FAN` | Fan relay | GPIO16 |
| `DEVICE_LIGHT` | Light relay | GPIO17 |
| `DEVICE_AC` | AC relay | GPIO19 |

## API Reference

### Initialization

```c
esp_err_t device_control_init(void);
esp_err_t device_control_deinit(void);
```

### Control Functions

```c
esp_err_t device_control_set_state(device_type_t device, device_state_t state);
esp_err_t device_control_get_state(device_type_t device, device_state_t *state);
esp_err_t device_control_toggle(device_type_t device);
```

## Data Types

```c
typedef enum {
    DEVICE_FAN = 0,
    DEVICE_LIGHT,
    DEVICE_AC,
    DEVICE_MAX
} device_type_t;

typedef enum {
    DEVICE_OFF = 0,
    DEVICE_ON = 1
} device_state_t;
```

## Configuration via Menuconfig

Run `idf.py menuconfig` and navigate to `Device Control Configuration`:

| Option | Default | Description |
|--------|---------|-------------|
| Device Active Level | 1 | 0=Active LOW, 1=Active HIGH |
| Fan Control GPIO | 16 | Fan output pin |
| Light Control GPIO | 17 | Light output pin |
| AC Control GPIO | 19 | AC output pin |

## Usage Example

```c
#include "device_control.h"

// Initialize
device_control_init();

// Turn on light
device_control_set_state(DEVICE_LIGHT, DEVICE_ON);

// Toggle fan
device_control_toggle(DEVICE_FAN);

// Get AC state
device_state_t ac_state;
device_control_get_state(DEVICE_AC, &ac_state);
printf("AC is %s\n", ac_state == DEVICE_ON ? "ON" : "OFF");

// Turn off all devices before cleanup
device_control_set_state(DEVICE_FAN, DEVICE_OFF);
device_control_set_state(DEVICE_LIGHT, DEVICE_OFF);
device_control_set_state(DEVICE_AC, DEVICE_OFF);
device_control_deinit();
```

## Notes

- All devices start in OFF state after initialization
- Deinit automatically turns off all devices
- Thread-safe for multi-task access