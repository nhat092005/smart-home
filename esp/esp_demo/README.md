# ESP32 Smart Home System - Demo Version

## Overview

Demonstration build of ESP32 Smart Home system with MQTT over SSL/TLS. Includes embedded web interface for testing and development purposes. Built on ESP-IDF v5.x framework with production-ready security features.

## Key Differences from Production Version

- **Embedded Web Interface**: HTML/CSS/JS files compiled into firmware
- **Demo Credentials**: Pre-configured test credentials for quick setup
- **Extended Logging**: Verbose debug output for development
- **Test Mode Support**: Additional debugging endpoints

## Features

- WiFi station mode with captive portal fallback
- MQTT over SSL/TLS for secure communication (port 8883)
- Embedded web interface (HTML/CSS/JS in SPIFFS)
- Environmental monitoring: temperature, humidity, light intensity
- Device control: 3 relay outputs (fan, light, AC)
- Real-time clock with DS3231
- 128x64 OLED display interface
- 5 button inputs with debouncing
- 3-color status LED indicators
- NVS persistence for configuration
- FreeRTOS multi-task architecture

## Hardware Requirements

| Component | Model | Interface |
|-----------|-------|-----------|
| MCU | ESP32 | - |
| Temperature/Humidity | SHT3x | I2C |
| Light Sensor | BH1750 | I2C |
| RTC | DS3231 | I2C |
| Display | SH1106 128x64 OLED | I2C |
| Buttons | 5x Tactile | GPIO |
| Status LEDs | 3x LED | GPIO |
| Output Relays | 3x (Fan, Light, AC) | GPIO |

## Project Structure

```
esp/
    CMakeLists.txt          # Project build configuration
    sdkconfig               # ESP-IDF configuration
    partitions.csv          # Flash partition table
    main/                   # Application entry point
    components/
        application/        # Business logic layer
            mode_manager/       # Device mode management
            mqtt_callback/      # MQTT callback registry
            shared_sensor/      # Thread-safe sensor data
            task_button/        # Button processing
            task_display/       # OLED rendering
            task_init/          # System initialization
            task_manager/       # Global configuration
            task_mode/          # Display and sensor task
            task_mqtt/          # MQTT publishing
            task_status/        # LED status polling
            task_wifi/          # WiFi event handling
        communication/      # Network layer
            wifi_manager/       # WiFi STA/AP management
            mqtt_manager/       # MQTT client
            webserver/          # HTTP provisioning server
        hardware/           # Hardware abstraction
            button_handler/     # Button input handling
            device_control/     # Output device control
            status_led/         # LED indicators
        sensor/             # Sensor drivers
            i2cdev/             # I2C bus abstraction
            bh1750/             # Light sensor
            sht3x/              # Temperature/humidity
            ds3231/             # RTC
            sh1106/             # OLED display
            sensor_manager/     # Sensor initialization
            sensor_reader/      # Unified sensor reading
        utilities/          # Helper modules
            json_helper/        # JSON parsing/creation
```

## Demo-Specific Features

### Embedded Web Interface

- **Files**: index.html, style.css, script.js compiled into firmware
- **Storage**: SPIFFS partition (960KB)
- **Access**: http://192.168.4.1 (AP mode) or device IP (STA mode)
- **Features**: Live sensor data, device control, WiFi configuration

### Pre-configured Settings

```c
// Default demo credentials
#define DEMO_WIFI_SSID      "SmartHome_Demo"
#define DEMO_WIFI_PASSWORD  "demo1234"
#define DEMO_MQTT_BROKER    "mqtt://test.mosquitto.org:8883"
```

## Build and Flash

### Prerequisites

- ESP-IDF v5.x
- Python 3.8+

### Build Commands

```bash
# Set up ESP-IDF environment
. $IDF_PATH/export.sh

# Configure project
idf.py menuconfig

# Build project
idf.py build

# Flash to device
idf.py -p PORT flash

# Monitor output
idf.py -p PORT monitor

# Build, flash, and monitor
idf.py -p PORT flash monitor
```

## Configuration (menuconfig)

### Smart Home Device Configuration

