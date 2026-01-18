# MQTT Callback Registry

## Overview

Extensible command registration system for MQTT message handling. Provides dynamic callback registry mapping MQTT topics to handler functions.

## Features

- Dynamic callback registration
- Topic-based command dispatch
- Automatic topic matching
- Extensible command set
- Decoupled message handling

## API Functions

### Callback Registration

```c
esp_err_t mqtt_callback_register(const char *topic, mqtt_callback_func callback);
```

### Command Dispatch

```c
esp_err_t mqtt_callback_dispatch(const char *topic, const char *data, int data_len);
```

### Callback Management

```c
esp_err_t mqtt_callback_clear(void);
```

## Usage Example

```c
#include "mqtt_callback.h"
#include "esp_log.h"

static const char *TAG = "APP";

void handle_light_command(const char *topic, const char *data, int len) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.*s", len, data);
    ESP_LOGI(TAG, "Light command: %s", buffer);
    
    if (strcmp(buffer, "ON") == 0) {
        // Turn on light
    } else if (strcmp(buffer, "OFF") == 0) {
        // Turn off light
    }
}

void handle_fan_command(const char *topic, const char *data, int len) {
    ESP_LOGI(TAG, "Fan command received");
    // Handle fan control
}

void app_main(void) {
    // Register command handlers
    mqtt_callback_register("device/light", handle_light_command);
    mqtt_callback_register("device/fan", handle_fan_command);
    
    // Simulate message reception
    mqtt_callback_dispatch("device/light", "ON", 2);
    mqtt_callback_dispatch("device/fan", "SPEED_HIGH", 10);
}
```

## Callback Type

```c
typedef void (*mqtt_callback_func)(const char *topic, const char *data, int data_len);
```

## Registry Capacity

Default configuration:
- Maximum callbacks: 16 simultaneous registrations
- Topic string length: Dynamic allocation
- Thread-safe operations

## Integration Pattern

```c
// In task_init.c - Register all command handlers
mqtt_callback_register("device/mode", on_mode_command);
mqtt_callback_register("device/light", on_light_command);
mqtt_callback_register("device/fan", on_fan_command);
mqtt_callback_register("device/ac", on_ac_command);

// In task_mqtt.c - Dispatch incoming messages
void on_mqtt_message(const char *topic, const char *data, int len) {
    mqtt_callback_dispatch(topic, data, len);
}
```

## Dependencies

- ESP-IDF
- FreeRTOS
- Standard library (string.h) Manager

## Overview

Centralized callback management for MQTT events and commands. Acts as a registry and dispatcher - receives events from mqtt_manager, parses commands, and invokes registered callbacks. Contains no business logic.

## Features

- Event callbacks: connected, disconnected, data_publish, state_publish
- Command callbacks: set_device, set_devices, set_mode, set_interval, set_timestamp, get_status, reboot, factory_reset
- JSON command parsing with cmd_id tracking
- Separation of concerns: registry only, handlers implement logic

## File Structure

```
mqtt_callback/
    CMakeLists.txt
    mqtt_callback.c
    include/
        mqtt_callback.h
```

## API Reference

### Event Callback Types

```c
typedef void (*mqtt_event_connected_cb_t)(void);
typedef void (*mqtt_event_disconnected_cb_t)(void);
typedef void (*mqtt_event_data_publish_cb_t)(uint32_t timestamp, float *temp, float *hum, int *light);
typedef void (*mqtt_event_state_publish_cb_t)(uint32_t timestamp, int *mode, int *fan, int *light, int *ac);
```

### Command Callback Types

```c
typedef void (*mqtt_cmd_set_device_cb_t)(const char *cmd_id, const char *device, int state);
typedef void (*mqtt_cmd_set_devices_cb_t)(const char *cmd_id, int fan, int light, int ac);
typedef void (*mqtt_cmd_set_mode_cb_t)(const char *cmd_id, int mode);
typedef void (*mqtt_cmd_set_interval_cb_t)(const char *cmd_id, int interval);
typedef void (*mqtt_cmd_set_timestamp_cb_t)(const char *cmd_id, uint32_t timestamp);
typedef void (*mqtt_cmd_get_status_cb_t)(const char *cmd_id);
typedef void (*mqtt_cmd_reboot_cb_t)(const char *cmd_id);
typedef void (*mqtt_cmd_factory_reset_cb_t)(const char *cmd_id);
```

### Registration Functions

| Function | Description |
|----------|-------------|
| `mqtt_callback_init()` | Initialize and register with mqtt_manager |
| `mqtt_callback_register_on_connected(cb)` | Register connected callback |
| `mqtt_callback_register_on_disconnected(cb)` | Register disconnected callback |
| `mqtt_callback_register_on_data_publish(cb)` | Register data publish callback |
| `mqtt_callback_register_on_state_publish(cb)` | Register state publish callback |
| `mqtt_callback_register_on_set_device(cb)` | Register set_device command |
| `mqtt_callback_register_on_set_devices(cb)` | Register set_devices command |
| `mqtt_callback_register_on_set_mode(cb)` | Register set_mode command |
| `mqtt_callback_register_on_set_interval(cb)` | Register set_interval command |
| `mqtt_callback_register_on_set_timestamp(cb)` | Register set_timestamp command |
| `mqtt_callback_register_on_get_status(cb)` | Register get_status command |
| `mqtt_callback_register_on_reboot(cb)` | Register reboot command |
| `mqtt_callback_register_on_factory_reset(cb)` | Register factory_reset command |

### Invocation Functions

| Function | Description |
|----------|-------------|
| `mqtt_callback_invoke_connected()` | Invoke connected callback |
| `mqtt_callback_invoke_disconnected()` | Invoke disconnected callback |
| `mqtt_callback_invoke_data_publish(...)` | Invoke data publish callback |
| `mqtt_callback_invoke_state_publish(...)` | Invoke state publish callback |
| `mqtt_callback_invoke_set_device(...)` | Invoke set_device callback |
| `mqtt_callback_invoke_set_devices(...)` | Invoke set_devices callback |
| `mqtt_callback_invoke_set_mode(...)` | Invoke set_mode callback |
| `mqtt_callback_invoke_set_interval(...)` | Invoke set_interval callback |
| `mqtt_callback_invoke_set_timestamp(...)` | Invoke set_timestamp callback |
| `mqtt_callback_invoke_get_status(...)` | Invoke get_status callback |
| `mqtt_callback_invoke_reboot(...)` | Invoke reboot callback |
| `mqtt_callback_invoke_factory_reset(...)` | Invoke factory_reset callback |

## Supported Commands

| Command | Parameters | Description |
|---------|------------|-------------|
| `set_device` | device, state | Control single device |
| `set_devices` | fan, light, ac | Control multiple devices |
| `set_mode` | mode | Set device mode (0/1) |
| `set_interval` | interval | Set publish interval |
| `set_timestamp` | timestamp | Sync RTC time |
| `get_status` | - | Request status publish |
| `reboot` | - | Reboot device |
| `factory_reset` | - | Reset to factory defaults |

## Usage Example

```c
#include "mqtt_callback.h"

void on_set_device(const char *cmd_id, const char *device, int state)
{
    ESP_LOGI(TAG, "[%s] Set %s to %d", cmd_id, device, state);
    // Handle device control
}

void app_main(void)
{
    mqtt_callback_init();
    mqtt_callback_register_on_set_device(on_set_device);
}
```

## Dependencies

- `mqtt_manager` - MQTT client
- `json_helper` - JSON parsing
- `cJSON` - JSON library

