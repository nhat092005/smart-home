# MQTT Module

## Overview

MQTT communication module for real-time messaging between mobile app and ESP32 devices via WebSocket protocol.

## Files

- `mqtt-client.js` - MQTT connection and publishing
- `mqtt-handlers.js` - Message handling and parsing

## mqtt-client.js

### Purpose

Manages MQTT WebSocket connection, publishing commands, and connection state.

### Configuration

```javascript
const MQTT_CONFIG = {
    host: 'broker.emqx.io',
    port: 8083,
    path: '/mqtt',
    clientId: 'mobile_app_' + Date.now(),
    username: '',
    password: '',
    keepAlive: 60,
    cleanSession: true,
    useSSL: false
};
```

### Functions

#### connectMQTT()
```javascript
connectMQTT()
```
Establishes MQTT WebSocket connection.

**Returns:** Promise resolving on successful connection

**Example:**
```javascript
import { connectMQTT } from './mqtt-client.js';

await connectMQTT();
console.log('MQTT connected');
```

#### sendMQTTCommand()
```javascript
sendMQTTCommand(deviceId, command, value)
```
Publishes control command to device topic.

**Parameters:**
- `deviceId` - Target device identifier
- `command` - Command name ('fan', 'lamp', 'pump', etc.)
- `value` - Command value (0 or 1 for toggles)

**Example:**
```javascript
import { sendMQTTCommand } from './mqtt-client.js';

// Turn on fan
await sendMQTTCommand('device001', 'fan', 1);

// Turn off lamp
await sendMQTTCommand('device001', 'lamp', 0);
```

#### disconnectMQTT()
```javascript
disconnectMQTT()
```
Closes MQTT connection gracefully.

#### getConnectionStatus()
```javascript
getConnectionStatus()
```
Returns current connection status.

**Returns:** Boolean (true if connected)

### Connection Events

```javascript
client.onConnectionLost = (response) => {
    console.error('MQTT connection lost:', response.errorMessage);
    // Auto-reconnect logic
    setTimeout(connectMQTT, 3000);
};

client.onConnect = () => {
    console.log('MQTT connected');
    subscribeToTopics();
};
```

## mqtt-handlers.js

### Purpose

Processes incoming MQTT messages and updates application state.

### Functions

#### onMessageArrived()
```javascript
onMessageArrived(message)
```
Main message handler for all incoming MQTT messages.

**Parameters:**
- `message` - Paho MQTT message object

**Internal function, called automatically by MQTT client.**

#### parseDeviceData()
```javascript
parseDeviceData(payload)
```
Parses sensor data from device messages.

**Parameters:**
- `payload` - JSON string from MQTT message

**Returns:** Parsed device data object

**Example payload:**
```json
{
  "temp": 25.5,
  "hum": 65.2,
  "light": 450,
  "fan": 1,
  "lamp": 0,
  "pump": 0,
  "datetime": "2024-01-18 10:00:00"
}
```

#### subscribeToTopics()
```javascript
subscribeToTopics()
```
Subscribes to all device topics after connection.

**Topics:**
```
esp/+/data        # All device sensor data
esp/+/status      # All device status updates
```

### Message Routing

```javascript
function onMessageArrived(message) {
    const topic = message.destinationName;
    const payload = message.payloadString;
    
    if (topic.endsWith('/data')) {
        handleDeviceData(topic, payload);
    } else if (topic.endsWith('/status')) {
        handleDeviceStatus(topic, payload);
    }
}
```

### Data Handling

```javascript
function handleDeviceData(topic, payload) {
    const deviceId = extractDeviceId(topic);
    const data = parseDeviceData(payload);
    
    // Update Firebase
    updateFirebase(deviceId, data);
    
    // Update UI
    updateDeviceCard(deviceId, data);
}
```

## MQTT Topics

### Device Data Topic
```
esp/<device_id>/data
```
**Payload:**
```json
{
  "temp": 25.5,
  "hum": 65.2,
  "light": 450,
  "fan": 1,
  "lamp": 0,
  "pump": 0,
  "datetime": "2024-01-18 10:00:00"
}
```

### Device Control Topic
```
esp/<device_id>/control
```
**Payload:**
```json
{
  "command": "fan",
  "value": 1
}
```

### Device Status Topic
```
esp/<device_id>/status
```
**Payload:**
```json
{
  "status": "online",
  "uptime": 3600,
  "ip": "192.168.1.100"
}
```

## Connection Management

### Auto-Reconnect
```javascript
let reconnectAttempts = 0;
const MAX_RECONNECT_ATTEMPTS = 5;

function autoReconnect() {
    if (reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
        reconnectAttempts++;
        setTimeout(connectMQTT, 3000 * reconnectAttempts);
    }
}
```

### Connection Status Indicator
```javascript
function updateConnectionStatus(connected) {
    const indicator = document.getElementById('mqtt-status');
    indicator.className = connected ? 'status-online' : 'status-offline';
    indicator.textContent = connected ? 'Connected' : 'Disconnected';
}
```

## Quality of Service (QoS)

```javascript
const QOS = {
    AT_MOST_ONCE: 0,    // Fire and forget
    AT_LEAST_ONCE: 1,   // Delivery guaranteed
    EXACTLY_ONCE: 2     // Delivery exactly once
};

// Use QoS 1 for commands
client.publish(topic, payload, QOS.AT_LEAST_ONCE, false);
```

## Error Handling

### Connection Errors
```javascript
try {
    await connectMQTT();
} catch (error) {
    console.error('Connection failed:', error);
    showNotification('MQTT connection failed', 'error');
}
```

### Message Errors
```javascript
function parseDeviceData(payload) {
    try {
        return JSON.parse(payload);
    } catch (error) {
        console.error('Invalid JSON:', payload);
        return null;
    }
}
```

## WebSocket Configuration

### Broker Settings
```javascript
const wsUrl = `ws://${host}:${port}${path}`;
// Production: wss://broker.example.com:8083/mqtt
```

### Keep-Alive
```javascript
client.keepAliveInterval = 60;  // Ping every 60 seconds
```

## Performance

- Connection time: <2s
- Message latency: <100ms
- Auto-reconnect: 3s intervals
- Max payload: 256KB (MQTT default)

## Security

### Authentication
```javascript
const connectOptions = {
    userName: 'mqtt_user',
    password: 'mqtt_password',
    useSSL: true  // Production
};
```

### Topic Restrictions
- Subscribe: Read-only device topics
- Publish: Control topics only

## Integration with Firebase

### Sync MQTT to Firebase
```javascript
function updateFirebase(deviceId, data) {
    const ref = firebase.database().ref(`devices/${deviceId}`);
    ref.update(data);
}
```

## Browser Support

- Android 7.0+ WebView
- WebSocket support
- Paho MQTT.js v1.0.1

## Dependencies

- Paho MQTT.js library
- Firebase Realtime Database

## Related Documentation

- [../core/README.md](../core/README.md) - Authentication
- [../devices/README.md](../devices/README.md) - Device management
- [../../README.md](../../README.md) - JavaScript overview
