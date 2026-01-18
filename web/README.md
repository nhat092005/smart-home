# Smart Home Web Dashboard Project

## Overview

Web-based monitoring and control interfaces for ESP32 Smart Home IoT system. Includes two variants: production build with SSL/TLS security and lightweight build for local networks without encryption.

## Project Variants

### 1. web (Production with TLS)
Secure web dashboard with MQTT over WebSocket SSL.

- **MQTT Protocol**: wss:// (WebSocket Secure)
- **MQTT Port**: 8081
- **Security**: SSL/TLS encrypted
- **Use Case**: Production deployment, internet-facing
- **Performance**: Standard

### 2. web_no_tls (Non-TLS)
Lightweight dashboard without MQTT encryption.

- **MQTT Protocol**: ws:// (WebSocket)
- **MQTT Port**: 8000
- **Security**: No encryption for MQTT
- **Use Case**: Local network deployment
- **Performance**: Lower latency

## Common Features

Both variants share:

- Real-time device control via MQTT
- Live sensor data visualization
- Interactive charts (Chart.js)
- Historical data export to Excel
- Device management (CRUD operations)
- Firebase Realtime Database integration
- Firebase Authentication
- Responsive single-page application
- Modular ES6 architecture

## Technology Stack

- HTML5, CSS3, JavaScript ES6
- Firebase (Realtime Database, Authentication)
- MQTT.js (WebSocket client)
- Chart.js (data visualization)
- SheetJS (Excel export)

## Version Comparison

| Feature | web | web_no_tls |
|---------|-----|------------|
| MQTT Protocol | wss:// (SSL) | ws:// (no SSL) |
| MQTT Port | 8081 | 8000 |
| Encryption | Yes | No |
| Security Level | High | Low |
| Certificate | Required | Not required |
| Use Case | Production | Local Network |
| Latency | Standard | Lower |
| Deployment | Internet | Local only |

## Quick Start

### Prerequisites

```bash
# Any modern web browser
# Python or Node.js for local server (optional)
```

### Run Locally

```bash
# Navigate to variant folder
cd web/web          # or web/web_no_tls

# Serve with Python
python -m http.server 8000

# Or with Node.js
npx http-server

# Open browser
open http://localhost:8000
```

### Deploy to Firebase

```bash
# Install Firebase CLI
npm install -g firebase-tools

# Login
firebase login

# Navigate to project folder
cd web/web_no_tls

# Deploy
firebase deploy
```

## Configuration

### Firebase Setup

1. Create project at https://console.firebase.google.com
2. Enable Realtime Database
3. Enable Email/Password authentication
4. Copy config to `firebase-config.js`

### MQTT Broker

**For web (TLS)**:
- Host: test.mosquitto.org
- Port: 8081
- Protocol: wss://

**For web_no_tls**:
- Host: broker.hivemq.com
- Port: 8000
- Protocol: ws://

## Project Structure

```
web/
├── web/                    # TLS variant
│   ├── index.html
│   ├── login.html
│   ├── README.md
│   └── assets/
│       ├── css/           # Stylesheets
│       └── js/            # JavaScript modules
│
├── web_no_tls/            # Non-TLS variant
│   ├── index.html
│   ├── login.html
│   ├── README.md
│   ├── firebase.json      # Firebase hosting config
│   └── assets/
│       ├── css/           # Stylesheets
│       └── js/            # JavaScript modules
│
└── README.md              # This file
```

## Module Organization

Both variants share identical structure:

```
assets/
├── css/
│   ├── base/              # Foundation styles
│   ├── layouts/           # Page structure
│   ├── components/        # Reusable UI
│   └── views/             # Page-specific
├── js/
│   ├── core/              # Firebase, auth
│   ├── mqtt/              # MQTT client, handlers
│   ├── devices/           # Device management
│   ├── charts/            # Visualization
│   ├── export/            # Data export
│   ├── settings/          # Configuration
│   ├── ui/                # UI utilities
│   └── utils/             # Helpers
└── main.js                # Entry point
```

## Features by Module

### Core
- Firebase configuration and initialization
- User authentication (login/register/logout)
- Protected route handling

### MQTT
- WebSocket client connection
- Topic subscription/publishing
- Message handling and parsing
- Ping service for connectivity
- Firebase data synchronization

