# MQTT Callback Manager

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

