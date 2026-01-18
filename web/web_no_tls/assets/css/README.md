# CSS Assets

## Overview

This directory contains all CSS stylesheets for the Smart Home web dashboard. The styles are organized using a modular architecture that separates base styles, layout components, reusable UI elements, and view-specific styles.

## Structure

```
css/
├── style.css              # Main stylesheet (imports all modules)
├── base/                  # Foundation styles
│   ├── variables.css      # CSS custom properties (colors, spacing, shadows)
│   └── reset.css          # CSS reset and base element styles
├── layouts/               # Structural components
│   ├── sidebar.css        # Navigation sidebar layout
│   ├── header.css         # Page header layout
│   ├── main-content.css   # Main content area layout
│   └── footer.css         # Footer layout
├── components/            # Reusable UI components
│   ├── buttons.css        # Button styles and variants
│   ├── cards.css          # Card component styles
│   ├── badges.css         # Badge and status indicator styles
│   └── modals.css         # Modal dialog styles
└── views/                 # Page-specific styles
    ├── login.css          # Login page styles
    ├── dashboard.css      # Dashboard view styles
    ├── settings.css       # Settings page styles
    └── export.css         # Data export view styles
```

## Architecture

### Base Layer

**variables.css**
- Color palette (primary, success, warning, danger colors)
- Typography system (font sizes, weights, line heights)
- Spacing scale (margins, paddings)
- Border radius values
- Shadow definitions
- Z-index layers
- Sensor-specific colors (temperature, humidity, light)

**reset.css**
- Browser default reset
- Base typography styles
- Common element normalization

### Layout Layer

**sidebar.css**
- Fixed navigation sidebar
- Menu items and active states
- Collapse/expand behavior
- Mobile responsive layout

**header.css**
- Page header with title
- Status badges container
- User info display
- Logout button

**main-content.css**
- Content area layout
- Page sections
- Grid and flex layouts
- Responsive breakpoints

**footer.css**
- Footer layout and styling

### Component Layer

**buttons.css**
- Primary, secondary, danger button variants
- Icon buttons
- Button states (hover, active, disabled)
- Size modifiers

**cards.css**
- Device cards
- Sensor value display
- Control cards with switches
- Card hover effects
- Action buttons

**badges.css**
- Status indicators (success, warning, error)
- Connection status badges
- Info badges

**modals.css**
- Modal dialog container
- Modal header and content
- Form inputs in modals
- Modal overlay

### View Layer

**login.css**
- Authentication page layout
- Login/signup form styles
- Error message display
- Form input styling

**dashboard.css**
- Device grid layout
- Sensor cards
- Real-time data display
- Device control toggles

**settings.css**
- Settings panels
- Configuration forms
- Device info tables
- Action buttons

**export.css**
- Data table styles
- Filter controls
- Export button
- Date range pickers

## Import Order

The main `style.css` file imports all modules in this order:

1. Base styles (variables, reset)
2. Layout components (sidebar, header, main-content, footer)
3. UI components (buttons, cards, badges, modals)
4. View-specific styles (login, dashboard, settings, export)

This order ensures CSS variables are available to all other modules and prevents specificity issues.

## Design System

### Color Scheme
- Primary: Blue (#2563eb) - main actions and branding
- Success: Green (#10b981) - positive states and confirmations
- Warning: Yellow (#f59e0b) - caution states
- Danger: Red (#ef4444) - errors and destructive actions
- Sidebar: Dark slate (#1e293b)

### Responsive Breakpoints
- Desktop: 1024px and above
- Tablet: 768px to 1023px
- Mobile: below 768px

### Typography
- Font family: Inter (sans-serif)
- Base font size: 16px
- Font weights: 400 (regular), 500 (medium), 600 (semibold), 700 (bold)

## Usage

To use these styles in HTML:

```html
<link rel="stylesheet" href="assets/css/style.css" />
```

The main stylesheet automatically imports all necessary modules.

## Maintenance

When adding new styles:
1. Place in appropriate directory (base/layouts/components/views)
2. Import in style.css following the established order
3. Use CSS variables from variables.css for consistency
4. Follow BEM naming convention for component classes
5. Ensure responsive behavior at all breakpoints
