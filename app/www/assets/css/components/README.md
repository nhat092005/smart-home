# Component Styles

## Overview

Reusable UI component styles for buttons, cards, badges, modals, forms, and other interface elements used throughout the mobile application.

## Components

- Buttons
- Cards
- Badges
- Modals
- Forms
- Tables
- Charts
- Notifications

## Buttons

### Primary Button
```css
.btn {
    padding: 12px 24px;
    border-radius: var(--border-radius);
    font-weight: var(--font-weight-medium);
    cursor: pointer;
    transition: all 0.3s ease;
    min-height: var(--touch-target-min);
}

.btn-primary {
    background: var(--primary);
    color: white;
    border: none;
}

.btn-primary:active {
    background: var(--primary-dark);
}
```

### Icon Button
```css
.btn-icon {
    width: 44px;
    height: 44px;
    border-radius: 50%;
    display: flex;
    align-items: center;
    justify-content: center;
    background: var(--bg-tertiary);
}
```

### Toggle Button
```css
.toggle-btn {
    width: 60px;
    height: 32px;
    border-radius: 16px;
    background: var(--bg-tertiary);
    position: relative;
    transition: background 0.3s;
}

.toggle-btn.active {
    background: var(--primary);
}
```

## Cards

### Device Card
```css
.device-card {
    background: var(--bg-secondary);
    border-radius: var(--border-radius);
    padding: var(--spacing-md);
    box-shadow: var(--shadow-md);
    transition: transform 0.2s;
}

.device-card:active {
    transform: scale(0.98);
}
```

### Sensor Card
```css
.sensor-card {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
    background: var(--bg-tertiary);
    padding: var(--spacing-md);
    border-radius: var(--border-radius);
}

.sensor-card .icon {
    font-size: 24px;
    color: var(--primary);
}

.sensor-card .value {
    font-size: var(--font-size-xl);
    font-weight: var(--font-weight-bold);
}
```

## Badges

### Status Badge
```css
.badge {
    display: inline-block;
    padding: 4px 12px;
    border-radius: 12px;
    font-size: var(--font-size-sm);
    font-weight: var(--font-weight-medium);
}

.badge-success {
    background: rgba(16, 185, 129, 0.2);
    color: var(--success);
}

.badge-error {
    background: rgba(239, 68, 68, 0.2);
    color: var(--error);
}
```

## Modals

### Modal Container
```css
.modal {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: rgba(0, 0, 0, 0.8);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
}

.modal-content {
    background: var(--bg-secondary);
    border-radius: var(--border-radius);
    padding: var(--spacing-lg);
    max-width: 90%;
    max-height: 90%;
    overflow-y: auto;
}
```

### Modal Header
```css
.modal-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: var(--spacing-md);
}

.modal-close {
    width: 32px;
    height: 32px;
    border-radius: 50%;
    background: var(--bg-tertiary);
    border: none;
    color: var(--text-primary);
    font-size: 20px;
}
```

## Forms

### Input Field
```css
.form-group {
    margin-bottom: var(--spacing-md);
}

.form-label {
    display: block;
    margin-bottom: var(--spacing-sm);
    font-weight: var(--font-weight-medium);
    color: var(--text-secondary);
}

.form-input {
    width: 100%;
    padding: 12px;
    border-radius: var(--border-radius);
    border: 1px solid var(--border-color);
    background: var(--bg-tertiary);
    color: var(--text-primary);
    font-size: var(--font-size-base);
    min-height: var(--touch-target-min);
}
```

### Select Dropdown
```css
.form-select {
    width: 100%;
    padding: 12px;
    border-radius: var(--border-radius);
    border: 1px solid var(--border-color);
    background: var(--bg-tertiary);
    color: var(--text-primary);
    min-height: var(--touch-target-min);
}
```

## Tables

