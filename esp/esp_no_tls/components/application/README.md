# Application Component

## Overview

Business logic layer orchestrating all system tasks for the ESP32 Smart Home system. Coordinates hardware drivers, communication layers, and implements the main application control flow.

## Modules

### Core Management

- **mode_manager** - Device operation mode (ON/OFF) with NVS persistence
- **mqtt_callback** - MQTT event and command callback registry
- **shared_sensor** - Thread-safe sensor data sharing between tasks
- **task_manager** - Global configuration and task coordination

### FreeRTOS Tasks

- **task_init** - System initialization orchestrator
- **task_button** - Button event queue and processing
- **task_display** - OLED display rendering loop
- **task_mode** - Sensor reading and display update coordination
- **task_mqtt** - MQTT publishing and command handling
- **task_status** - Status LED state polling
- **task_wifi** - WiFi event handling and MQTT triggering

## Task Architecture

```
       task_init (startup)
            |
    +-------+-------+
    |       |       |
 task_wifi task_button task_status
    |       |       |
    +-------+-------+
            |
      task_mqtt <--> mqtt_callback
            |               |
      shared_sensor    mode_manager
            |
      task_mode
            |
      task_display
```

## Data Flow

```
Sensors --> task_mode --> shared_sensor --> task_mqtt --> MQTT Broker
                |                                ^
                v                                |
            task_display                  mqtt_callback
```

## Quick Start

```c
// Main application entry
void app_main(void) {
    // Initialize all components
    task_init();
    
    // Tasks are now running
    // - Button events processed via queue
    // - MQTT publishes sensor data periodically
    // - Display updates every second
    // - Status LEDs show system state
}
```

## Configuration

Configurable via Kconfig:
- VERSION_APP: Application version (default: "1.0")
- INTERVAL_TIME_MS: Sensor/publish interval (default: 5000ms)

## Global State Variables

- **isModeON** (bool): Device mode state
- **isWiFi** (bool): WiFi connection state
- **isMQTT** (bool): MQTT connection state

## Dependencies

- hardware (button_handler, device_control, status_led)
- sensor (sensor_manager, sensor_reader)
- communication (wifi_manager, mqtt_manager)
- utilities (json_helper)

## Architecture

```
application/
    CMakeLists.txt
    README.md
    mode_manager/       # Device mode (ON/OFF) management with NVS persistence
    mqtt_callback/      # Centralized MQTT callback registry
    shared_sensor/      # Thread-safe sensor data sharing between tasks
    task_button/        # Button event processing task
    task_display/       # OLED display rendering task
    task_init/          # System initialization orchestrator
    task_manager/       # Global configuration and task header registry
    task_mode/          # Display update and sensor reading task
    task_mqtt/          # MQTT publishing and command handling task
    task_status/        # Status LED polling task
    task_wifi/          # WiFi event handling task
```

## Task Architecture

```
+------------------+
|    task_init     |  System startup - initializes all components
+--------+---------+
         |
         v
+------------------+     +------------------+     +------------------+
|   task_status    |     |   task_button    |     |    task_wifi     |
| LED polling 50ms |     | Button queue     |     | WiFi events      |
+------------------+     +------------------+     +------------------+
         |                       |                        |
         v                       v                        v
+------------------+     +------------------+     +------------------+
|   task_mode      |     |  mode_manager    |     |   task_mqtt      |
| Display+Sensors  |     | Mode persistence |     | MQTT pub/sub     |
+------------------+     +------------------+     +------------------+
         |                                                |
         +------------------+-----------------------------+
                            |
                    +-------v-------+
                    | shared_sensor |
                    | Thread-safe   |
                    +---------------+
```

## Data Flow

```
Sensors (SHT3x, BH1750) --> task_mode --> shared_sensor --> task_mqtt --> MQTT Broker
                              |                               ^
                              v                               |
                         task_display                  mqtt_callback
                              |                               |
                              v                               v
                         SH1106 OLED                   task_mqtt handlers
```

## Subcomponents

| Component | Description |
|-----------|-------------|
| mode_manager | Device mode (ON/OFF) with NVS persistence |
| mqtt_callback | MQTT event and command callback registry |
| shared_sensor | Thread-safe sensor data storage |
| task_button | Button event queue and processing |
| task_display | OLED display rendering with fonts |
| task_init | System initialization sequence |
| task_manager | Global config and header aggregation |
| task_mode | Display update loop and sensor reading |
| task_mqtt | MQTT publishing and command handling |
| task_status | Status LED state polling |
| task_wifi | WiFi event handling and MQTT trigger |

## Configuration (Kconfig)

```
VERSION_APP           # Application version string (default: "1.0")
INTERVAL_TIME_MS      # Sensor/publish interval (default: 5000ms)
```

## Global State Variables

| Variable | Type | Source | Description |
|----------|------|--------|-------------|
| `isModeON` | bool | mode_manager | Device mode state |
| `isWiFi` | bool | wifi_manager | WiFi connection state |
| `isMQTT` | bool | mqtt_manager | MQTT connection state |
| `g_app_version` | char[] | task_manager | App version string |
| `g_interval_time_ms` | uint32_t | task_manager | Publish interval |

## Dependencies

- **Hardware Components:** sensor, hardware
- **Communication Components:** communication (wifi_manager, mqtt_manager)
- **Utilities Components:** utilities (json_helper)
- **ESP-IDF:** FreeRTOS, NVS, esp_log

## Usage

```c
#include "task_manager.h"

void app_main(void)
{
    // Initialize all system components
    task_init();
    
    // Start operational tasks
    task_mode_init();      // Display and sensor reading
    task_mqtt_init();      // MQTT publishing
}
```

## Related Documentation

- [mode_manager README](mode_manager/README.md)
- [mqtt_callback README](mqtt_callback/README.md)
- [shared_sensor README](shared_sensor/README.md)
- [task_button README](task_button/README.md)
- [task_display README](task_display/README.md)
- [task_init README](task_init/README.md)
- [task_manager README](task_manager/README.md)
- [task_mode README](task_mode/README.md)
- [task_mqtt README](task_mqtt/README.md)
- [task_status README](task_status/README.md)
- [task_wifi README](task_wifi/README.md)
