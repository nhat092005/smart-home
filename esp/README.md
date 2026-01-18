# ESP32 Smart Home Project

## Overview

Complete ESP32-based Smart Home system with multiple build configurations for different deployment scenarios. Includes production-ready firmware with SSL/TLS security, demonstration builds with embedded web interfaces, and lightweight variants for local network deployments.

## Project Variants

This repository contains four ESP32 firmware variants:

### 1. esp (Production)
Production-ready firmware with full security features.

- **MQTT Security**: SSL/TLS over port 8883
- **Web Interface**: Minimal provisioning pages
- **Memory Usage**: Standard
- **Certificate**: Required (CA certificate)
- **Use Case**: Production deployment, internet-facing

### 2. esp_demo (Demo with TLS)
Demonstration build with embedded web interface and full security.

- **MQTT Security**: SSL/TLS over port 8883
- **Web Interface**: Full HTML/CSS/JS embedded in firmware
- **Memory Usage**: High (web assets in SPIFFS)
- **Certificate**: Required (CA certificate)
- **Use Case**: Development, testing, demonstrations

### 3. esp_no_tls (Production without TLS)
Lightweight production firmware without encryption.

- **MQTT Security**: TCP only, port 1883
- **Web Interface**: Minimal provisioning pages
- **Memory Usage**: Low (no mbedTLS)
- **Certificate**: Not required
- **Use Case**: Local network deployment, private LAN

### 4. esp_no_tls_demo (Demo without TLS)
Demonstration build without encryption, embedded web interface.

- **MQTT Security**: TCP only, port 1883
- **Web Interface**: Full HTML/CSS/JS embedded in firmware
- **Memory Usage**: Medium
- **Certificate**: Not required
- **Use Case**: Local testing, development on isolated networks

## Version Comparison Table

| Feature | esp | esp_demo | esp_no_tls | esp_no_tls_demo |
|---------|-----|----------|------------|-----------------|
| **MQTT Protocol** | MQTT over SSL/TLS | MQTT over SSL/TLS | MQTT over TCP | MQTT over TCP |
| **MQTT Port** | 8883 | 8883 | 1883 | 1883 |
| **Web Interface** | Minimal | Embedded Full | Minimal | Embedded Full |
| **Flash Usage** | ~2.9MB | ~3.1MB | ~2.7MB | ~2.9MB |
| **Heap Available** | Standard | -40KB | +40KB | Standard |
| **Certificate Required** | Yes | Yes | No | No |
| **Debug Logging** | Standard | Verbose | Standard | Verbose |
| **Security Level** | High | High | Low | Low |
| **Best For** | Production | Development | Local Network | Local Testing |

## Common Features

All variants share the same core functionality:

- **WiFi Management**: Station mode with captive portal provisioning
- **Sensors**: SHT3x (temp/humidity), BH1750 (light), DS3231 (RTC)
- **Display**: SH1106 128x64 OLED
- **Device Control**: 3 relay outputs (fan, light, AC)
- **Inputs**: 5 button inputs with debouncing
- **Status**: 3-color LED indicators
- **Storage**: NVS for persistent configuration
- **Architecture**: FreeRTOS multi-task system

## Hardware Requirements

| Component | Model | Interface | Pins |
|-----------|-------|-----------|------|
| MCU | ESP32 | - | - |
| Temperature/Humidity | SHT3x | I2C | SDA=21, SCL=22 |
| Light Sensor | BH1750 | I2C | SDA=21, SCL=22 |
| RTC | DS3231 | I2C | SDA=21, SCL=22 |
| Display | SH1106 128x64 | I2C | SDA=21, SCL=22 |
| Buttons | 5x Tactile | GPIO | Configurable |
| Status LEDs | 3x RGB LED | GPIO | Configurable |
| Relays | 3x Relay Module | GPIO | Configurable |

## Quick Start

### Prerequisites

```bash
# Install ESP-IDF v5.x
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh
. ./export.sh
```

### Build Any Variant

```bash
# Navigate to desired variant
cd esp              # or esp_demo, esp_no_tls, esp_no_tls_demo

# Configure (optional)
idf.py menuconfig

# Build
idf.py build

# Flash
idf.py -p COM3 flash

# Monitor
idf.py -p COM3 monitor
```

## Configuration Options

All variants support menuconfig for customization:

```bash
idf.py menuconfig
```

### Smart Home Device Configuration
- Application version string
- Data publish interval (seconds)
- WiFi AP SSID and password
- MQTT broker URI and credentials
- Button GPIO pin assignments
- Device control GPIO pins
- Status LED GPIO pins
- I2C interface configuration

## Project Structure

Each variant has the same directory structure:

```
esp/                        # or esp_demo, esp_no_tls, esp_no_tls_demo
├── CMakeLists.txt         # Build configuration
├── sdkconfig              # ESP-IDF configuration
├── partitions.csv         # Partition table
├── README.md              # Variant-specific documentation
├── main/                  # Application entry point
│   ├── main.c
│   └── README.md
└── components/            # Shared component library
    ├── application/       # Business logic (12 modules)
    ├── communication/     # Network layer (4 modules)
    ├── hardware/          # Hardware abstraction (4 modules)
    ├── sensor/            # Sensor drivers (8 modules)
    └── utilities/         # Helper modules (2 modules)
```

## Component Documentation

Complete documentation available in each variant:

### Application Layer (12 modules)
- [application/README.md](esp/components/application/README.md) - Business logic overview
- [mode_manager](esp/components/application/mode_manager/README.md) - Device operation modes
- [mqtt_callback](esp/components/application/mqtt_callback/README.md) - Command registry
- [shared_sensor](esp/components/application/shared_sensor/README.md) - Thread-safe data sharing
- [task_manager](esp/components/application/task_manager/README.md) - Task creation
- [task_init](esp/components/application/task_init/README.md) - System initialization
- [task_button](esp/components/application/task_button/README.md) - Button handling
- [task_display](esp/components/application/task_display/README.md) - OLED updates
- [task_mode](esp/components/application/task_mode/README.md) - Mode coordination
- [task_mqtt](esp/components/application/task_mqtt/README.md) - MQTT communication
- [task_status](esp/components/application/task_status/README.md) - LED indicators
- [task_wifi](esp/components/application/task_wifi/README.md) - WiFi management

### Communication Layer (4 modules)
- [communication/README.md](esp/components/communication/README.md) - Network overview
- [wifi_manager](esp/components/communication/wifi_manager/README.md) - WiFi provisioning
- [mqtt_manager](esp/components/communication/mqtt_manager/README.md) - MQTT client
- [webserver](esp/components/communication/webserver/README.md) - HTTP server

### Hardware Layer (4 modules)
- [hardware/README.md](esp/components/hardware/README.md) - Hardware overview
- [button_handler](esp/components/hardware/button_handler/README.md) - Button input
- [device_control](esp/components/hardware/device_control/README.md) - Relay control
- [status_led](esp/components/hardware/status_led/README.md) - LED control

### Sensor Layer (8 modules)
- [sensor/README.md](esp/components/sensor/README.md) - Sensor overview
- [i2cdev](esp/components/sensor/i2cdev/README.md) - I2C abstraction
- [bh1750](esp/components/sensor/bh1750/README.md) - Light sensor
- [ds3231](esp/components/sensor/ds3231/README.md) - RTC
- [sht3x](esp/components/sensor/sht3x/README.md) - Temperature/humidity
- [sh1106](esp/components/sensor/sh1106/README.md) - OLED display
- [sensor_manager](esp/components/sensor/sensor_manager/README.md) - Sensor initialization
- [sensor_reader](esp/components/sensor/sensor_reader/README.md) - Unified reading

### Utilities (2 modules)
- [utilities/README.md](esp/components/utilities/README.md) - Utilities overview
- [json_helper](esp/components/utilities/json_helper/README.md) - JSON utilities

## Security Considerations

### Production Variants (esp, esp_demo)
- Use MQTT over SSL/TLS on port 8883
- Requires valid CA certificate
- Data encrypted in transit
- Suitable for internet connectivity
- Certificate validation enabled

### Non-TLS Variants (esp_no_tls, esp_no_tls_demo)
- Use MQTT over TCP on port 1883
- No encryption or certificate required
- Data transmitted in plaintext
- Use only on trusted local networks
- Not recommended for internet-facing deployments

## Memory Footprint

Typical memory usage per variant:

| Variant | Flash Used | Free Heap | Stack Usage |
|---------|------------|-----------|-------------|
| esp | ~2.9MB | ~180KB | Standard |
| esp_demo | ~3.1MB | ~140KB | +SPIFFS |
| esp_no_tls | ~2.7MB | ~220KB | -mbedTLS |
| esp_no_tls_demo | ~2.9MB | ~180KB | Balanced |

## MQTT Topics

All variants use the same MQTT topic structure:

| Topic | Direction | Description |
|-------|-----------|-------------|
| SmartHome/{id}/data | Publish | Sensor readings (JSON) |
| SmartHome/{id}/state | Publish | Device states |
| SmartHome/{id}/info | Publish | Device information |
| device/mode | Subscribe | Mode control (ON/OFF) |
| device/light | Subscribe | Light relay control |
| device/fan | Subscribe | Fan relay control |
| device/ac | Subscribe | AC relay control |

## Development Workflow

### Recommended Workflow
1. **Start with esp_demo**: Full features + debugging
2. **Test with esp_no_tls_demo**: Local network testing
3. **Optimize with esp_no_tls**: Production local deployment
4. **Deploy with esp**: Production internet deployment

### Debugging
- Use demo variants for verbose logging
- Use non-TLS variants for faster connection times
- Use TLS variants to test certificate handling

## Troubleshooting

### Common Issues

**WiFi Connection Fails**
- Check SSID/password in NVS or menuconfig
- Verify WiFi signal strength
- Use captive portal for provisioning

**MQTT Connection Fails (TLS variants)**
- Verify CA certificate is correct
- Check broker supports SSL/TLS on port 8883
- Confirm system time is accurate (for certificate validation)

**MQTT Connection Fails (non-TLS variants)**
- Verify broker supports TCP on port 1883
- Check firewall rules
- Confirm broker address is reachable

**Out of Memory**
- Reduce logging level in menuconfig
- Use non-TLS variant for lower memory usage
- Disable unused features in menuconfig

## License

MIT License

## Related Projects

- [web/](../web/README.md) - Web dashboard with TLS
- [web_no_tls/](../web_no_tls/README.md) - Web dashboard without TLS
- [broker/](../broker/README.md) - MQTT broker configuration
- [documents/](../documents/README.md) - Additional documentation

## Contributing

1. Choose appropriate variant for your changes
2. Test changes across all variants if modifying shared components
3. Update relevant README files
4. Follow existing code style and structure

## Support

For issues or questions:
- Check variant-specific README: [esp](esp/README.md), [esp_demo](esp_demo/README.md), [esp_no_tls](esp_no_tls/README.md), [esp_no_tls_demo](esp_no_tls_demo/README.md)
- Review component documentation in components/ folders
- Check ESP-IDF documentation for framework-specific issues
