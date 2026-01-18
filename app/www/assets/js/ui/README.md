# UI Module

## Overview

UI utilities module for common interface operations including modals, bottom navigation, and helper functions.

## Files

- `ui-helpers.js` - UI utility functions

## ui-helpers.js

### Purpose

Provides reusable UI functions for navigation, modals, and interface state management.

### Functions

#### showModal()
```javascript
showModal(modalId)
```
Displays modal dialog.

**Parameters:**
- `modalId` - HTML element ID

**Example:**
```javascript
import { showModal } from './ui-helpers.js';

showModal('wifi-guide-modal');
```

#### hideModal()
```javascript
hideModal(modalId)
```
Closes modal dialog.

**Parameters:**
- `modalId` - HTML element ID

#### switchTab()
```javascript
switchTab(tabName)
```
Switches between bottom navigation tabs.

**Parameters:**
- `tabName` - 'home', 'dashboard', or 'settings'

**Example:**
```javascript
document.querySelector('[data-tab="home"]').addEventListener('click', () => {
    switchTab('home');
});
```

#### showLoading()
```javascript
showLoading(elementId)
```
Displays loading spinner.

**Parameters:**
- `elementId` - Container element ID

#### hideLoading()
```javascript
hideLoading(elementId)
```
Removes loading spinner.

#### showConfirmDialog()
```javascript
showConfirmDialog(title, message)
```
Displays confirmation dialog.

**Parameters:**
- `title` - Dialog title
- `message` - Confirmation message

**Returns:** Promise resolving to boolean

**Example:**
```javascript
const confirmed = await showConfirmDialog(
    'Reboot Device',
    'Device will restart. Continue?'
);

if (confirmed) {
    rebootDevice();
}
```

## Bottom Navigation

### Tab Switching
```javascript
function switchTab(tabName) {
    // Hide all tabs
    document.querySelectorAll('.tab-content').forEach(tab => {
        tab.classList.remove('active');
    });
    
    // Show selected tab
    document.getElementById(`${tabName}-tab`).classList.add('active');
    
    // Update nav item states
    document.querySelectorAll('.nav-item').forEach(item => {
        item.classList.remove('active');
    });
    
    document.querySelector(`[data-tab="${tabName}"]`).classList.add('active');
}
```

### Navigation HTML
```html
<nav class="bottom-nav">
    <div class="nav-item active" data-tab="home" onclick="switchTab('home')">
        <i class="fas fa-home icon"></i>
        <span class="label">Home</span>
    </div>
    <div class="nav-item" data-tab="dashboard" onclick="switchTab('dashboard')">
        <i class="fas fa-chart-line icon"></i>
        <span class="label">Dashboard</span>
    </div>
    <div class="nav-item" data-tab="settings" onclick="switchTab('settings')">
        <i class="fas fa-cog icon"></i>
        <span class="label">Settings</span>
    </div>
</nav>
```

## Modal Management

### Show Modal
```javascript
function showModal(modalId) {
    const modal = document.getElementById(modalId);
    if (!modal) return;
    
    modal.style.display = 'flex';
    document.body.style.overflow = 'hidden';
    
    // Add backdrop
    const backdrop = document.createElement('div');
    backdrop.className = 'modal-backdrop';
    backdrop.onclick = () => hideModal(modalId);
    document.body.appendChild(backdrop);
}
```

### Hide Modal
```javascript
function hideModal(modalId) {
    const modal = document.getElementById(modalId);
    if (!modal) return;
    
    modal.style.display = 'none';
    document.body.style.overflow = '';
    
    // Remove backdrop
    const backdrop = document.querySelector('.modal-backdrop');
    if (backdrop) backdrop.remove();
}
```

### ESC Key Support
```javascript
document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape') {
        const openModal = document.querySelector('.modal[style*="display: flex"]');
        if (openModal) {
            hideModal(openModal.id);
        }
    }
});
```

## Loading States

### Show Loading
```javascript
function showLoading(elementId) {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    const spinner = document.createElement('div');
    spinner.className = 'loading-spinner';
    spinner.innerHTML = `
        <div class="spinner"></div>
        <p>Loading...</p>
    `;
    
    element.innerHTML = '';
    element.appendChild(spinner);
}
```

### Hide Loading
```javascript
function hideLoading(elementId) {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    const spinner = element.querySelector('.loading-spinner');
    if (spinner) {
        spinner.remove();
    }
}
```

## Confirmation Dialogs

### Custom Confirm
```javascript
function showConfirmDialog(title, message) {
    return new Promise((resolve) => {
        const dialog = document.createElement('div');
        dialog.className = 'confirm-dialog';
        dialog.innerHTML = `
            <div class="confirm-content">
                <h3>${title}</h3>
                <p>${message}</p>
                <div class="confirm-actions">
                    <button id="btn-cancel" class="btn btn-secondary">Cancel</button>
                    <button id="btn-confirm" class="btn btn-primary">Confirm</button>
                </div>
            </div>
        `;
        
        document.body.appendChild(dialog);
        
        dialog.querySelector('#btn-confirm').onclick = () => {
            dialog.remove();
            resolve(true);
        };
        
        dialog.querySelector('#btn-cancel').onclick = () => {
            dialog.remove();
            resolve(false);
        };
    });
}
```

