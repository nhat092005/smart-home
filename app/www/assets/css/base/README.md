# Base Styles

## Overview

Foundation CSS module containing global variables, reset styles, and typography definitions for the mobile application.

## Files

- Variables and theme colors
- CSS reset and normalization
- Typography and font definitions
- Global utility classes

## CSS Variables

### Color Palette
```css
:root {
    /* Primary Colors */
    --primary: #2563eb;
    --primary-dark: #1e40af;
    --primary-light: #3b82f6;
    
    /* Background */
    --bg-primary: #0f172a;
    --bg-secondary: #1e293b;
    --bg-tertiary: #334155;
    
    /* Text */
    --text-primary: #f1f5f9;
    --text-secondary: #cbd5e1;
    --text-muted: #94a3b8;
    
    /* Status */
    --success: #10b981;
    --warning: #f59e0b;
    --error: #ef4444;
    --info: #3b82f6;
    
    /* Borders */
    --border-color: #334155;
    --border-radius: 8px;
    
    /* Shadows */
    --shadow-sm: 0 1px 2px rgba(0, 0, 0, 0.05);
    --shadow-md: 0 4px 6px rgba(0, 0, 0, 0.1);
    --shadow-lg: 0 10px 15px rgba(0, 0, 0, 0.2);
}
```

### Spacing Scale
```css
:root {
    --spacing-xs: 4px;
    --spacing-sm: 8px;
    --spacing-md: 16px;
    --spacing-lg: 24px;
    --spacing-xl: 32px;
}
```

### Typography Scale
```css
:root {
    --font-family: 'Inter', -apple-system, sans-serif;
    
    --font-size-xs: 12px;
    --font-size-sm: 14px;
    --font-size-base: 16px;
    --font-size-lg: 18px;
    --font-size-xl: 24px;
    
    --font-weight-normal: 400;
    --font-weight-medium: 500;
    --font-weight-semibold: 600;
    --font-weight-bold: 700;
}
```

## Reset Styles

### Box Sizing
```css
*,
*::before,
*::after {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
}
```

### Body Defaults
```css
body {
    font-family: var(--font-family);
    font-size: var(--font-size-base);
    line-height: 1.5;
    color: var(--text-primary);
    background: var(--bg-primary);
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
}
```

### List Reset
```css
ul,
ol {
    list-style: none;
}
```

### Link Reset
```css
a {
    color: inherit;
    text-decoration: none;
}
```

## Typography

### Headings
```css
h1 {
    font-size: var(--font-size-xl);
    font-weight: var(--font-weight-bold);
    line-height: 1.2;
}

h2 {
    font-size: var(--font-size-lg);
    font-weight: var(--font-weight-semibold);
    line-height: 1.3;
}

h3 {
    font-size: var(--font-size-base);
    font-weight: var(--font-weight-semibold);
    line-height: 1.4;
}
```

### Paragraph
```css
p {
    font-size: var(--font-size-base);
    font-weight: var(--font-weight-normal);
    line-height: 1.6;
    color: var(--text-secondary);
}
```

### Small Text
```css
small {
    font-size: var(--font-size-sm);
    color: var(--text-muted);
}
```

## Utility Classes

### Text Colors
```css
.text-primary { color: var(--text-primary); }
.text-secondary { color: var(--text-secondary); }
.text-muted { color: var(--text-muted); }
.text-success { color: var(--success); }
.text-warning { color: var(--warning); }
.text-error { color: var(--error); }
```

### Background Colors
```css
.bg-primary { background: var(--bg-primary); }
.bg-secondary { background: var(--bg-secondary); }
.bg-tertiary { background: var(--bg-tertiary); }
```

### Spacing Utilities
```css
.m-0 { margin: 0; }
.mt-sm { margin-top: var(--spacing-sm); }
.mt-md { margin-top: var(--spacing-md); }
.mt-lg { margin-top: var(--spacing-lg); }

.p-0 { padding: 0; }
.p-sm { padding: var(--spacing-sm); }
.p-md { padding: var(--spacing-md); }
.p-lg { padding: var(--spacing-lg); }
```

### Display Utilities
```css
.d-none { display: none; }
.d-block { display: block; }
.d-flex { display: flex; }
.d-grid { display: grid; }
```

### Flexbox Utilities
```css
.flex-row { flex-direction: row; }
.flex-column { flex-direction: column; }
.justify-center { justify-content: center; }
.justify-between { justify-content: space-between; }
.align-center { align-items: center; }
.gap-sm { gap: var(--spacing-sm); }
.gap-md { gap: var(--spacing-md); }
```

## Mobile Optimizations

### Touch-Friendly Sizing
```css
:root {
    --touch-target-min: 44px;
}
```

### Safe Area Insets
```css
:root {
    --safe-area-top: env(safe-area-inset-top);
    --safe-area-bottom: env(safe-area-inset-bottom);
    --safe-area-left: env(safe-area-inset-left);
    --safe-area-right: env(safe-area-inset-right);
}
```

### Viewport Units
```css
:root {
    --vh: 1vh;
    --vw: 1vw;
}
```

## Dark Theme

All styles use dark theme by default:
- Dark backgrounds (#0f172a, #1e293b)
- Light text (#f1f5f9, #cbd5e1)
- Blue accent (#2563eb)

## Font Loading

### Google Fonts
```html
<link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap" rel="stylesheet">
```

### Font Display
```css
@font-face {
    font-family: 'Inter';
    font-display: swap;
}
```

## Browser Support

- Android 7.0+ WebView
- iOS Safari (if ported)
- Modern CSS features (CSS Variables, Flexbox, Grid)

## Performance

- CSS Variables for theme consistency
- Minimal resets to reduce overhead
- System font stack fallbacks
- Font display swap for faster loading

## Related Documentation

- [../components/README.md](components/README.md) - Component styles
- [../layouts/README.md](layouts/README.md) - Layout styles
- [../../README.md](../../README.md) - Asset overview
