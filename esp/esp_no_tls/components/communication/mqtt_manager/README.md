# MQTT Manager Module

## Overview

Secure MQTT client implementation for IoT messaging using SSL/TLS. Supports publish/subscribe operations with configurable topics, QoS levels, and command handling.

## Features

- MQTT over SSL/TLS (port 8883)
- Certificate bundle verification
- Hierarchical topic structure
- Configurable QoS and retain flags
- Command subscription with callback system
- Connection state callbacks
- Automatic reconnection
- Thread-safe operations
- Configurable via Kconfig

## Topic Structure

```
{base_topic}/{device_id}/{topic_type}
```

Example: `SmartHome/esp_01/data`

### Topic Types

- **data**: Sensor readings (QoS 0, no retain)
- **state**: Device state (QoS 1, retain)
- **info**: Device information (QoS 1, retain)
- **command**: Control commands (QoS 1, no retain)
- **response**: Command responses (QoS 1, retain)

## API Functions

### Initialization

```c
esp_err_t mqtt_manager_init(void);
esp_err_t mqtt_manager_start(void);
esp_err_t mqtt_manager_stop(void);
bool mqtt_manager_is_connected(void);
```

### Publishing

```c
esp_err_t mqtt_manager_publish_data(uint32_t timestamp, float temperature, 
                                    float humidity, int light);
esp_err_t mqtt_manager_publish_state(uint32_t timestamp, int mode, int interval,
                                     int fan, int light, int ac);
esp_err_t mqtt_manager_publish_info(uint32_t timestamp, const char *device_id,
                                    const char *ssid, const char *ip, const char *broker);
```

### Callbacks

```c
void mqtt_manager_register_command_callback(mqtt_command_callback_t callback);
void mqtt_manager_register_connected_callback(mqtt_event_callback_t callback);
void mqtt_manager_register_disconnected_callback(mqtt_event_callback_t callback);
```

## Usage Example

```c
#include "mqtt_manager.h"

void on_mqtt_command(const char *cmd_id, const char *command, cJSON *params) {
    printf("Received command: %s (ID: %s)\n", command, cmd_id);
    
    if (strcmp(command, "set_device") == 0) {
        const char *device = json_helper_get_string(params, "device", "");
        int state = json_helper_get_int(params, "state", 0);
        // Control device
    }
}

void on_mqtt_connected(void) {
    printf("MQTT connected\n");
    // Publish initial state
}

// Initialize
mqtt_manager_init();
mqtt_manager_register_command_callback(on_mqtt_command);
mqtt_manager_register_connected_callback(on_mqtt_connected);

// Start connection
mqtt_manager_start();

// Wait for connection
while (!mqtt_manager_is_connected()) {
    vTaskDelay(pdMS_TO_TICKS(100));
}

// Publish sensor data
mqtt_manager_publish_data(time(NULL), 25.5, 60.2, 450);

// Publish device state
mqtt_manager_publish_state(time(NULL), 1, 5, 1, 0, 1);
```

## Command Format

Commands received on the command topic:

```json
{
    "cmd_id": "a1b2c3",
    "command": "set_device",
    "params": {
        "device": "fan",
        "state": 1
    }
}
```

## Configuration

Configurable via Kconfig:
- MQTT_BASE_TOPIC: Base topic prefix (default: SmartHome)
- MQTT_DEVICE_ID: Device identifier (default: esp_01)
- MQTT_BROKER_URI: Broker hostname
- MQTT_BROKER_PORT: Broker port (default: 8883)
- MQTT_USERNAME: Authentication username
- MQTT_PASSWORD: Authentication password
- MQTT_KEEP_ALIVE_SEC: Keep alive interval (default: 120)

## Dependencies

- ESP-MQTT library
- ESP-TLS
- ESP certificate bundle
- utilities/json_helper

## Features

- MQTT over SSL/TLS (port 8883)
- ESP-TLS certificate bundle verification
- Configurable topic structure with base topic and device ID
- Four topic types: data, state, info, command
- QoS and retain configuration per topic type
- Command subscription with callback system
- Connection event callbacks
- Automatic reconnection
- Configurable via Kconfig

## File Structure

