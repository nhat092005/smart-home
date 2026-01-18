# Layout Styles

## Overview

Core layout structure styles for the mobile application including bottom navigation, main content area, grid systems, and responsive containers.

## Layout Components

- Bottom navigation bar
- Main content container
- Grid system
- Safe area handling
- Responsive utilities

## Bottom Navigation

### Navigation Bar
```css
.bottom-nav {
    position: fixed;
    bottom: 0;
    left: 0;
    right: 0;
    height: var(--bottom-nav-height);
    background: var(--bg-secondary);
    border-top: 1px solid var(--border-color);
    display: flex;
    justify-content: space-around;
    align-items: center;
    padding-bottom: var(--safe-area-bottom);
    z-index: 100;
}

:root {
    --bottom-nav-height: 64px;
}
```

### Navigation Items
```css
.nav-item {
    flex: 1;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 4px;
    padding: 8px;
    color: var(--text-muted);
    transition: color 0.3s;
    min-height: var(--touch-target-min);
}

.nav-item.active {
    color: var(--primary);
}

.nav-item .icon {
    font-size: 24px;
}

.nav-item .label {
    font-size: var(--font-size-xs);
    font-weight: var(--font-weight-medium);
}
```

## Main Content

### Content Container
```css
.main-content {
    padding: var(--spacing-md);
    padding-top: calc(var(--spacing-md) + var(--safe-area-top));
    padding-bottom: calc(var(--bottom-nav-height) + var(--spacing-md));
    min-height: 100vh;
}
```

### Section Container
```css
.section {
    margin-bottom: var(--spacing-lg);
}

.section-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: var(--spacing-md);
}

.section-title {
    font-size: var(--font-size-lg);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
}
```

## Grid System

### Device Grid
```css
.device-grid {
    display: grid;
    grid-template-columns: 1fr;
    gap: var(--spacing-md);
}

@media (min-width: 640px) {
    .device-grid {
        grid-template-columns: repeat(2, 1fr);
    }
}

@media (min-width: 1024px) {
    .device-grid {
        grid-template-columns: repeat(3, 1fr);
    }
}
```

### Sensor Grid
```css
.sensor-grid {
    display: grid;
    grid-template-columns: 1fr;
    gap: var(--spacing-sm);
}

@media (min-width: 640px) {
    .sensor-grid {
        grid-template-columns: repeat(2, 1fr);
    }
}
```

### Control Grid
```css
.control-grid {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: var(--spacing-sm);
}
```

## Container

### Page Container
```css
.container {
    width: 100%;
    max-width: 1200px;
    margin: 0 auto;
    padding: 0 var(--spacing-md);
}
```

### Fluid Container
```css
.container-fluid {
    width: 100%;
    padding: 0 var(--spacing-md);
}
```

## Safe Area Support

### iOS Safe Area
```css
.safe-area-top {
    padding-top: var(--safe-area-top);
}

.safe-area-bottom {
    padding-bottom: var(--safe-area-bottom);
}

.safe-area-left {
    padding-left: var(--safe-area-left);
}

.safe-area-right {
    padding-right: var(--safe-area-right);
}
```

### Notch Support
```css
@supports (padding: env(safe-area-inset-top)) {
    .bottom-nav {
        padding-bottom: max(var(--safe-area-bottom), 8px);
    }
    
    .main-content {
        padding-top: max(var(--safe-area-top), 16px);
    }
}
```

## Tab Views

### Tab Container
```css
.tabs {
    display: none;
}

.tabs.active {
    display: block;
    animation: fadeIn 0.3s ease;
}

@keyframes fadeIn {
    from {
        opacity: 0;
        transform: translateY(10px);
    }
    to {
        opacity: 1;
        transform: translateY(0);
    }
}
```

### Tab Content
```css
.tab-content {
    padding: var(--spacing-md) 0;
}
```

## Modal Layout

### Full Screen Modal
```css
.modal-fullscreen {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: var(--bg-primary);
    z-index: 1000;
    overflow-y: auto;
}

.modal-fullscreen .modal-content {
    padding: var(--spacing-md);
    padding-top: calc(var(--spacing-md) + var(--safe-area-top));
    padding-bottom: calc(var(--spacing-md) + var(--safe-area-bottom));
}
```

## Header Layout

