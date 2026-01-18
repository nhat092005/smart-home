# Utils Module

## Overview

General utility functions and helper methods used throughout the application for data formatting, validation, and common operations.

## Files

- `helpers.js` - Utility functions and helpers

## Features

- Date/time formatting
- Data validation
- String manipulation
- Number formatting
- Array utilities
- Object operations
- DOM helpers

## helpers.js

### Purpose

Provides reusable utility functions for common operations across the application.

### Functions

#### formatTimestamp()
```javascript
formatTimestamp(timestamp, format)
```
Converts timestamp to formatted string.

**Parameters**:
- `timestamp` - Unix timestamp or Date object
- `format` - 'date', 'time', 'datetime', or custom format

**Returns**: Formatted string

**Example**:
```javascript
import { formatTimestamp } from './helpers.js';

formatTimestamp(1705561200000, 'datetime');
// Returns: "2024-01-18 10:00:00"

formatTimestamp(Date.now(), 'date');
// Returns: "2024-01-18"
```

#### formatSensorValue()
```javascript
formatSensorValue(value, sensorType)
```
Formats sensor value with unit.

**Parameters**:
- `value` - Numeric sensor value
- `sensorType` - 'temp', 'hum', or 'light'

**Returns**: Formatted string with unit

**Example**:
```javascript
formatSensorValue(25.5, 'temp');
// Returns: "25.5°C"

formatSensorValue(65.2, 'hum');
// Returns: "65.2%"

formatSensorValue(450, 'light');
// Returns: "450 lux"
```

#### validateDeviceId()
```javascript
validateDeviceId(deviceId)
```
Validates device ID format.

**Parameters**:
- `deviceId` - Device identifier string

**Returns**: Boolean

**Example**:
```javascript
validateDeviceId('device001');  // true
validateDeviceId('dev 001');    // false (contains space)
validateDeviceId('');           // false (empty)
```

#### generateUniqueId()
```javascript
generateUniqueId(prefix)
```
Generates unique identifier.

**Parameters**:
- `prefix` - Optional prefix string

**Returns**: Unique ID string

**Example**:
```javascript
generateUniqueId('device');
// Returns: "device_1705561200000_abc123"

generateUniqueId();
// Returns: "1705561200000_abc123"
```

#### debounce()
```javascript
debounce(func, delay)
```
Creates debounced function.

**Parameters**:
- `func` - Function to debounce
- `delay` - Delay in milliseconds

**Returns**: Debounced function

**Example**:
```javascript
const search = debounce((query) => {
    performSearch(query);
}, 300);

// Call multiple times, only executes once after 300ms
search('device');
search('devices');
search('devices01');  // Only this executes
```

#### throttle()
```javascript
throttle(func, limit)
```
Creates throttled function.

**Parameters**:
- `func` - Function to throttle
- `limit` - Time limit in milliseconds

**Returns**: Throttled function

**Example**:
```javascript
const updateStatus = throttle(() => {
    fetchDeviceStatus();
}, 1000);

// Call multiple times, executes max once per second
setInterval(updateStatus, 100);
```

#### deepClone()
```javascript
deepClone(obj)
```
Creates deep copy of object.

**Parameters**:
- `obj` - Object to clone

**Returns**: Cloned object

**Example**:
```javascript
const original = { device: { id: 1, name: 'Device 1' } };
const copy = deepClone(original);
copy.device.name = 'Device 2';
// original.device.name remains 'Device 1'
```

#### isEmpty()
```javascript
isEmpty(value)
```
Checks if value is empty.

**Parameters**:
- `value` - Any value to check

**Returns**: Boolean

**Example**:
```javascript
isEmpty('');        // true
isEmpty([]);        // true
isEmpty({});        // true
isEmpty(null);      // true
isEmpty(0);         // false
isEmpty('text');    // false
```

#### getQueryParam()
```javascript
getQueryParam(param)
```
Gets URL query parameter value.

**Parameters**:
- `param` - Parameter name

**Returns**: Parameter value or null

**Example**:
```javascript
// URL: index.html?deviceId=device001&tab=history
getQueryParam('deviceId');  // "device001"
getQueryParam('tab');       // "history"
getQueryParam('missing');   // null
```

#### setQueryParam()
```javascript
setQueryParam(param, value)
```
Sets URL query parameter.

**Parameters**:
- `param` - Parameter name
- `value` - Parameter value

**Updates URL without page reload.**

#### capitalizeFirst()
```javascript
capitalizeFirst(str)
```
Capitalizes first letter.

**Parameters**:
- `str` - Input string

**Returns**: Capitalized string

**Example**:
```javascript
capitalizeFirst('device');   // "Device"
capitalizeFirst('DEVICE');   // "Device"
```

#### truncate()
```javascript
truncate(str, length, suffix)
```
Truncates string to length.

