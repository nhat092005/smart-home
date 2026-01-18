# Settings Module

## Overview

Application configuration module for managing MQTT broker settings, Firebase credentials, and device management.

## Files

- `settings-manager.js` - Settings CRUD operations and persistence

## Features

- MQTT broker configuration
- Firebase configuration
- Device management
- Settings persistence (localStorage)
- Validation
- Import/export settings

## settings-manager.js

### Purpose

Manages application settings with local storage persistence and validation.

### Functions

#### loadSettings()
```javascript
loadSettings()
```
Loads settings from localStorage.

**Returns**: Object with all settings or defaults

**Example**:
```javascript
import { loadSettings } from './settings-manager.js';

const settings = loadSettings();
console.log(settings.mqttBroker);
```

#### saveSettings()
```javascript
saveSettings(settings)
```
Saves settings to localStorage.

**Parameters**:
- `settings` - Settings object

**Example**:
```javascript
import { saveSettings } from './settings-manager.js';

const newSettings = {
    mqttBroker: 'wss://broker.example.com:8081',
    firebaseApiKey: 'YOUR_API_KEY',
    // ...
};
saveSettings(newSettings);
```

#### updateMqttSettings()
```javascript
updateMqttSettings(broker, port, username, password)
```
Updates MQTT broker configuration.

**Parameters**:
- `broker` - Broker hostname
- `port` - Broker port
- `username` - MQTT username (optional)
- `password` - MQTT password (optional)

#### updateFirebaseSettings()
```javascript
updateFirebaseSettings(config)
```
Updates Firebase configuration.

**Parameters**:
- `config` - Firebase config object

**Example**:
```javascript
updateFirebaseSettings({
    apiKey: "YOUR_API_KEY",
    authDomain: "your-app.firebaseapp.com",
    databaseURL: "https://your-app.firebaseio.com",
    projectId: "your-app",
    storageBucket: "your-app.appspot.com",
    messagingSenderId: "123456789",
    appId: "1:123456789:web:abcdef"
});
```

#### validateSettings()
```javascript
validateSettings(settings)
```
Validates settings object.

**Parameters**:
- `settings` - Settings object to validate

**Returns**: Object with `valid` boolean and `errors` array

#### exportSettings()
```javascript
exportSettings()
```
Exports settings as JSON file.

**Triggers browser download of settings.json**

#### importSettings()
```javascript
importSettings(file)
```
Imports settings from JSON file.

**Parameters**:
- `file` - File object from input element

**Returns**: Promise that resolves with imported settings

### Usage

```javascript
import {
    loadSettings,
    saveSettings,
    updateMqttSettings,
    exportSettings
} from './settings/settings-manager.js';

// Load on page init
const settings = loadSettings();
populateSettingsForm(settings);

// Save from form
document.getElementById('btn-save').addEventListener('click', () => {
    const broker = document.getElementById('mqtt-broker').value;
    const port = document.getElementById('mqtt-port').value;
    const username = document.getElementById('mqtt-username').value;
    const password = document.getElementById('mqtt-password').value;
    
    updateMqttSettings(broker, port, username, password);
});

// Export settings
document.getElementById('btn-export-settings').addEventListener('click', () => {
    exportSettings();
});
```

## Settings Structure

### Default Settings
```javascript
{
    mqttBroker: 'wss://localhost:8081',
    mqttPort: 8081,
    mqttUsername: '',
    mqttPassword: '',
    mqttClientId: 'web_client_' + Date.now(),
    
    firebaseApiKey: '',
    firebaseAuthDomain: '',
    firebaseDatabaseURL: '',
    firebaseProjectId: '',
    firebaseStorageBucket: '',
    firebaseMessagingSenderId: '',
    firebaseAppId: '',
    
    theme: 'dark',
    language: 'en',
    notifications: true,
    autoRefresh: true,
    refreshInterval: 5000
}
```

