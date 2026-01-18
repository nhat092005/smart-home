# Devices Module

## Overview

Device management module for rendering device cards, handling device detail modals, and managing device state in the mobile application.

## Files

- `device-manager.js` - Device data management
- `device-card.js` - Device UI rendering

## device-manager.js

### Purpose

Manages device data retrieval from Firebase, state updates, and real-time listeners.

### Functions

#### initializeDeviceManager()
```javascript
initializeDeviceManager()
```
Sets up Firebase listeners for device data changes.

**Call once on app initialization.**

**Example:**
```javascript
import { initializeDeviceManager } from './device-manager.js';

initializeDeviceManager();
```

#### getAllDevices()
```javascript
getAllDevices()
```
Retrieves all devices from Firebase.

**Returns:** Promise resolving to devices object

**Example:**
```javascript
const devices = await getAllDevices();
// { device001: { name: 'Living Room', temp: 25.5, ... }, ... }
```

#### getDevice()
```javascript
getDevice(deviceId)
```
Retrieves single device data.

**Parameters:**
- `deviceId` - Device identifier

**Returns:** Promise resolving to device data

#### updateDevice()
```javascript
updateDevice(deviceId, data)
```
Updates device data in Firebase.

**Parameters:**
- `deviceId` - Device identifier
- `data` - Updated device data object

**Example:**
```javascript
await updateDevice('device001', {
    name: 'Kitchen',
    temp: 26.0
});
```

#### isDeviceOnline()
```javascript
isDeviceOnline(deviceId)
```
Checks if device is currently online.

**Parameters:**
- `deviceId` - Device identifier

**Returns:** Boolean

**Logic:**
- Online if last update within 5 minutes
- Uses `lastUpdate` timestamp from Firebase

### Real-time Listeners

```javascript
function setupDeviceListeners() {
    const devicesRef = firebase.database().ref('devices');
    
    devicesRef.on('child_added', (snapshot) => {
        const deviceId = snapshot.key;
        const data = snapshot.val();
        renderDeviceCard(deviceId, data);
    });
    
    devicesRef.on('child_changed', (snapshot) => {
        const deviceId = snapshot.key;
        const data = snapshot.val();
        updateDeviceCard(deviceId, data);
    });
}
```

## device-card.js

### Purpose

Renders device cards on dashboard and handles card interactions.

### Functions

#### renderDeviceCard()
```javascript
renderDeviceCard(deviceId, data)
```
Creates and displays device card in grid.

**Parameters:**
- `deviceId` - Device identifier
- `data` - Device data object

**Example:**
```javascript
renderDeviceCard('device001', {
    name: 'Living Room',
    temp: 25.5,
    hum: 65.2,
    light: 450,
    fan: 1,
    lamp: 0
});
```

#### updateDeviceCard()
```javascript
updateDeviceCard(deviceId, data)
```
Updates existing device card with new data.

**Parameters:**
- `deviceId` - Device identifier
- `data` - Updated device data

#### openReportDetail()
```javascript
openReportDetail(deviceId)
```
Opens device detail modal with full information and controls.

**Parameters:**
- `deviceId` - Device identifier

**Example:**
```javascript
document.getElementById('device-card').addEventListener('click', () => {
    openReportDetail('device001');
});
```

#### closeReportDetail()
```javascript
closeReportDetail()
```
Closes device detail modal.

### Card HTML Structure

```html
<div class="device-card" data-device-id="device001" onclick="openReportDetail('device001')">
    <div class="device-header">
        <h3 class="device-name">Living Room</h3>
        <span class="badge badge-success">Online</span>
    </div>
    <div class="device-sensors">
        <div class="sensor-item">
            <i class="fas fa-thermometer-half sensor-icon"></i>
            <div class="sensor-value">25.5°C</div>
            <div class="sensor-label">Temperature</div>
        </div>
        <div class="sensor-item">
            <i class="fas fa-tint sensor-icon"></i>
            <div class="sensor-value">65.2%</div>
            <div class="sensor-label">Humidity</div>
        </div>
        <div class="sensor-item">
            <i class="fas fa-sun sensor-icon"></i>
            <div class="sensor-value">450 lux</div>
            <div class="sensor-label">Light</div>
        </div>
    </div>
</div>
```

## Device Detail Modal

### Modal Structure

```html
<div id="report-modal" class="modal" style="display: none;">
    <div class="modal-header">
        <h2 id="modal-device-name">Living Room</h2>
        <button onclick="closeReportDetail()">&times;</button>
    </div>
    
    <div class="report-stats">
        <!-- Sensor cards -->
    </div>
    
    <div class="report-controls">
        <!-- Toggle switches for fan, lamp, pump -->
    </div>
    
    <div class="report-chart">
        <!-- Chart.js chart -->
    </div>
</div>
```

