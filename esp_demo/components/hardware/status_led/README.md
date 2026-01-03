# Status LED

Status LED indicators for Device, WiFi, and MQTT status.

## Features

- 3 status LED channels
- ON/OFF/Toggle operations
- Thread-safe with mutex protection
- Configurable active level (HIGH/LOW)

## LEDs

| LED | Purpose | Default GPIO |
|-----|---------|--------------|
| `LED_DEVICE` | Device/System status | GPIO27 |
| `LED_WIFI` | WiFi connection status | GPIO14 |
| `LED_MQTT` | MQTT connection status | GPIO13 |

## API Reference

### Initialization

```c
esp_err_t status_led_init(void);
esp_err_t status_led_deinit(void);
```

### Control Functions

```c
esp_err_t status_led_set_state(led_type_t led, led_state_t state);
esp_err_t status_led_get_state(led_type_t led, led_state_t *state);
esp_err_t status_led_toggle(led_type_t led);
```

## Data Types

```c
typedef enum {
    LED_DEVICE = 0,
    LED_WIFI,
    LED_MQTT,
    LED_MAX
} led_type_t;

typedef enum {
    LED_OFF = 0,
    LED_ON = 1
} led_state_t;
```

## Configuration via Menuconfig

Run `idf.py menuconfig` and navigate to `Status LED Configuration`:

| Option | Default | Description |
|--------|---------|-------------|
| LED Active Level | 1 | 0=Active LOW, 1=Active HIGH |
| LED DEVICE GPIO | 27 | Device status LED pin |
| LED WIFI GPIO | 14 | WiFi status LED pin |
| LED MQTT GPIO | 13 | MQTT status LED pin |

## Usage Example

```c
#include "status_led.h"

// Initialize
status_led_init();

// Turn on device LED (system running)
status_led_set_state(LED_DEVICE, LED_ON);

// WiFi connected
status_led_set_state(LED_WIFI, LED_ON);

// Toggle MQTT LED (for blinking effect)
status_led_toggle(LED_MQTT);

// Get LED state
led_state_t wifi_led;
status_led_get_state(LED_WIFI, &wifi_led);
printf("WiFi LED is %s\n", wifi_led == LED_ON ? "ON" : "OFF");

// Cleanup
status_led_deinit();
```

## Typical Status Patterns

| State | LED_DEVICE | LED_WIFI | LED_MQTT |
|-------|------------|----------|----------|
| Booting | Blink | OFF | OFF |
| WiFi Connecting | ON | Blink | OFF |
| WiFi Connected | ON | ON | OFF |
| MQTT Connected | ON | ON | ON |
| Error | Blink | OFF | OFF |

## Notes

- All LEDs start in OFF state after initialization
- Deinit automatically turns off all LEDs
- Thread-safe for multi-task access