### MQTT Settings
```javascript
{
    mqttBroker: 'wss://broker.example.com:8081',
    mqttPort: 8081,
    mqttUsername: 'admin',
    mqttPassword: 'password123',
    mqttClientId: 'web_client_1705561200000',
    mqttKeepAlive: 60,
    mqttCleanSession: true,
    mqttReconnectPeriod: 3000
}
```

### Firebase Settings
```javascript
{
    firebaseApiKey: "AIzaSyABCDEFGH...",
    firebaseAuthDomain: "smart-home-12345.firebaseapp.com",
    firebaseDatabaseURL: "https://smart-home-12345.firebaseio.com",
    firebaseProjectId: "smart-home-12345",
    firebaseStorageBucket: "smart-home-12345.appspot.com",
    firebaseMessagingSenderId: "123456789012",
    firebaseAppId: "1:123456789012:web:abcdef123456"
}
```

## Storage

### localStorage Keys
```javascript
'smart_home_settings'        // Main settings object
'smart_home_mqtt_broker'     // Quick access to broker
'smart_home_firebase_config' // Quick access to Firebase
```

### Save Implementation
```javascript
function saveSettings(settings) {
    try {
        localStorage.setItem('smart_home_settings', JSON.stringify(settings));
        return true;
    } catch (error) {
        console.error('Save failed:', error);
        return false;
    }
}
```

### Load Implementation
```javascript
function loadSettings() {
    try {
        const stored = localStorage.getItem('smart_home_settings');
        if (stored) {
            return JSON.parse(stored);
        }
    } catch (error) {
        console.error('Load failed:', error);
    }
    return getDefaultSettings();
}
```

## Validation

### MQTT Validation
```javascript
function validateMqttSettings(settings) {
    const errors = [];
    
    if (!settings.mqttBroker) {
        errors.push('MQTT broker is required');
    }
    
    if (!settings.mqttPort || settings.mqttPort < 1 || settings.mqttPort > 65535) {
        errors.push('Invalid MQTT port');
    }
    
    if (settings.mqttBroker.startsWith('wss://') && settings.mqttPort !== 8081) {
        errors.push('WSS typically uses port 8081');
    }
    
    return errors;
}
```

### Firebase Validation
```javascript
function validateFirebaseSettings(settings) {
    const errors = [];
    const required = [
        'firebaseApiKey',
        'firebaseAuthDomain',
        'firebaseDatabaseURL',
        'firebaseProjectId'
    ];
    
    required.forEach(field => {
        if (!settings[field]) {
            errors.push(`${field} is required`);
        }
    });
    
    return errors;
}
```

## UI Components

### Settings Form
```html
<form id="settings-form">
    <h3>MQTT Broker</h3>
    <label>Broker URL:
        <input type="text" id="mqtt-broker" placeholder="wss://broker.example.com">
    </label>
    <label>Port:
        <input type="number" id="mqtt-port" value="8081">
    </label>
    <label>Username:
        <input type="text" id="mqtt-username">
    </label>
    <label>Password:
        <input type="password" id="mqtt-password">
    </label>
    
    <h3>Firebase Configuration</h3>
    <label>API Key:
        <input type="text" id="firebase-api-key">
    </label>
    <label>Auth Domain:
        <input type="text" id="firebase-auth-domain">
    </label>
    <!-- More Firebase fields -->
    
    <button type="button" id="btn-save">Save Settings</button>
    <button type="button" id="btn-reset">Reset to Defaults</button>
</form>
```

### Import/Export
```html
<div class="settings-actions">
    <button id="btn-export-settings">Export Settings</button>
    <label>
        Import Settings:
        <input type="file" id="file-import-settings" accept=".json">
    </label>
</div>
```

## Import/Export

### Export Format (JSON)
```json
{
  "version": "1.0",
  "exported": "2024-01-18T10:00:00Z",
  "settings": {
    "mqttBroker": "wss://broker.example.com:8081",
    "mqttPort": 8081,
    "firebaseApiKey": "...",
    "theme": "dark"
  }
}
```