## Scroll Management

### Scroll to Top
```javascript
function scrollToTop(smooth = true) {
    window.scrollTo({
        top: 0,
        behavior: smooth ? 'smooth' : 'auto'
    });
}
```

### Scroll to Element
```javascript
function scrollToElement(elementId) {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    element.scrollIntoView({
        behavior: 'smooth',
        block: 'start'
    });
}
```

## Active States

### Update Active Button
```javascript
function updateActiveButton(buttonGroup, activeId) {
    const buttons = document.querySelectorAll(`.${buttonGroup}`);
    buttons.forEach(btn => btn.classList.remove('active'));
    
    document.getElementById(activeId).classList.add('active');
}
```

## Page Transitions

### Fade In
```javascript
function fadeIn(elementId, duration = 300) {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    element.style.opacity = '0';
    element.style.display = 'block';
    
    let start = null;
    function animate(timestamp) {
        if (!start) start = timestamp;
        const progress = (timestamp - start) / duration;
        
        element.style.opacity = Math.min(progress, 1);
        
        if (progress < 1) {
            requestAnimationFrame(animate);
        }
    }
    
    requestAnimationFrame(animate);
}
```

## Utility Classes

### Toggle Class
```javascript
function toggleClass(elementId, className) {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    element.classList.toggle(className);
}
```

### Add/Remove Class
```javascript
function addClass(elementId, className) {
    document.getElementById(elementId)?.classList.add(className);
}

function removeClass(elementId, className) {
    document.getElementById(elementId)?.classList.remove(className);
}
```

## Event Delegation

### Setup Delegation
```javascript
function delegateEvent(selector, event, handler) {
    document.addEventListener(event, (e) => {
        if (e.target.matches(selector)) {
            handler(e);
        }
    });
}

// Example
delegateEvent('.device-card', 'click', (e) => {
    const deviceId = e.target.dataset.deviceId;
    openReportDetail(deviceId);
});
```

## Debouncing

### Debounce Function
```javascript
function debounce(func, delay) {
    let timeoutId;
    return function(...args) {
        clearTimeout(timeoutId);
        timeoutId = setTimeout(() => func.apply(this, args), delay);
    };
}

// Example
const debouncedSearch = debounce((query) => {
    performSearch(query);
}, 300);
```

## Touch Feedback

### Ripple Effect
```javascript
function addRippleEffect(button) {
    button.addEventListener('click', function(e) {
        const ripple = document.createElement('span');
        ripple.className = 'ripple-effect';
        
        const rect = button.getBoundingClientRect();
        const size = Math.max(rect.width, rect.height);
        const x = e.clientX - rect.left - size / 2;
        const y = e.clientY - rect.top - size / 2;
        
        ripple.style.width = ripple.style.height = size + 'px';
        ripple.style.left = x + 'px';
        ripple.style.top = y + 'px';
        
        button.appendChild(ripple);
        
        setTimeout(() => ripple.remove(), 600);
    });
}
```

## Safe Area Support

### Apply Safe Area
```javascript
function applySafeArea() {
    const root = document.documentElement;
    root.style.setProperty('--safe-area-top', 'env(safe-area-inset-top)');
    root.style.setProperty('--safe-area-bottom', 'env(safe-area-inset-bottom)');
    root.style.setProperty('--safe-area-left', 'env(safe-area-inset-left)');
    root.style.setProperty('--safe-area-right', 'env(safe-area-inset-right)');
}
```

## Performance

- Tab switch: <50ms
- Modal open/close: <100ms
- Debounce delay: 300ms default
- Animation frame: 60fps target

## Accessibility

### Focus Management
```javascript
function trapFocus(modalId) {
    const modal = document.getElementById(modalId);
    const focusableElements = modal.querySelectorAll(
        'button, [href], input, select, textarea, [tabindex]:not([tabindex="-1"])'
    );
    
    const firstElement = focusableElements[0];
    const lastElement = focusableElements[focusableElements.length - 1];
    
    modal.addEventListener('keydown', (e) => {
        if (e.key !== 'Tab') return;
        
        if (e.shiftKey) {
            if (document.activeElement === firstElement) {
                lastElement.focus();
                e.preventDefault();
            }
        } else {
            if (document.activeElement === lastElement) {
                firstElement.focus();
                e.preventDefault();
            }
        }
    });
}
```

## Browser Support

- Android 7.0+ WebView
- Promise support
- requestAnimationFrame
- classList API

## Dependencies

- None (vanilla JavaScript)

## Related Documentation

- [../notifications/README.md](../notifications/README.md) - Notifications
- [../../README.md](../../README.md) - JavaScript overview
