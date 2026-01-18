# Notifications Module

## Overview

Toast notification system for displaying user feedback messages in the mobile application.

## Files

- `notifications.js` - Notification display and management

## notifications.js

### Purpose

Creates and manages toast notifications for success, error, warning, and info messages.

### Functions

#### showNotification()
```javascript
showNotification(message, type, duration)
```
Displays toast notification.

**Parameters:**
- `message` - Notification text
- `type` - 'success', 'error', 'warning', or 'info'
- `duration` - Display time in milliseconds (default: 3000)

**Example:**
```javascript
import { showNotification } from './notifications.js';

showNotification('Device updated successfully', 'success', 3000);
showNotification('Connection failed', 'error', 5000);
showNotification('Battery low', 'warning', 4000);
showNotification('New update available', 'info', 3000);
```

#### hideNotification()
```javascript
hideNotification(notificationId)
```
Manually dismisses notification.

**Parameters:**
- `notificationId` - Notification element ID

#### clearAllNotifications()
```javascript
clearAllNotifications()
```
Removes all active notifications.

### Notification Types

#### Success
```javascript
showNotification('Device added successfully', 'success');
```
- Green background
- Checkmark icon
- Default duration: 3s

#### Error
```javascript
showNotification('Failed to connect to broker', 'error');
```
- Red background
- Error icon
- Default duration: 5s

#### Warning
```javascript
showNotification('Device offline for 5 minutes', 'warning');
```
- Yellow background
- Warning icon
- Default duration: 4s

#### Info
```javascript
showNotification('Syncing data...', 'info');
```
- Blue background
- Info icon
- Default duration: 3s

## UI Structure

### Notification Container
```html
<div id="notification-container" class="notification-container"></div>
```

### Toast HTML
```html
<div class="toast toast-success">
    <div class="toast-icon">
        <i class="fas fa-check-circle"></i>
    </div>
    <div class="toast-content">
        <div class="toast-message">Device updated successfully</div>
    </div>
    <button class="toast-close" onclick="hideNotification(id)">
        <i class="fas fa-times"></i>
    </button>
</div>
```

## Styling

### Toast Classes
```css
.toast {
    display: flex;
    align-items: center;
    gap: 12px;
    padding: 16px;
    border-radius: 8px;
    margin-bottom: 8px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    animation: slideIn 0.3s ease;
}

.toast-success {
    background: rgba(16, 185, 129, 0.2);
    border-left: 4px solid var(--success);
}

.toast-error {
    background: rgba(239, 68, 68, 0.2);
    border-left: 4px solid var(--error);
}

.toast-warning {
    background: rgba(245, 158, 11, 0.2);
    border-left: 4px solid var(--warning);
}

.toast-info {
    background: rgba(59, 130, 246, 0.2);
    border-left: 4px solid var(--info);
}
```

### Animation
```css
@keyframes slideIn {
    from {
        transform: translateX(100%);
        opacity: 0;
    }
    to {
        transform: translateX(0);
        opacity: 1;
    }
}

@keyframes fadeOut {
    from {
        opacity: 1;
    }
    to {
        opacity: 0;
    }
}
```

## Position

### Fixed Bottom Right
```css
.notification-container {
    position: fixed;
    bottom: calc(var(--bottom-nav-height) + 16px);
    left: 16px;
    right: 16px;
    z-index: 999;
    pointer-events: none;
}

.toast {
    pointer-events: auto;
}
```

## Auto-dismiss

```javascript
function showNotification(message, type = 'info', duration = 3000) {
    const toast = createToast(message, type);
    container.appendChild(toast);
    
    setTimeout(() => {
        toast.classList.add('toast-fade-out');
        setTimeout(() => {
            toast.remove();
        }, 300);
    }, duration);
}
```

## Queue Management

### Multiple Notifications
```javascript
const notificationQueue = [];
const MAX_VISIBLE = 3;

function showNotification(message, type, duration) {
    if (getVisibleCount() >= MAX_VISIBLE) {
        notificationQueue.push({ message, type, duration });
        return;
    }
    
    displayNotification(message, type, duration);
}

function onNotificationDismissed() {
    if (notificationQueue.length > 0) {
        const next = notificationQueue.shift();
        displayNotification(next.message, next.type, next.duration);
    }
}
```

## Icon Mapping

```javascript
const icons = {
    success: 'fa-check-circle',
    error: 'fa-times-circle',
    warning: 'fa-exclamation-triangle',
    info: 'fa-info-circle'
};

function getIcon(type) {
    return icons[type] || icons.info;
}
```

## Close Button

```javascript
toast.querySelector('.toast-close').addEventListener('click', () => {
    toast.classList.add('toast-fade-out');
    setTimeout(() => {
        toast.remove();
        onNotificationDismissed();
    }, 300);
});
```

## Usage Examples

### Device Operations
```javascript
// Device added
showNotification('Device added successfully', 'success');

// Device updated
showNotification('Device settings updated', 'success');

// Device deleted
showNotification('Device removed', 'info');
```

### Connection Status
```javascript
// MQTT connected
showNotification('Connected to broker', 'success');

// MQTT disconnected
showNotification('Connection lost. Reconnecting...', 'warning');

// Firebase error
showNotification('Database sync failed', 'error', 5000);
```

### User Actions
```javascript
// Settings saved
showNotification('Settings saved', 'success');

// Time synced
showNotification('Time synchronized with NTP', 'success');

// Device rebooted
showNotification('Device rebooting...', 'info', 2000);
```

## Accessibility

### Screen Reader Support
```html
<div class="toast" role="alert" aria-live="polite">
    <div class="toast-message">Message text</div>
</div>
```

### Keyboard Navigation
```javascript
toast.querySelector('.toast-close').setAttribute('aria-label', 'Close notification');
```

## Performance

- Toast creation: <20ms
- Animation: 300ms
- Auto-dismiss: Configurable
- Max simultaneous: 3 (recommended)

## Mobile Optimization

- Touch-friendly close button (44px)
- Swipe to dismiss (optional)
- Bottom positioning above navigation
- Responsive width

## Error Prevention

### Validation
```javascript
function showNotification(message, type, duration) {
    if (!message || typeof message !== 'string') {
        console.error('Invalid notification message');
        return;
    }
    
    if (!['success', 'error', 'warning', 'info'].includes(type)) {
        type = 'info';
    }
    
    if (duration < 1000 || duration > 10000) {
        duration = 3000;
    }
    
    displayNotification(message, type, duration);
}
```

## Browser Support

- Android 7.0+ WebView
- CSS Animations
- Flexbox

## Dependencies

- Font Awesome (for icons)

## Related Documentation

- [../ui/README.md](../ui/README.md) - UI utilities
- [../../README.md](../../README.md) - JavaScript overview
