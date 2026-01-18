# Web Assets

## Overview

This directory contains all frontend assets for the Smart Home web dashboard, including stylesheets, JavaScript modules, and related resources. The assets are organized into separate directories for CSS and JavaScript, following a modular architecture for maintainability and scalability.

## Directory Structure

```
assets/
├── css/                   # Stylesheet modules
│   ├── base/              # Foundation styles (variables, reset)
│   ├── layouts/           # Layout components (sidebar, header, main-content)
│   ├── components/        # Reusable UI components (buttons, cards, badges)
│   ├── views/             # Page-specific styles (login, dashboard, settings)
│   └── style.css          # Main stylesheet (imports all modules)
├── js/                    # JavaScript modules
│   ├── core/              # Core functionality (Firebase, auth, login)
│   ├── mqtt/              # MQTT communication (client, handlers, sync)
│   ├── devices/           # Device management (CRUD, rendering)
│   ├── charts/            # Data visualization (Chart.js integration)
│   ├── export/            # Data export functionality
│   ├── settings/          # Settings management
│   ├── ui/                # UI utilities and helpers
│   └── main.js            # Application entry point
├── css/README.md          # CSS documentation
└── js/README.md           # JavaScript documentation
```

## Architecture

### CSS Architecture
The stylesheet system uses a layered approach:

1. **Base Layer** - CSS variables, reset, and foundation styles
2. **Layout Layer** - Structural components (sidebar, header, content areas)
3. **Component Layer** - Reusable UI elements (buttons, cards, badges, modals)
4. **View Layer** - Page-specific styles (login, dashboard, settings, export)

All styles are imported through a single entry point (`style.css`) that maintains proper cascade order.

### JavaScript Architecture
The JavaScript codebase follows ES6 module architecture with clear separation of concerns:

1. **Core Modules** - Firebase configuration, authentication, login controller
2. **MQTT Modules** - MQTT client management, message handling, data synchronization
3. **Device Modules** - Device CRUD operations, UI rendering, state management
4. **Chart Modules** - Data visualization with Chart.js
5. **Export Module** - Historical data retrieval and Excel export
6. **Settings Module** - System configuration management
7. **UI Module** - User interface utilities and interactions

## Key Features

### Real-time Communication
- **MQTT Integration** - WebSocket-based MQTT client for real-time device communication
- **Firebase Sync** - Automatic synchronization of MQTT data to Firebase Realtime Database
- **Live Updates** - Real-time UI updates when device states or sensor values change

### Device Management
- **CRUD Operations** - Create, read, update, and delete devices via Firebase
- **Device Controls** - Toggle switches for relay control
- **Sensor Monitoring** - Real-time temperature, humidity, and light sensor readings
- **Device Information** - View device status, IP address, uptime, and firmware version

### Data Visualization
- **Interactive Charts** - Chart.js powered graphs for sensor data
- **Multi-sensor Support** - Separate charts for temperature, humidity, and light
- **Real-time Updates** - Charts update automatically as new data arrives
- **Historical View** - Display last 10 readings for each sensor

### Data Export
- **History Retrieval** - Fetch all historical sensor data from Firebase
- **Filtering** - Filter by device, date range, and sensor value ranges
- **Quick Filters** - One-click filters for last 7, 30, or 90 days
- **Excel Export** - Export filtered data to Excel format using SheetJS

### User Authentication
- **Firebase Auth** - Email/password authentication
- **Protected Routes** - Automatic redirect to login for unauthenticated users
- **Session Management** - Persistent login sessions
- **Logout Functionality** - Secure session termination

### Configuration Management
- **MQTT Configuration** - Customize broker settings (host, port, credentials)
- **Firebase Configuration** - Switch Firebase projects without code changes
- **Connection Testing** - Test MQTT and Firebase connectivity
- **Local Storage** - Persist user configurations

## Technology Stack

### Frontend Framework
- **HTML5** - Semantic markup with accessibility attributes
- **CSS3** - Modern CSS with custom properties and flexbox/grid layouts
- **ES6 JavaScript** - Modular JavaScript with import/export syntax

