# View Styles

## Overview

Page-specific styles for login, dashboard, and settings views in the mobile application. Each view has unique layout requirements and component styling.

## Views

- Login page
- Dashboard (home tab)
- Report view (dashboard tab)
- Settings view

## Login View

### Login Container
```css
.login-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    min-height: 100vh;
    padding: var(--spacing-lg);
    background: linear-gradient(135deg, var(--bg-primary) 0%, var(--bg-secondary) 100%);
}

.login-logo {
    width: 120px;
    height: 120px;
    margin-bottom: var(--spacing-xl);
}

.login-title {
    font-size: var(--font-size-xl);
    font-weight: var(--font-weight-bold);
    color: var(--text-primary);
    margin-bottom: var(--spacing-sm);
    text-align: center;
}

.login-subtitle {
    font-size: var(--font-size-base);
    color: var(--text-muted);
    margin-bottom: var(--spacing-xl);
    text-align: center;
}
```

### Login Form
```css
.login-form {
    width: 100%;
    max-width: 400px;
    background: var(--bg-secondary);
    padding: var(--spacing-lg);
    border-radius: var(--border-radius);
    box-shadow: var(--shadow-lg);
}

.login-form .form-group {
    margin-bottom: var(--spacing-md);
}

.login-form .btn {
    width: 100%;
    margin-top: var(--spacing-md);
}

.login-divider {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    margin: var(--spacing-lg) 0;
    color: var(--text-muted);
}

.login-divider::before,
.login-divider::after {
    content: '';
    flex: 1;
    height: 1px;
    background: var(--border-color);
}
```

### Login Error
```css
.login-error {
    background: rgba(239, 68, 68, 0.1);
    border: 1px solid var(--error);
    border-radius: var(--border-radius);
    padding: var(--spacing-sm);
    margin-bottom: var(--spacing-md);
    color: var(--error);
    font-size: var(--font-size-sm);
}
```

## Dashboard View

### Dashboard Header
```css
.dashboard-header {
    background: var(--bg-secondary);
    padding: var(--spacing-md);
    padding-top: calc(var(--spacing-md) + var(--safe-area-top));
    margin-bottom: var(--spacing-md);
}

.dashboard-greeting {
    font-size: var(--font-size-lg);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
    margin-bottom: var(--spacing-xs);
}

.dashboard-date {
    font-size: var(--font-size-sm);
    color: var(--text-muted);
}
```

### Device Grid
```css
.dashboard-devices {
    padding: 0 var(--spacing-md);
    padding-bottom: calc(var(--bottom-nav-height) + var(--spacing-md));
}

.device-card {
    background: var(--bg-secondary);
    border-radius: var(--border-radius);
    padding: var(--spacing-md);
    box-shadow: var(--shadow-md);
    cursor: pointer;
    transition: transform 0.2s;
}

.device-card:active {
    transform: scale(0.98);
}

.device-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: var(--spacing-md);
}

.device-name {
    font-size: var(--font-size-base);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
}

.device-sensors {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: var(--spacing-sm);
}

.sensor-item {
    text-align: center;
}

.sensor-icon {
    font-size: 20px;
    margin-bottom: var(--spacing-xs);
}

.sensor-value {
    font-size: var(--font-size-base);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
}

.sensor-label {
    font-size: var(--font-size-xs);
    color: var(--text-muted);
}
```

## Report View (Modal)

### Report Modal
```css
.report-modal {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: var(--bg-primary);
    z-index: 1000;
    overflow-y: auto;
    animation: slideUp 0.3s ease;
}

@keyframes slideUp {
    from {
        transform: translateY(100%);
    }
    to {
        transform: translateY(0);
    }
}

.report-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: var(--spacing-md);
    padding-top: calc(var(--spacing-md) + var(--safe-area-top));
    background: var(--bg-secondary);
    position: sticky;
    top: 0;
    z-index: 10;
}

.report-close {
    width: 32px;
    height: 32px;
    border-radius: 50%;
    background: var(--bg-tertiary);
    border: none;
    color: var(--text-primary);
    font-size: 20px;
}
```

### Report Content
```css
.report-content {
    padding: var(--spacing-md);
    padding-bottom: calc(var(--spacing-md) + var(--safe-area-bottom));
}

.report-stats {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: var(--spacing-sm);
    margin-bottom: var(--spacing-lg);
}

.stat-card {
    background: var(--bg-secondary);
    padding: var(--spacing-md);
    border-radius: var(--border-radius);
    text-align: center;
}

.stat-icon {
    font-size: 32px;
    margin-bottom: var(--spacing-sm);
}

.stat-value {
    font-size: var(--font-size-xl);
    font-weight: var(--font-weight-bold);
    color: var(--text-primary);
}

.stat-label {
    font-size: var(--font-size-xs);
    color: var(--text-muted);
}
```

