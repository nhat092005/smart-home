# Settings Module

## Overview

Settings management module for device configuration, time synchronization, and WiFi setup guide in the mobile application.

## Files

- `settings-manager.js` - Settings UI and actions

## settings-manager.js

### Purpose

Manages settings page functionality including system clock, device actions, and WiFi configuration guide.

### Functions

#### initializeSettings()
```javascript
initializeSettings()
```
Initializes settings page with device data and clock.

**Call on settings page load.**

**Example:**
```javascript
import { initializeSettings } from './settings-manager.js';

document.addEventListener('DOMContentLoaded', () => {
    initializeSettings();
});
```

#### updateSystemClock()
```javascript
updateSystemClock()
```
Updates clock display every second.

**Runs automatically after initialization.**

#### syncNTPTime()
```javascript
syncNTPTime(deviceId)
```
Sends NTP sync command to device.

**Parameters:**
- `deviceId` - Target device identifier

**Example:**
```javascript
document.getElementById('btn-sync-ntp').addEventListener('click', () => {
    syncNTPTime(currentDeviceId);
    showNotification('NTP sync requested', 'info');
});
```

#### setManualTime()
```javascript
setManualTime(deviceId, datetime)
```
Sets device time manually.

**Parameters:**
- `deviceId` - Target device identifier
- `datetime` - ISO datetime string

**Example:**
```javascript
const datetime = document.getElementById('manual-datetime').value;
setManualTime('device001', datetime);
```

#### rebootDevice()
```javascript
rebootDevice(deviceId)
```
Sends reboot command to device.

**Parameters:**
- `deviceId` - Target device identifier

**Example:**
```javascript
document.getElementById('btn-reboot').addEventListener('click', async () => {
    const confirmed = await showConfirmDialog(
        'Reboot Device',
        'Device will restart. Continue?'
    );
    
    if (confirmed) {
        rebootDevice(currentDeviceId);
        showNotification('Device rebooting...', 'info');
    }
});
```

#### showWiFiGuide()
```javascript
showWiFiGuide()
```
Displays WiFi setup guide modal.

## System Clock

### Clock Display
```html
<div class="system-clock">
    <div id="clock-time" class="clock-time">10:30:45</div>
    <div id="clock-date" class="clock-date">Thursday, January 18, 2024</div>
</div>
```

### Update Implementation
```javascript
function updateSystemClock() {
    const now = new Date();
    
    const timeStr = now.toLocaleTimeString('en-US', {
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: false
    });
    
    const dateStr = now.toLocaleDateString('en-US', {
        weekday: 'long',
        year: 'numeric',
        month: 'long',
        day: 'numeric'
    });
    
    document.getElementById('clock-time').textContent = timeStr;
    document.getElementById('clock-date').textContent = dateStr;
}

setInterval(updateSystemClock, 1000);
```

## Device Information

### Info Table
```html
<table class="settings-table">
    <tr>
        <th>Property</th>
        <th>Value</th>
    </tr>
    <tr>
        <td>Device Name</td>
        <td id="device-name">Living Room</td>
    </tr>
    <tr>
        <td>Device ID</td>
        <td id="device-id">device001</td>
    </tr>
    <tr>
        <td>IP Address</td>
        <td id="device-ip">192.168.1.100</td>
    </tr>
    <tr>
        <td>Uptime</td>
        <td id="device-uptime">3h 25m</td>
    </tr>
    <tr>
        <td>Last Update</td>
        <td id="last-update">Just now</td>
    </tr>
</table>
```

### Data Population
```javascript
function populateDeviceInfo(deviceId) {
    firebase.database().ref(`devices/${deviceId}`).once('value', (snapshot) => {
        const data = snapshot.val();
        
        document.getElementById('device-name').textContent = data.name;
        document.getElementById('device-id').textContent = deviceId;
        document.getElementById('device-ip').textContent = data.ip || 'N/A';
        document.getElementById('device-uptime').textContent = formatUptime(data.uptime);
        document.getElementById('last-update').textContent = getRelativeTime(data.lastUpdate);
    });
}
```

## Device Actions

### Action Buttons
```html
<div class="settings-actions">
    <button class="action-btn" onclick="syncNTPTime()">
        <span>Sync NTP Time</span>
        <i class="fas fa-sync"></i>
    </button>
    
    <button class="action-btn" onclick="showManualTimeModal()">
        <span>Set Manual Time</span>
        <i class="fas fa-clock"></i>
    </button>
    
    <button class="action-btn" onclick="showWiFiGuide()">
        <span>WiFi Setup Guide</span>
        <i class="fas fa-wifi"></i>
    </button>
    
    <button class="action-btn danger" onclick="rebootDevice()">
        <span>Reboot Device</span>
        <i class="fas fa-power-off"></i>
    </button>
</div>
```

