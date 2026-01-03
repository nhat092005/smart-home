# Hardware Component

GPIO hardware control drivers for Smart Home project.

## Structure

```
hardware/
  button_handler/  - Physical button input handling
  device_control/  - Output device control (Fan, Light, AC)
  status_led/      - Status LED indicators
```

## Components

| Component | Description |
|-----------|-------------|
| button_handler | 5 button inputs with debounce and callback |
| device_control | 3 output devices with ON/OFF/Toggle control |
| status_led | 3 status LEDs for Device/WiFi/MQTT indication |

## Hardware Summary

| Type | Count | GPIOs (Default) |
|------|-------|-----------------|
| Buttons | 5 | GPIO32, 33, 25, 26, 23 |
| Devices | 3 | GPIO16, 17, 19 |
| LEDs | 3 | GPIO27, 14, 13 |

## Dependencies

- ESP-IDF driver component
- FreeRTOS

## Usage

Add to your component CMakeLists.txt:

```cmake
REQUIRES hardware
```