```
mqtt_manager/
    CMakeLists.txt
    Kconfig
    mqtt_manager.c          # MQTT client implementation
    include/
        mqtt_manager.h      # Public API
        mqtt_config.h       # Configuration defines
```

## Dependencies

- `mqtt` - ESP-MQTT client library
- `esp-tls` - TLS/SSL support
- `esp_crt_bundle` - Certificate bundle for SSL verification
- `utilities/json_helper` - JSON parsing for commands

## API Reference

### Types

```c
// Event callback function type
typedef void (*mqtt_event_callback_t)(void);

// Command callback function type  
typedef void (*mqtt_command_callback_t)(const char *device, const char *action);
```

### Initialization Functions

| Function | Description |
|----------|-------------|
| `mqtt_manager_init(void)` | Initialize MQTT client with SSL configuration |

### Connection Functions

| Function | Description |
|----------|-------------|
| `mqtt_manager_start(void)` | Start MQTT client and connect to broker |
| `mqtt_manager_stop(void)` | Stop MQTT client and disconnect |
| `mqtt_manager_is_connected(void)` | Check if connected to broker |

### Publish Functions

| Function | QoS | Retain | Description |
|----------|-----|--------|-------------|
| `mqtt_manager_publish_data(json)` | 0 | No | Publish sensor data |
| `mqtt_manager_publish_state(json)` | 1 | Yes | Publish device state |
| `mqtt_manager_publish_info(json)` | 1 | Yes | Publish device info |

### Callback Registration

| Function | Description |
|----------|-------------|
| `mqtt_manager_register_command_callback(cb)` | Register command handler |
| `mqtt_manager_register_connected_callback(cb)` | Register connection handler |
| `mqtt_manager_register_disconnected_callback(cb)` | Register disconnection handler |

## Configuration (Kconfig)

```
MQTT_BASE_TOPIC       # Base topic prefix (default: "SmartHome")
MQTT_DEVICE_ID        # Device identifier (default: "esp_01")
MQTT_BROKER_URI       # Broker hostname (default: HiveMQ cloud)
MQTT_BROKER_PORT      # Broker port (default: 8883)
MQTT_USERNAME         # Authentication username
MQTT_PASSWORD         # Authentication password
MQTT_KEEP_ALIVE_SEC   # Keep alive interval (default: 120)
```

## Topic Structure

The MQTT Manager uses a hierarchical topic structure:

```
{base_topic}/{device_id}/{topic_type}
```

### Topic Types

| Topic Type | Full Path | QoS | Retain | Direction | Purpose |
|------------|-----------|-----|--------|-----------|---------|
| data | SmartHome/esp_01/data | 0 | No | Publish | Sensor readings |
| state | SmartHome/esp_01/state | 1 | Yes | Publish | Device states |
| info | SmartHome/esp_01/info | 1 | Yes | Publish | Device information |
| command | SmartHome/esp_01/command | 1 | No | Subscribe | Control commands |

### Example Topics (default configuration)

```
SmartHome/esp_01/data      # Sensor data (temperature, humidity, etc.)
SmartHome/esp_01/state     # Device states (light: ON, fan: OFF)
SmartHome/esp_01/info      # Device info (IP, firmware version)
SmartHome/esp_01/command   # Commands from server/app
```

## Configuration Defines (mqtt_config.h)

```c
// Broker Configuration
#define MQTT_BROKER_URI       CONFIG_MQTT_BROKER_URI
#define MQTT_BROKER_PORT      CONFIG_MQTT_BROKER_PORT
#define MQTT_USERNAME         CONFIG_MQTT_USERNAME
#define MQTT_PASSWORD         CONFIG_MQTT_PASSWORD
#define MQTT_KEEP_ALIVE       CONFIG_MQTT_KEEP_ALIVE_SEC

// Topic Configuration
#define MQTT_BASE_TOPIC       CONFIG_MQTT_BASE_TOPIC
#define MQTT_DEVICE_ID        CONFIG_MQTT_DEVICE_ID

// Topic Format Strings
#define MQTT_TOPIC_DATA       "%s/%s/data"
#define MQTT_TOPIC_STATE      "%s/%s/state"
#define MQTT_TOPIC_INFO       "%s/%s/info"
#define MQTT_TOPIC_COMMAND    "%s/%s/command"
```

