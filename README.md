# SMART HOME IoT System

## Overview

Complete IoT smart home system with ESP32 firmware, web dashboard, mobile application, and custom PCB. Monitors environmental sensors and controls home appliances via MQTT and Firebase.

## Features

- Environmental monitoring (temperature, humidity, light)
- Remote device control (fan, light, AC via relays)
- Real-time data synchronization (MQTT + Firebase)
- Multiple interfaces (web dashboard, mobile app, OLED display)
- WiFi provisioning with captive portal

## Project Structure

```
SMART_HOME/
├── esp/            # ESP32 firmware (4 variants)
├── web/            # Web dashboard (TLS + non-TLS)
├── app/            # Mobile application (Capacitor)
├── pcb/            # PCB design (Altium)
├── broker/         # MQTT broker config
└── documents/      # Documentation
```

## Technology Stack

- Firmware: ESP-IDF v5.x, C, FreeRTOS
- Web: HTML5, CSS3, JavaScript ES6, Firebase, MQTT.js, Chart.js
- Mobile: Capacitor 8.0.0, Android SDK API 36
- Cloud: HiveMQ MQTT Broker, Firebase Auth + Realtime DB
- Hardware: ESP32, SHT3x, BH1750, DS3231, SH1106 OLED, Relays

## Quick Start

### ESP32 Firmware

```bash
cd esp/esp
idf.py build flash monitor
```

### Web Dashboard

```bash
cd web/web
python -m http.server 8000
```

### Mobile App

```bash
cd app
npm install
npx cap sync android
npx cap open android
```

## Documentation

### Main Components

- [ESP32 Firmware](esp/README.md) - Four firmware variants with modular architecture
- [Web Dashboard](web/README.md) - Two versions (TLS/non-TLS) with Firebase integration
- [Mobile Application](app/README.md) - Capacitor-based Android app
- [PCB Design](pcb/README.md) - Custom ESP32 carrier board
- [MQTT Broker](broker/README.md) - Mosquitto configuration
- [MQTT Commands](documents/MQTT_COMMANDS.md) - Complete command reference

### ESP32 Components

- [Application](esp/esp/components/application/README.md) - Business logic
- [Communication](esp/esp/components/communication/README.md) - WiFi, MQTT, WebServer
- [Hardware](esp/esp/components/hardware/README.md) - Buttons, LEDs, Relays
- [Sensor](esp/esp/components/sensor/README.md) - Sensor drivers
- [Utilities](esp/esp/components/utilities/README.md) - Helper modules

### Web/Mobile Assets

- [Web CSS](web/web/assets/css/README.md) - Stylesheet architecture
- [Web JavaScript](web/web/assets/js/README.md) - ES6 modules
- [Mobile CSS](app/www/assets/css/README.md) - Mobile-optimized styles
- [Mobile JavaScript](app/www/assets/js/README.md) - Mobile app modules

### Video Demo

- [YouTube Demo Video](https://youtu.be/4AQauzxO2xM) - Watch the system in action
- [YouTube Demo Backup Video](https://youtu.be/38vhAsI9Et0) - Watch the system in action

## Authors

Le Quang Minh Nhat, Thai Huu Loi, Tran Huu Dao

## License

MIT License - see [LICENSE.md](LICENSE.md)