### Report Controls
```css
.report-controls {
    background: var(--bg-secondary);
    padding: var(--spacing-md);
    border-radius: var(--border-radius);
    margin-bottom: var(--spacing-lg);
}

.control-row {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: var(--spacing-sm) 0;
}

.control-label {
    font-weight: var(--font-weight-medium);
    color: var(--text-primary);
}

.control-toggle {
    width: 60px;
    height: 32px;
    border-radius: 16px;
    background: var(--bg-tertiary);
    position: relative;
    cursor: pointer;
    transition: background 0.3s;
}

.control-toggle.active {
    background: var(--success);
}

.control-toggle::before {
    content: '';
    position: absolute;
    width: 28px;
    height: 28px;
    border-radius: 50%;
    background: white;
    top: 2px;
    left: 2px;
    transition: transform 0.3s;
}

.control-toggle.active::before {
    transform: translateX(28px);
}
```

### Report Chart
```css
.report-chart {
    background: var(--bg-secondary);
    padding: var(--spacing-md);
    border-radius: var(--border-radius);
}

.chart-buttons {
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
    transition: all 0.3s;
}

.chart-btn.active {
    background: var(--primary);
    color: white;
}
```

## Settings View

### Settings Container
```css
.settings-container {
    padding: var(--spacing-md);
    padding-top: calc(var(--spacing-md) + var(--safe-area-top));
    padding-bottom: calc(var(--bottom-nav-height) + var(--spacing-md));
}

.settings-section {
    margin-bottom: var(--spacing-lg);
}

.settings-section-title {
    font-size: var(--font-size-lg);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
    margin-bottom: var(--spacing-md);
}
```

### System Clock
```css
.system-clock {
    background: var(--bg-secondary);
    padding: var(--spacing-lg);
    border-radius: var(--border-radius);
    text-align: center;
    margin-bottom: var(--spacing-md);
}

.clock-time {
    font-size: 48px;
    font-weight: var(--font-weight-bold);
    color: var(--primary);
    font-variant-numeric: tabular-nums;
}

.clock-date {
    font-size: var(--font-size-base);
    color: var(--text-muted);
    margin-top: var(--spacing-sm);
}
```

### Settings Table
```css
.settings-table {
    background: var(--bg-secondary);
    border-radius: var(--border-radius);
    overflow: hidden;
}

.settings-table table {
    width: 100%;
    border-collapse: collapse;
}

.settings-table th {
    background: var(--bg-tertiary);
    padding: var(--spacing-sm);
    text-align: left;
    font-weight: var(--font-weight-semibold);
    color: var(--text-secondary);
    font-size: var(--font-size-sm);
}

.settings-table td {
    padding: var(--spacing-sm);
    border-bottom: 1px solid var(--border-color);
    color: var(--text-primary);
}

.settings-table tr:last-child td {
    border-bottom: none;
}
```

### Settings Actions
```css
.settings-actions {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-sm);
}

.action-btn {
    width: 100%;
    padding: var(--spacing-md);
    border-radius: var(--border-radius);
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    color: var(--text-primary);
    font-size: var(--font-size-base);
    font-weight: var(--font-weight-medium);
    display: flex;
    align-items: center;
    justify-content: space-between;
    min-height: var(--touch-target-min);
}

.action-btn .icon {
    font-size: 20px;
    color: var(--primary);
}

.action-btn.danger {
    border-color: var(--error);
    color: var(--error);
}

.action-btn.danger .icon {
    color: var(--error);
}
```

### WiFi Guide
```css
.wifi-guide {
    background: var(--bg-secondary);
    padding: var(--spacing-md);
    border-radius: var(--border-radius);
}

.wifi-guide .step {
    margin-bottom: var(--spacing-md);
}

.wifi-guide .step-number {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    width: 24px;
    height: 24px;
    border-radius: 50%;
    background: var(--primary);
    color: white;
    font-size: var(--font-size-sm);
    font-weight: var(--font-weight-bold);
    margin-right: var(--spacing-sm);
}

.wifi-guide .step-text {
    color: var(--text-secondary);
    line-height: 1.6;
}

.wifi-guide .credential {
    background: var(--bg-tertiary);
    padding: var(--spacing-sm);
    border-radius: 4px;
    margin: var(--spacing-sm) 0;
    font-family: monospace;
    color: var(--primary);
}
```

## Animations

### Page Transitions
```css
.view-enter {
    animation: slideIn 0.3s ease;
}

@keyframes slideIn {
    from {
        opacity: 0;
        transform: translateX(20px);
    }
    to {
        opacity: 1;
        transform: translateX(0);
    }
}
```

### Loading State
```css
.view-loading {
    display: flex;
    align-items: center;
    justify-content: center;
    min-height: 100vh;
}
```

## Performance

- Optimized animations for 60fps
- Hardware acceleration
- Minimal repaints
- Efficient grid layouts

## Accessibility

- Semantic HTML structure
- Proper heading hierarchy
- Touch-friendly controls
- Sufficient color contrast

## Browser Support

- Android 7.0+ WebView
- CSS Grid and Flexbox
- CSS Animations
- Safe area insets

## Related Documentation

- [../base/README.md](../base/README.md) - Base styles
- [../components/README.md](../components/README.md) - Component styles
- [../layouts/README.md](../layouts/README.md) - Layout styles
- [../../README.md](../../README.md) - Asset overview
