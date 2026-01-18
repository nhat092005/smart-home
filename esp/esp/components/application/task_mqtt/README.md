# MQTT Communication Task

## Overview

FreeRTOS task for MQTT broker connection management and periodic sensor data publishing with SSL/TLS encryption.

## Features

- Automatic broker connection
- SSL/TLS secure communication
- Periodic data publishing
- Connection state monitoring
- Automatic reconnection
- Mode-aware publishing

## Task Function

```c
void mqtt_task(void *pvParameters);
```

## Task Workflow

1. **Wait for WiFi**: Monitor WiFi connection status
2. **Connect to Broker**: Establish MQTT over SSL/TLS
3. **Subscribe to Topics**: Register command topics
4. **Publish Loop**: Send sensor data at configured intervals
5. **Handle Disconnection**: Automatic reconnection attempts

## Usage Example

```c
#include "task_mqtt.h"
#include "task_manager.h"

void app_main(void) {
    // Initialize prerequisites
    wifi_manager_init();
    mqtt_manager_init();
    shared_sensor_init();
    mode_manager_init();
    
    // Create MQTT task (typically done by task_manager)
    xTaskCreate(mqtt_task, "mqtt", 5120, NULL, 3, NULL);
    
    // Task manages connection and publishing lifecycle
}
```

## Task Loop

```c
void mqtt_task(void *pvParameters) {
    sensor_data_t data;
    
    while (1) {
        // Wait for WiFi connection
        while (!wifi_manager_is_connected()) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        
        // Connect to MQTT broker
        if (!mqtt_manager_is_connected()) {
            mqtt_manager_connect();
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }
        
        // Publish data if mode is ON
        if (isModeON) {
            shared_sensor_get_data(&data);
            
            char payload[256];
            snprintf(payload, sizeof(payload),
                     "{\"temperature\":%.1f,\"humidity\":%.1f,\"light\":%d,\"datetime\":\"%s\"}",
                     data.temperature, data.humidity, data.light, data.datetime);
            
            mqtt_manager_publish("device/sensor", payload, 0, 0);
        }
        
        vTaskDelay(pdMS_TO_TICKS(interval_seconds * 1000));
    }
}
```

## Published Topics

| Topic | Payload Format | QoS | Retain |
|-------|----------------|-----|--------|
| device/sensor | JSON sensor data | 0 | No |
| device/status | JSON device status | 0 | Yes |

## Subscribed Topics

| Topic | Handler | Purpose |
|-------|---------|----------|
| device/mode | on_mode_command | Mode control |
| device/light | on_light_command | Light relay |
| device/fan | on_fan_command | Fan relay |
| device/ac | on_ac_command | AC relay |

## Payload Examples

### Sensor Data (Published)
```json
{
  "temperature": 25.5,
  "humidity": 65.2,
  "light": 450,
  "datetime": "2024-01-15 14:30:25"
}
```

### Device Status (Published)
```json
{
  "mode": "ON",
  "wifi_rssi": -45,
  "uptime": 86400,
  "free_heap": 120000
}
```

### Commands (Received)
```
Topic: device/mode
Payload: ON | OFF

Topic: device/light
Payload: ON | OFF
```

## Connection Management

```c
// Connection state machine
while (!mqtt_manager_is_connected()) {
    ESP_LOGI(TAG, "Connecting to MQTT broker...");
    esp_err_t ret = mqtt_manager_connect();
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Connection failed, retry in 5s");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
```

## Publishing Strategy

- **Interval**: Configurable via `interval_seconds` (default 5s)
- **Mode-Dependent**: Only publish when `isModeON == true`
- **WiFi-Dependent**: Wait for WiFi connection before MQTT
- **Error Handling**: Continue on publish failure

## Task Configuration

- Task name: "mqtt"
- Stack size: 5120 bytes (TLS overhead)
- Priority: 3 (high)
- Publish interval: 5-3600 seconds

## SSL/TLS Configuration

- Protocol: MQTT over SSL/TLS
- Port: 8883
- Certificate validation: CA certificate
- Cipher suites: Default ESP-IDF

## Dependencies

- mqtt_manager
- wifi_manager
- shared_sensor
- mode_manager
- mbedTLS
- FreeRTOS

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