## Time Synchronization

### NTP Sync
```javascript
function syncNTPTime(deviceId) {
    const command = {
        action: 'sync_ntp'
    };
    
    sendMQTTCommand(deviceId, 'system', command);
}
```

### Manual Time Setting
```javascript
function setManualTime(deviceId, datetime) {
    const timestamp = new Date(datetime).getTime();
    
    const command = {
        action: 'set_time',
        timestamp: timestamp
    };
    
    sendMQTTCommand(deviceId, 'system', command);
}
```

### Time Preview
```html
<div class="time-preview">
    <label>Select Date and Time:</label>
    <input type="datetime-local" id="manual-datetime">
    
    <div class="preview">
        <span>Preview:</span>
        <span id="time-preview">2024-01-18 10:30:00</span>
    </div>
    
    <button onclick="applyManualTime()">Apply</button>
</div>
```

## WiFi Setup Guide

### Guide Modal
```html
<div id="wifi-guide" class="modal">
    <div class="modal-header">
        <h2>WiFi Setup Guide</h2>
        <button onclick="hideModal('wifi-guide')">&times;</button>
    </div>
    
    <div class="wifi-guide">
        <div class="step">
            <span class="step-number">1</span>
            <p class="step-text">
                Turn on the device and wait for the blue LED to blink rapidly.
            </p>
        </div>
        
        <div class="step">
            <span class="step-number">2</span>
            <p class="step-text">
                Connect your phone to the WiFi network:
            </p>
            <div class="credential">
                SSID: ESP_SmartHome_AP
            </div>
            <div class="credential">
                Password: 12345678
            </div>
        </div>
        
        <div class="step">
            <span class="step-number">3</span>
            <p class="step-text">
                Open browser and go to:
            </p>
            <div class="credential">
                http://192.168.4.1
            </div>
        </div>
        
        <div class="step">
            <span class="step-number">4</span>
            <p class="step-text">
                Enter your WiFi credentials and save.
            </p>
        </div>
        
        <div class="step">
            <span class="step-number">5</span>
            <p class="step-text">
                Device will restart and connect to your network.
            </p>
        </div>
    </div>
</div>
```

## Device Reboot

### Confirmation Dialog
```javascript
async function rebootDevice(deviceId) {
    const confirmed = await showConfirmDialog(
        'Reboot Device',
        'The device will restart and be offline for 30 seconds. Continue?'
    );
    
    if (!confirmed) return;
    
    const command = {
        action: 'reboot'
    };
    
    sendMQTTCommand(deviceId, 'system', command);
    
    showNotification('Device rebooting. Please wait...', 'info', 5000);
}
```

## MQTT Commands

### System Commands Topic
```
esp/<device_id>/system
```

### Command Payloads

#### NTP Sync
```json
{
  "action": "sync_ntp"
}
```

#### Set Manual Time
```json
{
  "action": "set_time",
  "timestamp": 1705561200000
}
```

#### Reboot
```json
{
  "action": "reboot"
}
```

## Utility Functions

### Format Uptime
```javascript
function formatUptime(seconds) {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    
    if (hours > 24) {
        const days = Math.floor(hours / 24);
        return `${days}d ${hours % 24}h`;
    }
    
    return `${hours}h ${minutes}m`;
}
```

### Relative Time
```javascript
function getRelativeTime(timestamp) {
    const now = Date.now();
    const diff = now - timestamp;
    const seconds = Math.floor(diff / 1000);
    
    if (seconds < 60) return 'Just now';
    if (seconds < 3600) return `${Math.floor(seconds / 60)}m ago`;
    if (seconds < 86400) return `${Math.floor(seconds / 3600)}h ago`;
    return `${Math.floor(seconds / 86400)}d ago`;
}
```

## Performance

- Clock update: <10ms per second
- Device info fetch: <500ms
- Command send: <100ms
- Modal open: <100ms

## Error Handling

### Command Errors
```javascript
try {
    await syncNTPTime(deviceId);
    showNotification('Time sync requested', 'success');
} catch (error) {
    console.error('NTP sync failed:', error);
    showNotification('Failed to sync time', 'error');
}
```

### Missing Data
```javascript
function populateDeviceInfo(deviceId) {
    const data = snapshot.val();
    
    if (!data) {
        showNotification('Device data not found', 'error');
        return;
    }
    
    // Populate fields
}
```

## Browser Support

- Android 7.0+ WebView
- datetime-local input support
- setInterval for clock

## Dependencies

- Firebase Realtime Database
- MQTT client
- Notification system

## Related Documentation

- [../mqtt/README.md](../mqtt/README.md) - MQTT commands
- [../notifications/README.md](../notifications/README.md) - Notifications
- [../../README.md](../../README.md) - JavaScript overview