### Toggle Controls

```javascript
function toggleFeature(deviceId, feature) {
    const currentState = getFeatureState(deviceId, feature);
    const newState = currentState === 1 ? 0 : 1;
    
    // Send MQTT command
    sendMQTTCommand(deviceId, feature, newState);
    
    // Update UI immediately
    updateToggleUI(feature, newState);
    
    // Firebase sync happens via MQTT handler
}
```

### Control Flow

```
User taps toggle → toggleFeature() → sendMQTTCommand()
    ↓
MQTT message → ESP device → Updates state
    ↓
ESP publishes new state → MQTT handler receives
    ↓
Update Firebase → Firebase listener → Update UI
```

## Device Data Structure

### Firebase Schema
```json
{
  "devices": {
    "device001": {
      "name": "Living Room",
      "temp": 25.5,
      "hum": 65.2,
      "light": 450,
      "fan": 1,
      "lamp": 0,
      "pump": 0,
      "lastUpdate": 1705561200000,
      "datetime": "2024-01-18 10:00:00"
    }
  }
}
```

## Sensor Value Formatting

```javascript
function formatSensorValue(value, type) {
    switch (type) {
        case 'temp':
            return `${value.toFixed(1)}°C`;
        case 'hum':
            return `${value.toFixed(1)}%`;
        case 'light':
            return `${Math.round(value)} lux`;
        default:
            return value;
    }
}
```

## Device Status

### Online Check
```javascript
function isDeviceOnline(lastUpdate) {
    const now = Date.now();
    const fiveMinutes = 5 * 60 * 1000;
    return (now - lastUpdate) < fiveMinutes;
}
```

### Status Badge
```javascript
function renderStatusBadge(isOnline) {
    const badge = document.createElement('span');
    badge.className = `badge ${isOnline ? 'badge-success' : 'badge-error'}`;
    badge.textContent = isOnline ? 'Online' : 'Offline';
    return badge;
}
```

## Event Handlers

### Card Click
```javascript
document.querySelectorAll('.device-card').forEach(card => {
    card.addEventListener('click', () => {
        const deviceId = card.dataset.deviceId;
        openReportDetail(deviceId);
    });
});
```

### Toggle Switch
```javascript
document.querySelectorAll('.control-toggle').forEach(toggle => {
    toggle.addEventListener('click', (e) => {
        e.stopPropagation();
        const deviceId = getCurrentDeviceId();
        const feature = toggle.dataset.feature;
        toggleFeature(deviceId, feature);
    });
});
```

## UI Updates

### Real-time Updates
```javascript
function updateDeviceCard(deviceId, data) {
    const card = document.querySelector(`[data-device-id="${deviceId}"]`);
    if (!card) return;
    
    card.querySelector('.sensor-temp').textContent = formatSensorValue(data.temp, 'temp');
    card.querySelector('.sensor-hum').textContent = formatSensorValue(data.hum, 'hum');
    card.querySelector('.sensor-light').textContent = formatSensorValue(data.light, 'light');
    
    const badge = card.querySelector('.badge');
    const isOnline = isDeviceOnline(data.lastUpdate);
    badge.className = `badge ${isOnline ? 'badge-success' : 'badge-error'}`;
    badge.textContent = isOnline ? 'Online' : 'Offline';
}
```

## Performance

- Card rendering: <50ms per device
- Real-time update: <100ms
- Modal open: <100ms
- Firebase queries cached
- Debounced UI updates

## Error Handling

### Firebase Errors
```javascript
try {
    const devices = await getAllDevices();
    renderDevices(devices);
} catch (error) {
    console.error('Failed to load devices:', error);
    showEmptyState('Unable to load devices');
}
```

### Missing Data
```javascript
function renderDeviceCard(deviceId, data) {
    if (!data || !data.name) {
        console.warn('Invalid device data:', deviceId);
        return;
    }
    // Render card
}
```

## Browser Support

- Android 7.0+ WebView
- Firebase Realtime Database
- ES6 JavaScript

## Dependencies

- Firebase Realtime Database
- MQTT client (for controls)
- Chart.js (for modal chart)

## Related Documentation

- [../mqtt/README.md](../mqtt/README.md) - MQTT communication
- [../charts/README.md](../charts/README.md) - Chart rendering
- [../../README.md](../../README.md) - JavaScript overview