| Category | Options |
|----------|---------|
| Application | Version, interval time |
| WiFi Manager | AP SSID, max retry, scan limit |
| MQTT Manager | Broker URI, port, credentials, topics |
| Button Handler | GPIO pins, debounce time |
| Device Control | GPIO pins for fan, light, AC |
| Status LED | GPIO pins, active levels |
| I2C Interface | SDA/SCL pins, frequency |

## Partition Table

| Name | Type | Size | Description |
|------|------|------|-------------|
| nvs | data | 24KB | Non-volatile storage |
| phy_init | data | 4KB | PHY calibration |
| factory | app | 2.9MB | Application firmware |
| storage | data | 960KB | SPIFFS storage |
| coredump | data | 64KB | Core dump storage |

## System Architecture

```
+------------------+     +------------------+     +------------------+
|   MQTT Broker    |<--->|   WiFi Manager   |<--->|   Web Server     |
+------------------+     +--------+---------+     +------------------+
                                  |
                         +--------v---------+
                         |   Task Manager   |
                         +--------+---------+
                                  |
        +------------+------------+------------+------------+
        |            |            |            |            |
   +----v----+  +----v----+  +----v----+  +----v----+  +----v----+
   |  Mode   |  | Button  |  | Display |  |  MQTT   |  | Status  |
   |  Task   |  |  Task   |  |  Task   |  |  Task   |  |  Task   |
   +---------+  +---------+  +---------+  +---------+  +---------+
        |            |            |            |            |
        +------------+-----+------+------------+------------+
                           |
                  +--------v--------+
                  |  Shared Sensor  |
                  +--------+--------+
                           |
        +------------+-----+------+------------+
        |            |            |            |
   +----v----+  +----v----+  +----v----+  +----v----+
   |  SHT3x  |  | BH1750  |  | DS3231  |  | SH1106  |
   +---------+  +---------+  +---------+  +---------+
```

## MQTT Topics

| Topic | Direction | Description |
|-------|-----------|-------------|
| SmartHome/{id}/data | Publish | Sensor readings |
| SmartHome/{id}/state | Publish | Device states |
| SmartHome/{id}/info | Publish | Device info |
| SmartHome/{id}/command | Subscribe | Control commands |

## GPIO Pin Assignment

Configured via menuconfig. Default pins:

| Function | GPIO |
|----------|------|
| I2C SDA | 21 |
| I2C SCL | 22 |
| Button MODE | Configurable |
| Button WIFI | Configurable |
| Button LIGHT | Configurable |
| Button FAN | Configurable |
| Button AC | Configurable |
| LED Device | Configurable |
| LED WiFi | Configurable |
| LED MQTT | Configurable |
| Relay Fan | Configurable |
| Relay Light | Configurable |
| Relay AC | Configurable |

## Version Comparison

| Feature | esp (Production) | esp_demo (This) | esp_no_tls | esp_no_tls_demo |
|---------|------------------|-----------------|------------|------------------|
| MQTT Security | SSL/TLS (8883) | SSL/TLS (8883) | TCP (1883) | TCP (1883) |
| Web Interface | Minimal | Embedded | Minimal | Embedded |
| Memory Usage | Standard | High | Low | Medium |
| Certificate Required | Yes | Yes | No | No |
| Debugging | Standard | Verbose | Standard | Verbose |
| Use Case | Production | Development | Local Network | Local Testing |

## License

MIT License

## Related Documentation

- [main/README.md](main/README.md) - Application entry point
- [components/application/README.md](components/application/README.md) - Business logic
- [components/communication/README.md](components/communication/README.md) - Network layer
- [components/hardware/README.md](components/hardware/README.md) - Hardware abstraction
- [components/sensor/README.md](components/sensor/README.md) - Sensor drivers
- [components/utilities/README.md](components/utilities/README.md) - Helper modules
| LED MQTT | Configurable |
| Relay Fan | Configurable |
| Relay Light | Configurable |
| Relay AC | Configurable |

## License

MIT License

## Related Documentation

- [main/README.md](main/README.md) - Application entry point
- [components/application/README.md](components/application/README.md) - Business logic
- [components/communication/README.md](components/communication/README.md) - Network layer
- [components/hardware/README.md](components/hardware/README.md) - Hardware abstraction
- [components/sensor/README.md](components/sensor/README.md) - Sensor drivers
- [components/utilities/README.md](components/utilities/README.md) - Helper modules