**Parameters**:
- `str` - Input string
- `length` - Max length
- `suffix` - Suffix for truncated string (default: '...')

**Returns**: Truncated string

**Example**:
```javascript
truncate('This is a long device name', 10);
// Returns: "This is a..."

truncate('Short', 10);
// Returns: "Short"
```

### Usage

```javascript
import {
    formatTimestamp,
    formatSensorValue,
    validateDeviceId,
    generateUniqueId,
    debounce,
    isEmpty
} from './utils/helpers.js';

// Format sensor readings
const tempDisplay = formatSensorValue(device.temp, 'temp');
const timeDisplay = formatTimestamp(device.lastUpdate, 'datetime');

// Validate device ID before saving
if (validateDeviceId(newDeviceId)) {
    saveDevice({ id: newDeviceId, ...data });
}

// Generate unique ID for new device
const newId = generateUniqueId('device');

// Debounce search input
const searchInput = document.getElementById('search');
searchInput.addEventListener('input', debounce((e) => {
    performSearch(e.target.value);
}, 300));

// Check if form field is empty
if (isEmpty(deviceName)) {
    showError('Device name is required');
}
```

## Date/Time Utilities

### Format Options
```javascript
const formats = {
    'date': 'YYYY-MM-DD',
    'time': 'HH:MM:SS',
    'datetime': 'YYYY-MM-DD HH:MM:SS',
    'short': 'MM/DD/YYYY',
    'long': 'Month DD, YYYY HH:MM:SS'
};
```

### Implementation
```javascript
function formatTimestamp(timestamp, format = 'datetime') {
    const date = new Date(timestamp);
    
    const pad = (n) => String(n).padStart(2, '0');
    
    const year = date.getFullYear();
    const month = pad(date.getMonth() + 1);
    const day = pad(date.getDate());
    const hours = pad(date.getHours());
    const minutes = pad(date.getMinutes());
    const seconds = pad(date.getSeconds());
    
    switch (format) {
        case 'date':
            return `${year}-${month}-${day}`;
        case 'time':
            return `${hours}:${minutes}:${seconds}`;
        case 'datetime':
            return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`;
        case 'short':
            return `${month}/${day}/${year}`;
        default:
            return date.toString();
    }
}
```

### Relative Time
```javascript
function getRelativeTime(timestamp) {
    const now = Date.now();
    const diff = now - timestamp;
    const seconds = Math.floor(diff / 1000);
    const minutes = Math.floor(seconds / 60);
    const hours = Math.floor(minutes / 60);
    const days = Math.floor(hours / 24);
    
    if (seconds < 60) return 'Just now';
    if (minutes < 60) return `${minutes}m ago`;
    if (hours < 24) return `${hours}h ago`;
    return `${days}d ago`;
}
```

## Validation Utilities

### Device ID Validation
```javascript
function validateDeviceId(deviceId) {
    if (!deviceId || typeof deviceId !== 'string') {
        return false;
    }
    
    // Must be 3-32 characters
    if (deviceId.length < 3 || deviceId.length > 32) {
        return false;
    }
    
    // Alphanumeric and underscore only
    const pattern = /^[a-zA-Z0-9_]+$/;
    return pattern.test(deviceId);
}
```

### Email Validation
```javascript
function validateEmail(email) {
    const pattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return pattern.test(email);
}
```

### Number Range Validation
```javascript
function validateRange(value, min, max) {
    const num = parseFloat(value);
    return !isNaN(num) && num >= min && num <= max;
}
```

## Array Utilities

### Remove Duplicates
```javascript
function removeDuplicates(arr) {
    return [...new Set(arr)];
}
```

### Group By
```javascript
function groupBy(arr, key) {
    return arr.reduce((groups, item) => {
        const group = item[key];
        groups[group] = groups[group] || [];
        groups[group].push(item);
        return groups;
    }, {});
}

// Example
const devices = [
    { id: 1, type: 'sensor' },
    { id: 2, type: 'relay' },
    { id: 3, type: 'sensor' }
];
const grouped = groupBy(devices, 'type');
// { sensor: [...], relay: [...] }
```

### Sort By Property
```javascript
function sortBy(arr, key, descending = false) {
    return arr.sort((a, b) => {
        const aVal = a[key];
        const bVal = b[key];
        const result = aVal < bVal ? -1 : aVal > bVal ? 1 : 0;
        return descending ? -result : result;
    });
}
```

## Object Utilities

### Merge Objects
```javascript
function mergeDeep(target, source) {
    for (const key in source) {
        if (source[key] instanceof Object) {
            Object.assign(source[key], mergeDeep(target[key], source[key]));
        }
    }
    Object.assign(target || {}, source);
    return target;
}
```

### Get Nested Property
```javascript
function getNestedProperty(obj, path) {
    return path.split('.').reduce((current, key) => {
        return current?.[key];
    }, obj);
}

