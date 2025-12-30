# Task MQTT

## Overview

MQTT publishing and command handling task. Implements business logic for MQTT communication including periodic data publishing, device state management, and command execution.

## Features

- Periodic sensor data publishing
- Device state publishing with retain
- Device info publishing on connect
- Command handling for device control
- Thread-safe state management with mutex
- Extensible device registry

## File Structure

```
task_mqtt/
    CMakeLists.txt
    task_mqtt.c
    include/
        task_mqtt.h
```

## API Reference

### Event Callbacks

| Function | Description |
|----------|-------------|
| `task_mqtt_on_connected()` | Handle MQTT connect event |
| `task_mqtt_on_disconnected()` | Handle MQTT disconnect event |
| `task_mqtt_on_data_publish(...)` | Provide sensor data for publishing |
| `task_mqtt_on_state_publish(...)` | Provide state data for publishing |

### Command Callbacks

| Function | Description |
|----------|-------------|
| `task_mqtt_on_set_device(cmd_id, device, state)` | Control single device |
| `task_mqtt_on_set_devices(cmd_id, fan, light, ac)` | Control multiple devices |
| `task_mqtt_on_set_mode(cmd_id, mode)` | Set device mode |
| `task_mqtt_on_set_interval(cmd_id, interval)` | Set publish interval |
| `task_mqtt_on_set_timestamp(cmd_id, timestamp)` | Sync RTC time |
| `task_mqtt_on_get_status(cmd_id)` | Publish current status |
| `task_mqtt_on_reboot(cmd_id)` | Reboot device |
| `task_mqtt_on_factory_reset(cmd_id)` | Factory reset |

### Public Functions

| Function | Return | Description |
|----------|--------|-------------|
| `task_mqtt_init()` | `esp_err_t` | Initialize task and register callbacks |
| `task_mqtt_publish_current_state()` | `void` | Publish current device state |

## Device Registry

```c
static device_registry_entry_t device_registry[] = {
    {"fan", &device_state.fan},
    {"light", &device_state.light},
    {"ac", &device_state.ac},
    {NULL, NULL}  // End marker
};
```

## State Structure

```c
typedef struct {
    int mode;           // Current mode (0=OFF, 1=ON)
    int interval_sec;   // Data publish interval
    int fan;            // Fan state (0=OFF, 1=ON)
    int light;          // Light state (0=OFF, 1=ON)
    int ac;             // AC state (0=OFF, 1=ON)
} system_state_t;
```

## Publishing Topics

| Topic | Content | Trigger |
|-------|---------|--------|
| /data | Sensor readings | Periodic interval |
| /state | Device states | State change, periodic backup |
| /info | Device info | Connect, network change |

## Usage Example

```c
#include "task_mqtt.h"

void app_main(void)
{
    // After WiFi connected
    task_mqtt_init();
    
    // Manual state publish (e.g., after button press)
    task_mqtt_publish_current_state();
}
```

## Dependencies

- `mqtt_manager` - MQTT client
- `mqtt_callback` - Callback registration
- `json_helper` - JSON creation
- `shared_sensor` - Sensor data
- `device_control` - Hardware control
- `mode_manager` - Mode control
- `sensor_manager` - RTC timestamp