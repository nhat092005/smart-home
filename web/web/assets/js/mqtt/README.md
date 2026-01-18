# MQTT Module

## Overview

MQTT communication module handling WebSocket connections, message publishing/subscribing, device state synchronization, and connectivity monitoring.

## Files

- `mqtt-client.js` - MQTT client initialization and connection management
- `mqtt-handlers.js` - Message parsing and handling logic
- `mqtt-to-firebase.js` - Firebase data synchronization
- `ping-service.js` - Connection monitoring and ping service

## Features

- MQTT WebSocket client connection
- Topic subscription and publishing
- Message parsing and validation
- Firebase data synchronization
- Automatic reconnection
- Ping service for connectivity checks
- Device state caching

## mqtt-client.js

### Purpose

Manages MQTT client initialization, connection, and message publishing.

### Functions

#### initializeMQTTClient()
```javascript
initializeMQTTClient(onConnect, onMessage, onConnectionLost)
```
Initializes MQTT client with callbacks.

#### subscribeToDevice()
```javascript
subscribeToDevice(deviceId)
```
Subscribes to device topics.

#### sendMQTTCommand()
```javascript
sendMQTTCommand(topic, message)
```
Publishes command to MQTT broker.

### Configuration

```javascript
const mqttConfig = {
    host: 'test.mosquitto.org',
    port: 8081,              // 8081 for wss://, 8000 for ws://
    path: '/mqtt',
    clientId: 'web_' + Date.now(),
    username: '',            // Optional
    password: '',            // Optional
    useSSL: true             // true for wss://, false for ws://
};
```

### Usage

```javascript
import { initializeMQTTClient, subscribeToDevice, sendMQTTCommand } from './mqtt-client.js';

// Initialize
initializeMQTTClient(
    () => console.log('Connected'),
    (topic, message) => console.log('Message:', message),
    () => console.log('Disconnected')
);

// Subscribe
subscribeToDevice('device001');

// Publish
sendMQTTCommand('device/light', 'ON');
```

## mqtt-handlers.js

### Purpose

Processes incoming MQTT messages and manages device state cache.

### Functions

#### handleMQTTMessage()
```javascript
handleMQTTMessage(topic, message)
```
Parses and processes MQTT messages. Handles:
- `SmartHome/<deviceId>/data` - Sensor readings
- `SmartHome/<deviceId>/state` - Device state
- `SmartHome/<deviceId>/info` - Device information

#### handleMQTTConnect()
```javascript
handleMQTTConnect()
```
Handles successful MQTT connection.

#### handleMQTTConnectionLost()
```javascript
handleMQTTConnectionLost()
```
Handles connection loss.

#### getMQTTCachedState()
```javascript
getMQTTCachedState(deviceId)
```
Retrieves cached device state.

### Message Formats

#### Sensor Data
```json
{
    "temp": 25.5,
    "hum": 65.2,
    "light": 450,
    "datetime": "2024-01-18 10:30:00"
}
```

#### Device State
```json
{
    "light": 1,
    "fan": 0,
    "ac": 1,
    "mode": "ON"
}
```

#### Device Info
```json
{
    "ip": "192.168.1.100",
    "uptime": 86400,
    "version": "1.0.0"
}
```

## mqtt-to-firebase.js

### Purpose

Synchronizes MQTT data to Firebase Realtime Database.

### Functions

#### syncToFirebase()
```javascript
syncToFirebase(deviceId, data, dataType)
```
Writes MQTT data to Firebase.

### Data Types

- `sensor` - Sensor readings → `/devices/<deviceId>/` and `/history/<deviceId>/`
- `state` - Device state → `/devices/<deviceId>/state`
- `info` - Device info → `/devices/<deviceId>/info`

### Usage

```javascript
import { syncToFirebase } from './mqtt-to-firebase.js';

// Sync sensor data
syncToFirebase('device001', {
    temp: 25.5,
    hum: 65.2,
    light: 450
}, 'sensor');

// Sync device state
syncToFirebase('device001', {
    light: 1,
    fan: 0
}, 'state');
```

## ping-service.js

### Purpose

Monitors MQTT connectivity by sending periodic ping messages.

### Functions

#### startPingService()
```javascript
startPingService()
```
Starts periodic ping (every 30 seconds).

#### stopPingService()
```javascript
stopPingService()
```
Stops ping service.

#### performInitialPing()
```javascript
performInitialPing()
```
Sends immediate ping to all devices.

### Ping Topic

```
device/ping
```

Devices respond to ping topic to confirm connectivity.

## MQTT Topics

### Subscribe Topics

| Topic | Description | Message Format |
|-------|-------------|----------------|
| `SmartHome/<deviceId>/data` | Sensor readings | JSON with temp, hum, light |
| `SmartHome/<deviceId>/state` | Device state | JSON with relay states |
| `SmartHome/<deviceId>/info` | Device information | JSON with IP, uptime |

### Publish Topics

| Topic | Description | Message Format |
|-------|-------------|----------------|
| `device/mode` | Mode control | "ON" or "OFF" |
| `device/light` | Light relay | "ON" or "OFF" |
| `device/fan` | Fan relay | "ON" or "OFF" |
| `device/ac` | AC relay | "ON" or "OFF" |
| `device/timestamp` | Time sync | Unix timestamp |
| `device/reboot` | Reboot command | "REBOOT" |
| `device/ping` | Connectivity check | "PING" |

## Connection Flow

```
Initialize Client → Connect to Broker → Subscribe to Topics → Start Ping Service
                                              ↓
                                        Message Received
                                              ↓
                                        Parse Message
                                              ↓
                                        Update Cache
                                              ↓
                                        Sync to Firebase
                                              ↓
                                        Update UI
```

## Error Handling

### Connection Errors
- Retry with exponential backoff
- Show connection status in UI
- Log errors to console

### Message Errors
- Validate JSON format
- Check required fields
- Ignore malformed messages

## Performance

- Message latency: 50-200ms (TLS) / 30-150ms (non-TLS)
- Ping interval: 30 seconds
- Reconnect delay: 2-10 seconds (exponential)
- Message cache: In-memory, cleared on refresh

## Security

### TLS Version (web)
- WebSocket Secure (wss://)
- SSL/TLS encryption
- Port 8081

### Non-TLS Version (web_no_tls)
- Plain WebSocket (ws://)
- No encryption
- Port 8000
- Local network only

## Dependencies

- MQTT.js library
- Firebase Realtime Database

## Configuration Example

```javascript
// TLS configuration (web)
const mqttConfig = {
    host: 'test.mosquitto.org',
    port: 8081,
    useSSL: true
};

// Non-TLS configuration (web_no_tls)
const mqttConfig = {
    host: 'broker.hivemq.com',
    port: 8000,
    useSSL: false
};
```

## Troubleshooting

### Connection Fails
- Check broker URL and port
- Verify WebSocket support
- Confirm firewall rules
- Test with public broker

### Messages Not Received
- Verify topic subscription
- Check message format
- Confirm device is publishing
- Inspect network tab

### Firebase Sync Issues
- Verify Firebase connection
- Check database rules
- Confirm authentication
- Inspect console errors

## Related Documentation

- [../core/README.md](../core/README.md) - Authentication
- [../devices/README.md](../devices/README.md) - Device management