## Usage Examples

### Basic Initialization

```c
#include "mqtt_manager.h"

static void on_connected(void)
{
    ESP_LOGI(TAG, "MQTT connected");
    // Publish initial state
    mqtt_manager_publish_info("{\"status\":\"online\"}");
}

static void on_disconnected(void)
{
    ESP_LOGI(TAG, "MQTT disconnected");
}

static void on_command(const char *device, const char *action)
{
    ESP_LOGI(TAG, "Command: %s -> %s", device, action);
    
    if (strcmp(device, "light") == 0)
    {
        if (strcmp(action, "ON") == 0)
        {
            device_control_set_state(DEVICE_LIGHT, true);
        }
        else if (strcmp(action, "OFF") == 0)
        {
            device_control_set_state(DEVICE_LIGHT, false);
        }
    }
}

void app_main(void)
{
    mqtt_manager_init();
    
    mqtt_manager_register_connected_callback(on_connected);
    mqtt_manager_register_disconnected_callback(on_disconnected);
    mqtt_manager_register_command_callback(on_command);
    
    // Start after WiFi connected
    mqtt_manager_start();
}
```

### Publishing Sensor Data

```c
// QoS 0, no retain - for frequent sensor updates
char *json = json_helper_create_sensor_data(25.5, 60.0, 500);
mqtt_manager_publish_data(json);
free(json);
```

### Publishing Device State

```c
// QoS 1, retained - for device state persistence
char *json = json_helper_create_device_state(true, false, true);
mqtt_manager_publish_state(json);
free(json);
```

### Publishing Device Info

```c
// QoS 1, retained - for device information
char *json = json_helper_create_device_info("192.168.1.100", "1.0.0");
mqtt_manager_publish_info(json);
free(json);
```

## Command Message Format

The command topic expects JSON messages with the following structure:

```json
{
    "device": "light",
    "action": "ON"
}
```

### Supported Commands

| Device | Actions |
|--------|---------|
| light | ON, OFF, TOGGLE |
| fan | ON, OFF, TOGGLE |
| ac | ON, OFF, TOGGLE |

## SSL/TLS Configuration

The MQTT Manager uses ESP-TLS with certificate bundle for SSL verification:

```c
esp_mqtt_client_config_t config = {
    .broker = {
        .address.uri = "mqtts://broker.hivemq.com:8883",
        .verification.crt_bundle_attach = esp_crt_bundle_attach,
    },
    // ...
};
```

**Security Features:**
- TLS 1.2/1.3 encryption
- Server certificate verification via ESP certificate bundle
- Username/password authentication
- Secure port 8883

## Event Handling

The MQTT Manager handles the following ESP-MQTT events internally:

| Event | Internal Action |
|-------|-----------------|
| MQTT_EVENT_CONNECTED | Subscribe to command topic, call connected callback |
| MQTT_EVENT_DISCONNECTED | Call disconnected callback |
| MQTT_EVENT_DATA | Parse JSON, extract device/action, call command callback |
| MQTT_EVENT_ERROR | Log error details |

## Error Handling

All functions return `esp_err_t`:

| Return Value | Description |
|--------------|-------------|
| `ESP_OK` | Operation successful |
| `ESP_FAIL` | General failure |
| `ESP_ERR_INVALID_ARG` | Invalid parameter |
| `ESP_ERR_INVALID_STATE` | Client not initialized |
| `ESP_ERR_NO_MEM` | Memory allocation failed |

## Thread Safety

- MQTT client is internally thread-safe
- Callbacks execute in MQTT task context
- Publish functions can be called from any task
- Use mutexes if accessing shared resources in callbacks

## Performance Considerations

- **QoS 0**: Fire-and-forget, best for frequent sensor data
- **QoS 1**: At least once delivery, best for state changes
- **Retain**: Messages persist on broker, ideal for state/info
- **Keep Alive**: 120 seconds default, balance between responsiveness and power

## Notes

- Initialize WiFi and ensure connection before calling `mqtt_manager_start()`
- The client automatically reconnects on disconnection
- Command callback receives parsed JSON fields (device, action)
- All topic strings are dynamically formatted using base_topic and device_id
- Certificate bundle includes common CA certificates for SSL verification