### Export Implementation
```javascript
function exportSettings() {
    const settings = loadSettings();
    const exportData = {
        version: '1.0',
        exported: new Date().toISOString(),
        settings: settings
    };
    
    const blob = new Blob([JSON.stringify(exportData, null, 2)], {
        type: 'application/json'
    });
    
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'smart_home_settings.json';
    a.click();
    URL.revokeObjectURL(url);
}
```

### Import Implementation
```javascript
async function importSettings(file) {
    return new Promise((resolve, reject) => {
        const reader = new FileReader();
        
        reader.onload = (e) => {
            try {
                const data = JSON.parse(e.target.result);
                
                if (data.version !== '1.0') {
                    reject(new Error('Unsupported settings version'));
                    return;
                }
                
                saveSettings(data.settings);
                resolve(data.settings);
            } catch (error) {
                reject(new Error('Invalid settings file'));
            }
        };
        
        reader.onerror = () => reject(new Error('Failed to read file'));
        reader.readAsText(file);
    });
}
```

## Settings Migration

### Version Updates
```javascript
function migrateSettings(oldSettings, oldVersion) {
    let settings = { ...oldSettings };
    
    // v1.0 to v1.1
    if (oldVersion < '1.1') {
        settings.mqttReconnectPeriod = 3000;
        settings.mqttCleanSession = true;
    }
    
    // v1.1 to v1.2
    if (oldVersion < '1.2') {
        settings.theme = settings.theme || 'dark';
        settings.language = settings.language || 'en';
    }
    
    return settings;
}
```

## Event Handling

### Settings Changed Event
```javascript
window.addEventListener('settingsChanged', (event) => {
    console.log('Settings updated:', event.detail);
    
    // Reconnect MQTT if broker changed
    if (event.detail.mqttChanged) {
        reconnectMqtt();
    }
    
    // Reinitialize Firebase if config changed
    if (event.detail.firebaseChanged) {
        reinitializeFirebase();
    }
});
```

### Dispatch Event
```javascript
function notifySettingsChange(changedFields) {
    const event = new CustomEvent('settingsChanged', {
        detail: {
            mqttChanged: changedFields.includes('mqtt'),
            firebaseChanged: changedFields.includes('firebase'),
            fields: changedFields
        }
    });
    window.dispatchEvent(event);
}
```

## Error Handling

### localStorage Full
```javascript
try {
    localStorage.setItem('smart_home_settings', JSON.stringify(settings));
} catch (error) {
    if (error.name === 'QuotaExceededError') {
        alert('Storage quota exceeded. Clear browser data.');
    }
}
```

### Invalid Settings
```javascript
const validation = validateSettings(settings);
if (!validation.valid) {
    console.error('Invalid settings:', validation.errors);
    showNotification('Settings validation failed', 'error');
    return false;
}
```

## Performance

- Load settings: <10ms
- Save settings: <20ms
- Validate settings: <5ms
- Export to file: <100ms
- Import from file: <200ms

## Security

### Sensitive Data
```javascript
// Never log passwords
function sanitizeForLog(settings) {
    return {
        ...settings,
        mqttPassword: '***',
        firebaseApiKey: '***'
    };
}
```

### Encryption (Optional)
```javascript
function encryptSensitiveFields(settings) {
    // Implement encryption if needed
    return {
        ...settings,
        mqttPassword: encrypt(settings.mqttPassword),
        firebaseApiKey: encrypt(settings.firebaseApiKey)
    };
}
```

## Browser Compatibility

- Chrome 90+ (full support)
- Firefox 88+ (full support)
- Safari 14+ (full support)
- Edge 90+ (full support)

Requires localStorage support.

## Dependencies

- None (vanilla JavaScript)

## Related Documentation

- [../mqtt/README.md](../mqtt/README.md) - MQTT connection
- [../core/README.md](../core/README.md) - Firebase config