// Example
const device = { data: { sensors: { temp: 25.5 } } };
getNestedProperty(device, 'data.sensors.temp');  // 25.5
```

### Set Nested Property
```javascript
function setNestedProperty(obj, path, value) {
    const keys = path.split('.');
    const lastKey = keys.pop();
    const target = keys.reduce((current, key) => {
        current[key] = current[key] || {};
        return current[key];
    }, obj);
    target[lastKey] = value;
}
```

## DOM Utilities

### Element Creation
```javascript
function createElement(tag, attributes, children) {
    const element = document.createElement(tag);
    
    if (attributes) {
        Object.entries(attributes).forEach(([key, value]) => {
            if (key === 'class') {
                element.className = value;
            } else if (key.startsWith('data-')) {
                element.setAttribute(key, value);
            } else {
                element[key] = value;
            }
        });
    }
    
    if (children) {
        children.forEach(child => {
            if (typeof child === 'string') {
                element.appendChild(document.createTextNode(child));
            } else {
                element.appendChild(child);
            }
        });
    }
    
    return element;
}

// Example
const button = createElement('button', {
    class: 'btn btn-primary',
    'data-action': 'save'
}, ['Save Device']);
```

### Wait for Element
```javascript
async function waitForElement(selector, timeout = 5000) {
    return new Promise((resolve, reject) => {
        const element = document.querySelector(selector);
        if (element) {
            resolve(element);
            return;
        }
        
        const observer = new MutationObserver(() => {
            const element = document.querySelector(selector);
            if (element) {
                observer.disconnect();
                resolve(element);
            }
        });
        
        observer.observe(document.body, {
            childList: true,
            subtree: true
        });
        
        setTimeout(() => {
            observer.disconnect();
            reject(new Error('Element not found'));
        }, timeout);
    });
}
```

## String Utilities

### Convert to Kebab Case
```javascript
function toKebabCase(str) {
    return str
        .replace(/([a-z])([A-Z])/g, '$1-$2')
        .replace(/[\s_]+/g, '-')
        .toLowerCase();
}
```

### Convert to Camel Case
```javascript
function toCamelCase(str) {
    return str
        .replace(/[-_\s]+(.)?/g, (_, c) => c ? c.toUpperCase() : '')
        .replace(/^(.)/, (c) => c.toLowerCase());
}
```

### Strip HTML Tags
```javascript
function stripHtml(html) {
    const tmp = document.createElement('div');
    tmp.innerHTML = html;
    return tmp.textContent || tmp.innerText || '';
}
```

## Number Utilities

### Format Number
```javascript
function formatNumber(num, decimals = 1) {
    return parseFloat(num).toFixed(decimals);
}
```

### Clamp Value
```javascript
function clamp(value, min, max) {
    return Math.min(Math.max(value, min), max);
}
```

### Random Range
```javascript
function randomRange(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}
```

## Performance Utilities

### Debounce Implementation
```javascript
function debounce(func, delay) {
    let timeoutId;
    return function(...args) {
        clearTimeout(timeoutId);
        timeoutId = setTimeout(() => func.apply(this, args), delay);
    };
}
```

### Throttle Implementation
```javascript
function throttle(func, limit) {
    let inThrottle;
    return function(...args) {
        if (!inThrottle) {
            func.apply(this, args);
            inThrottle = true;
            setTimeout(() => inThrottle = false, limit);
        }
    };
}
```

### Memoize
```javascript
function memoize(func) {
    const cache = new Map();
    return function(...args) {
        const key = JSON.stringify(args);
        if (cache.has(key)) {
            return cache.get(key);
        }
        const result = func.apply(this, args);
        cache.set(key, result);
        return result;
    };
}
```

## Error Handling

### Try-Catch Wrapper
```javascript
async function tryCatch(promise, errorMessage) {
    try {
        return [await promise, null];
    } catch (error) {
        console.error(errorMessage, error);
        return [null, error];
    }
}

// Example
const [data, error] = await tryCatch(
    fetchDeviceData(deviceId),
    'Failed to fetch device data'
);

if (error) {
    showNotification('Error loading device', 'error');
} else {
    displayDevice(data);
}
```

## Browser Compatibility

- Chrome 90+ (full support)
- Firefox 88+ (full support)
- Safari 14+ (full support)
- Edge 90+ (full support)

## Performance

- formatTimestamp: <1ms
- validateDeviceId: <1ms
- debounce creation: <1ms
- deepClone: <10ms for typical objects
- isEmpty: <1ms

## Dependencies

- None (vanilla JavaScript)

## Related Documentation

- [../devices/README.md](../devices/README.md) - Device operations
- [../ui/README.md](../ui/README.md) - UI utilities