### Device Table
```css
.table {
    width: 100%;
    border-collapse: collapse;
}

.table th {
    background: var(--bg-tertiary);
    padding: var(--spacing-sm);
    text-align: left;
    font-weight: var(--font-weight-semibold);
    color: var(--text-secondary);
}

.table td {
    padding: var(--spacing-sm);
    border-bottom: 1px solid var(--border-color);
}

.table tr:last-child td {
    border-bottom: none;
}
```

## Charts

### Chart Container
```css
.chart-container {
    background: var(--bg-secondary);
    padding: var(--spacing-md);
    border-radius: var(--border-radius);
    margin-bottom: var(--spacing-md);
}

.chart-controls {
    display: flex;
    gap: var(--spacing-sm);
    margin-bottom: var(--spacing-md);
}

.chart-btn {
    flex: 1;
    padding: 8px;
    border-radius: var(--border-radius);
    background: var(--bg-tertiary);
    border: none;
    color: var(--text-secondary);
    font-size: var(--font-size-sm);
}

.chart-btn.active {
    background: var(--primary);
    color: white;
}
```

## Notifications

### Toast Notification
```css
.toast {
    position: fixed;
    bottom: calc(var(--bottom-nav-height) + var(--spacing-md));
    left: var(--spacing-md);
    right: var(--spacing-md);
    padding: var(--spacing-md);
    border-radius: var(--border-radius);
    background: var(--bg-secondary);
    box-shadow: var(--shadow-lg);
    z-index: 999;
    animation: slideUp 0.3s ease;
}

.toast-success {
    border-left: 4px solid var(--success);
}

.toast-error {
    border-left: 4px solid var(--error);
}

@keyframes slideUp {
    from {
        transform: translateY(100%);
        opacity: 0;
    }
    to {
        transform: translateY(0);
        opacity: 1;
    }
}
```

## Loading States

### Spinner
```css
.spinner {
    width: 40px;
    height: 40px;
    border: 4px solid var(--bg-tertiary);
    border-top-color: var(--primary);
    border-radius: 50%;
    animation: spin 1s linear infinite;
}

@keyframes spin {
    to { transform: rotate(360deg); }
}
```

### Skeleton Loader
```css
.skeleton {
    background: linear-gradient(
        90deg,
        var(--bg-secondary) 0%,
        var(--bg-tertiary) 50%,
        var(--bg-secondary) 100%
    );
    background-size: 200% 100%;
    animation: shimmer 1.5s infinite;
}

@keyframes shimmer {
    to { background-position: -200% 0; }
}
```

## Touch Interactions

### Active States
```css
.interactive:active {
    transform: scale(0.98);
    opacity: 0.8;
}
```

### Ripple Effect
```css
.ripple {
    position: relative;
    overflow: hidden;
}

.ripple::after {
    content: '';
    position: absolute;
    top: 50%;
    left: 50%;
    width: 0;
    height: 0;
    border-radius: 50%;
    background: rgba(255, 255, 255, 0.3);
    transform: translate(-50%, -50%);
    transition: width 0.6s, height 0.6s;
}

.ripple:active::after {
    width: 300px;
    height: 300px;
}
```

## Accessibility

### Focus States
```css
.btn:focus,
.form-input:focus,
.form-select:focus {
    outline: 2px solid var(--primary);
    outline-offset: 2px;
}
```

### Screen Reader Text
```css
.sr-only {
    position: absolute;
    width: 1px;
    height: 1px;
    padding: 0;
    margin: -1px;
    overflow: hidden;
    clip: rect(0, 0, 0, 0);
    border: 0;
}
```

## Performance

- Hardware-accelerated animations (transform, opacity)
- Minimal repaints with transform
- CSS-only interactions where possible
- Debounced resize handlers

## Browser Support

- Android 7.0+ WebView
- CSS Grid and Flexbox
- CSS Variables
- CSS Animations

## Related Documentation

- [../base/README.md](../base/README.md) - Base styles
- [../layouts/README.md](../layouts/README.md) - Layout structure
- [../../README.md](../../README.md) - Asset overview