### Devices
- Device CRUD operations
- Real-time status updates
- Relay control
- Sensor data display
- Online/offline indicators

### Charts
- Temperature visualization
- Humidity visualization
- Light level visualization
- Real-time chart updates
- Chart type switching

### Export
- Historical data retrieval
- Advanced filtering
- Date range selection
- Excel file generation
- Table view rendering

### Settings
- MQTT configuration
- Firebase configuration
- Device management
- Time synchronization
- Connection testing

## Firebase Database Structure

```json
{
  "devices": {
    "<deviceId>": {
      "name": "Living Room",
      "id": "device001",
      "online": true,
      "light": 450,
      "temp": 25.5,
      "hum": 65.2,
      "uptime": 86400,
      "ip": "192.168.1.100",
      "timestamp": "2024-01-18T10:30:00"
    }
  },
  "history": {
    "<deviceId>": {
      "<timestamp>": {
        "temp": 25.5,
        "hum": 65.2,
        "light": 450,
        "datetime": "2024-01-18 10:30:00"
      }
    }
  }
}
```

## MQTT Topics

### Subscribe (from ESP32)
```
SmartHome/<deviceId>/data      # Sensor data
SmartHome/<deviceId>/state     # Device state
SmartHome/<deviceId>/info      # Device information
```

### Publish (to ESP32)
```
device/mode                    # Mode control (ON/OFF)
device/light                   # Light relay control
device/fan                     # Fan relay control
device/ac                      # AC relay control
device/timestamp               # Time synchronization
device/reboot                  # Reboot command
```

## Security Considerations

### web (TLS)
- MQTT over WebSocket SSL (wss://)
- Firebase Authentication required
- HTTPS for all Firebase access
- Encrypted data in transit
- Suitable for internet deployment

### web_no_tls (Non-TLS)
- MQTT over plain WebSocket (ws://)
- Firebase Authentication required
- HTTPS for Firebase only
- No MQTT encryption
- Use only on trusted networks
- Not recommended for internet

## Browser Compatibility

- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

Requires ES6 module support.

## Development Workflow

### Recommended Approach
1. Start with web_no_tls for faster development
2. Test MQTT communication locally
3. Switch to web for production deployment
4. Add SSL certificate to broker

### Testing
1. Open browser developer tools
2. Check console for errors
3. Monitor network tab for MQTT/Firebase
4. Verify data flow in Firebase console

## Troubleshooting

### MQTT Connection Issues

**web (TLS)**:
- Verify broker supports WSS on port 8081
- Check SSL certificate validity
- Confirm firewall allows port 8081

**web_no_tls**:
- Verify broker supports WS on port 8000
- Check WebSocket support
- Confirm firewall allows port 8000

### Firebase Issues
- Verify config in firebase-config.js
- Check authentication is enabled
- Confirm database rules
- Test internet connectivity

### Chart Not Updating
- Check Firebase data structure
- Verify device is publishing
- Inspect console for errors
- Confirm Chart.js loaded

## Performance Metrics

| Metric | web (TLS) | web_no_tls |
|--------|-----------|------------|
| Initial Load | ~500ms | ~500ms |
| MQTT Latency | 50-200ms | 30-150ms |
| Firebase Sync | 100-300ms | 100-300ms |
| Memory Usage | ~50MB | ~50MB |
| Chart FPS | 60 | 60 |

## Documentation

Complete documentation in each variant:

- [web/README.md](web/README.md) - TLS variant details
- [web_no_tls/README.md](web_no_tls/README.md) - Non-TLS variant details
- [web/assets/README.md](web/assets/README.md) - Asset organization
- [web/assets/css/README.md](web/assets/css/README.md) - CSS architecture
- [web/assets/js/README.md](web/assets/js/README.md) - JavaScript modules

## Related Projects

- [../esp/](../esp/README.md) - ESP32 firmware variants
- [../broker/](../broker/README.md) - MQTT broker configuration
- [../documents/](../documents/README.md) - Additional documentation

## Contributing

1. Choose appropriate variant
2. Test changes in both variants if modifying shared structure
3. Update relevant README files
4. Follow existing code style

## License

MIT License