### External Libraries
- **Firebase JS SDK v10.7.1** - Backend services (auth, database)
- **Paho MQTT v1.0.1** - MQTT WebSocket client
- **Chart.js v3.9.1** - Data visualization library
- **SheetJS (xlsx)** - Excel file generation
- **Font Awesome 6.4.0** - Icon library
- **Google Fonts (Inter)** - Typography

### Development Tools
- **ES6 Modules** - Native module system (no bundler required)
- **CDN Delivery** - External dependencies loaded from CDN
- **No Build Step** - Direct browser execution without compilation

## File Loading

### CSS Loading
Add to HTML `<head>`:
```html
<link rel="stylesheet" href="assets/css/style.css" />
```

### JavaScript Loading
Add before closing `</body>` tag:

**Main Dashboard:**
```html
<script type="module" src="assets/js/main.js"></script>
```

**Login Page:**
```html
<script type="module" src="assets/js/core/login.js"></script>
```

### External Dependencies
Load from CDN in HTML:
```html
<!-- Firebase -->
<script type="module">
  import { initializeApp } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-app.js";
</script>

<!-- MQTT -->
<script defer src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js"></script>

<!-- Chart.js -->
<script defer src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.9.1/chart.min.js"></script>

<!-- Font Awesome -->
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css" />
```

## Data Flow Overview

### Device Control Flow
```
User Input → UI Event → MQTT Command → ESP Device → State Update → 
MQTT Message → Firebase Sync → UI Update
```

### Sensor Data Flow
```
ESP Sensors → MQTT Publish → Message Handler → Firebase Save → 
Real-time Listener → UI Update + Chart Update
```

### Authentication Flow
```
Login Form → Firebase Auth → Session Created → Dashboard Load → 
Module Initialization → MQTT Connect
```

## Configuration

### MQTT Settings
Configurable via Settings page:
- Broker host and port
- WebSocket path
- SSL/TLS toggle
- Authentication credentials
- Keepalive interval

### Firebase Settings
Configurable via Settings page:
- API key
- Auth domain
- Database URL
- Project ID
- Storage bucket
- Messaging sender ID
- App ID
- Measurement ID

## Browser Compatibility

### Minimum Requirements
- ES6 module support
- CSS custom properties (CSS variables)
- Fetch API
- Local Storage
- WebSocket support

### Tested Browsers
- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

## Performance Considerations

### Optimizations
- **Lazy Loading** - Charts loaded only when needed
- **State Caching** - Device states cached to reduce Firebase reads
- **Listener Cleanup** - Firebase listeners removed on tab switch
- **Data Limiting** - Chart data limited to last 10 records
- **Debouncing** - User inputs debounced to prevent excessive updates

### Resource Management
- Chart instances properly destroyed when not in use
- Firebase listeners unsubscribed when components unmount
- MQTT reconnection with exponential backoff
- Cached device data to minimize network requests

## Security

### Best Practices
- Firebase security rules enforce authentication
- MQTT credentials stored in localStorage (consider encryption)
- No sensitive data in client-side code
- HTTPS recommended for production deployment
- Content Security Policy headers recommended

### Authentication
- Protected route mechanism prevents unauthorized access
- Session tokens managed by Firebase Auth
- Automatic redirect to login for expired sessions
- Logout clears all session data

## Maintenance

### Adding New Features

**CSS Module:**
1. Create new file in appropriate directory (base/layouts/components/views)
2. Import in `style.css`
3. Use CSS variables for consistency
4. Document in [css/README.md](css/README.md)

**JavaScript Module:**
1. Create new file in appropriate directory
2. Export functions explicitly
3. Import in `main.js` if needed at startup
4. Follow error handling patterns
5. Document in [js/README.md](js/README.md)

### Code Organization
- Keep modules focused on single responsibility
- Use meaningful file and function names
- Add JSDoc comments for public functions
- Follow established naming conventions
- Maintain consistent code style

## Detailed Documentation

For in-depth documentation on specific asset types, refer to:

- **[CSS Documentation](css/README.md)** - Complete stylesheet architecture, design system, and component documentation
- **[JavaScript Documentation](js/README.md)** - Module-by-module functionality, data flow, and API documentation

## Support

For issues or questions:
1. Check individual module documentation
2. Review console logs for errors
3. Verify configuration settings
4. Test MQTT and Firebase connectivity
5. Ensure all dependencies are loaded correctly