### Page Header
```css
.page-header {
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

.page-title {
    font-size: var(--font-size-xl);
    font-weight: var(--font-weight-bold);
}
```

## List Layout

### List Container
```css
.list {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-sm);
}

.list-item {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
    padding: var(--spacing-md);
    background: var(--bg-secondary);
    border-radius: var(--border-radius);
}

.list-item .icon {
    font-size: 24px;
    color: var(--primary);
}

.list-item .content {
    flex: 1;
}

.list-item .title {
    font-weight: var(--font-weight-medium);
    color: var(--text-primary);
}

.list-item .subtitle {
    font-size: var(--font-size-sm);
    color: var(--text-muted);
}
```

## Empty States

### Empty State Container
```css
.empty-state {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    padding: var(--spacing-xl);
    text-align: center;
    min-height: 300px;
}

.empty-state .icon {
    font-size: 64px;
    color: var(--text-muted);
    margin-bottom: var(--spacing-md);
}

.empty-state .title {
    font-size: var(--font-size-lg);
    font-weight: var(--font-weight-semibold);
    color: var(--text-primary);
    margin-bottom: var(--spacing-sm);
}

.empty-state .description {
    font-size: var(--font-size-base);
    color: var(--text-muted);
}
```

## Divider

### Horizontal Divider
```css
.divider {
    height: 1px;
    background: var(--border-color);
    margin: var(--spacing-md) 0;
}
```

### Vertical Divider
```css
.divider-vertical {
    width: 1px;
    background: var(--border-color);
    margin: 0 var(--spacing-md);
}
```

## Spacing System

### Margin Utilities
```css
.m-0 { margin: 0; }
.mt-sm { margin-top: var(--spacing-sm); }
.mt-md { margin-top: var(--spacing-md); }
.mt-lg { margin-top: var(--spacing-lg); }
.mb-sm { margin-bottom: var(--spacing-sm); }
.mb-md { margin-bottom: var(--spacing-md); }
.mb-lg { margin-bottom: var(--spacing-lg); }
```

### Padding Utilities
```css
.p-0 { padding: 0; }
.p-sm { padding: var(--spacing-sm); }
.p-md { padding: var(--spacing-md); }
.p-lg { padding: var(--spacing-lg); }
.px-md { padding-left: var(--spacing-md); padding-right: var(--spacing-md); }
.py-md { padding-top: var(--spacing-md); padding-bottom: var(--spacing-md); }
```

## Responsive Utilities

### Breakpoints
```css
:root {
    --breakpoint-sm: 640px;
    --breakpoint-md: 768px;
    --breakpoint-lg: 1024px;
    --breakpoint-xl: 1280px;
}
```

### Visibility Classes
```css
.hide-mobile {
    display: none;
}

@media (min-width: 640px) {
    .hide-mobile {
        display: block;
    }
    
    .hide-desktop {
        display: none;
    }
}
```

### Responsive Text
```css
@media (max-width: 639px) {
    .text-responsive {
        font-size: var(--font-size-sm);
    }
}

@media (min-width: 640px) {
    .text-responsive {
        font-size: var(--font-size-base);
    }
}
```

## Scroll Behavior

### Smooth Scrolling
```css
html {
    scroll-behavior: smooth;
}
```

### Hide Scrollbar
```css
.hide-scrollbar {
    scrollbar-width: none;
    -ms-overflow-style: none;
}

.hide-scrollbar::-webkit-scrollbar {
    display: none;
}
```

## Z-Index Scale

```css
:root {
    --z-index-dropdown: 100;
    --z-index-sticky: 200;
    --z-index-fixed: 300;
    --z-index-modal-backdrop: 400;
    --z-index-modal: 500;
    --z-index-tooltip: 600;
}
```

## Performance

- Hardware acceleration for animations
- Minimal layout shifts
- Optimized for 60fps scrolling
- Touch-optimized tap targets

## Accessibility

- Minimum 44px tap targets
- Proper heading hierarchy
- Sufficient color contrast
- Focus states visible

## Browser Support

- Android 7.0+ WebView
- CSS Grid and Flexbox
- Safe area insets
- Custom properties

## Related Documentation

- [../base/README.md](../base/README.md) - Base styles
- [../components/README.md](../components/README.md) - Component styles
- [../../README.md](../../README.md) - Asset overview
