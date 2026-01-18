# Documents

## Overview

Project documentation directory containing MQTT command specifications and protocol documentation.

## Files

- `MQTT_COMMANDS.md` - MQTT topic structure and command reference

## MQTT_COMMANDS.md

### Purpose

Comprehensive documentation of MQTT topics, message formats, and communication protocols used in the Smart Home system.

### Topics Covered

- Device data topics
- Control command topics
- Status update topics
- Message payloads
- QoS levels
- Retained messages

### Topic Structure

```
esp/<device_id>/data       # Sensor readings
esp/<device_id>/control    # Device commands
esp/<device_id>/status     # Connection status
```

### Data Format

All MQTT messages use JSON format for consistency and ease of parsing.

### Usage

Reference this document when:
- Implementing new MQTT clients
- Adding new device types
- Debugging communication issues
- Understanding message flow

## Related Documentation

- [../esp/README.md](../esp/README.md) - ESP32 firmware MQTT client
- [../web/README.md](../web/README.md) - Web dashboard MQTT client
- [../app/README.md](../app/README.md) - Mobile app MQTT client
- [../broker/README.md](../broker/README.md) - MQTT broker configuration
