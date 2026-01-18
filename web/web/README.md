# Smart Home Web Dashboard

## Overview

Real-time web-based monitoring and control system for Smart Home IoT devices. Implements bidirectional communication using MQTT over WebSocket for device control and Firebase Realtime Database for data persistence. Supports multiple ESP32 devices with environmental sensors and relay controls.

## Features

- Real-time device control via MQTT WebSocket
- Live sensor data visualization (temperature, humidity, light)
- Interactive charts with Chart.js
- Historical data export to Excel
- Device management (add/edit/delete)
- MQTT and Firebase configuration
- User authentication with Firebase Auth
- Responsive single-page application

## Technology Stack

- HTML5 with semantic markup
- CSS3 with modern features (Grid, Flexbox, Custom Properties)
- JavaScript ES6 modules
- Chart.js for data visualization
- Firebase Realtime Database and Authentication
- MQTT.js for WebSocket communication
- SheetJS for Excel export

## Architecture

### Module Organization

```
assets/
├── css/
│   ├── base/              # Foundation (reset, variables)
│   ├── layouts/           # Page structure (header, sidebar, main)
│   ├── components/        # Reusable UI (buttons, cards, badges)
│   └── views/             # Page-specific styles
├── js/
│   ├── core/              # Firebase, authentication
│   ├── mqtt/              # MQTT client, handlers, sync
│   ├── devices/           # Device management
│   ├── charts/            # Data visualization
│   ├── export/            # Data export functionality
│   ├── settings/          # Configuration management
│   ├── ui/                # UI utilities
│   └── utils/             # Helper functions
└── main.js                # Application entry point
```

### Data Flow

```
User Action → UI Event → MQTT Command → ESP32 Device
                                         ↓
Firebase ← MQTT Handler ← Device Response
   ↓
UI Update
```

## Configuration

### Firebase Setup

1. Create Firebase project at https://console.firebase.google.com
2. Enable Realtime Database
3. Enable Email/Password authentication
4. Update `firebase-config.js` with your credentials:

```javascript
const firebaseConfig = {
    apiKey: "YOUR_API_KEY",
    authDomain: "YOUR_AUTH_DOMAIN",
    databaseURL: "YOUR_DATABASE_URL",
    projectId: "YOUR_PROJECT_ID",
    storageBucket: "YOUR_STORAGE_BUCKET",
    messagingSenderId: "YOUR_SENDER_ID",
    appId: "YOUR_APP_ID"
};
```

### MQTT Broker Setup

Default configuration:
- Host: test.mosquitto.org
- Port: 8081 (WebSocket with SSL)
- Path: /mqtt
- Username/Password: Optional

Update in Settings page or modify `mqtt-client.js`.

## Installation

### Local Development

```bash
# Clone repository
git clone <repository-url>
cd web/web

# Serve with any HTTP server
python -m http.server 8000
# or
npx http-server

# Open browser
open http://localhost:8000
```

### Firebase Hosting

```bash
# Install Firebase CLI
npm install -g firebase-tools

# Login
firebase login

# Initialize (if not done)
firebase init hosting

# Deploy
firebase deploy
```

## Usage

### First Time Setup

1. Open application in browser
2. Register new account
3. Configure MQTT broker in Settings
4. Add devices with unique IDs
5. Verify device connection

### Device Control

- Toggle relays: Click device card switches
- View sensor data: Monitor dashboard cards
- Check device status: View connection indicators

### Data Visualization

- Select chart type: Temperature, Humidity, or Light
- Charts update automatically with new readings
- View last 10 readings per sensor

### Data Export

1. Navigate to Export tab
2. Select device and date range
3. Apply filters (optional)
4. Click "Export to Excel"

## Firebase Database Structure

```json
{
  "devices": {
    "<deviceId>": {
      "name": "Living Room",
      "id": "device001",
      "online": true,
      "light": 450,
      "temp": 25.5,
      "hum": 65.2,
      "uptime": 86400,
      "ip": "192.168.1.100",
      "timestamp": "2024-01-18T10:30:00"
    }
  },
  "history": {
    "<deviceId>": {
      "<timestamp>": {
        "temp": 25.5,
        "hum": 65.2,
        "light": 450,
        "datetime": "2024-01-18 10:30:00"
      }
    }
  }
}
```

## MQTT Topics

### Subscribe (from ESP32)
```
SmartHome/<deviceId>/data      # Sensor data
SmartHome/<deviceId>/state     # Device state
SmartHome/<deviceId>/info      # Device info
```

### Publish (to ESP32)
```
device/mode                    # ON/OFF mode
device/light                   # Light relay
device/fan                     # Fan relay
device/ac                      # AC relay
device/timestamp               # Time sync
device/reboot                  # Reboot command
```

## Security Considerations

- Firebase Authentication required for all pages
- MQTT credentials configurable in Settings
- WebSocket over SSL (wss://) for secure MQTT
- Firebase HTTPS for database access
- No sensitive data in client-side code

## Browser Compatibility

- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

Requires ES6 module support and modern CSS features.

## Troubleshooting

### MQTT Connection Fails
- Verify broker URL and port
- Check WebSocket support (wss://)
- Confirm firewall allows port 8081
- Test with public broker first

### Firebase Connection Issues
- Verify configuration in firebase-config.js
- Check authentication is enabled
- Confirm database rules allow read/write

### Charts Not Updating
- Check Firebase data structure
- Verify device is publishing data
- Inspect browser console for errors

## Performance

- Initial load: ~500ms
- MQTT latency: 50-200ms
- Firebase sync: 100-300ms
- Chart updates: 60 FPS
- Memory usage: ~50MB

## Related Documentation

- [assets/README.md](assets/README.md) - Asset organization
- [assets/css/README.md](assets/css/README.md) - CSS architecture
- [assets/js/README.md](assets/js/README.md) - JavaScript modules
- [../web_no_tls/README.md](../web_no_tls/README.md) - Non-TLS variant

## License

MIT License

### Backend Services
- **Firebase Realtime Database** - NoSQL database for device data and history
- **Firebase Authentication** - User authentication and session management
- **MQTT Broker** - Real-time message broker for device communication

### External Libraries
- **Firebase JS SDK v10.7.1** - Backend integration
- **Paho MQTT v1.0.1** - MQTT WebSocket client
- **Chart.js v3.9.1** - Data visualization
- **SheetJS (xlsx)** - Excel file generation
- **Font Awesome 6.4.0** - Icon library
- **Google Fonts (Inter)** - Typography

## File Structure

```
web/
├── index.html             # Main dashboard page
├── login.html             # Authentication page
├── assets/                # Static assets
│   ├── css/               # Stylesheets
│   │   ├── base/          # Foundation styles (variables, reset)
│   │   ├── layouts/       # Layout components (sidebar, header, main)
│   │   ├── components/    # UI components (buttons, cards, badges)
│   │   ├── views/         # Page-specific styles
│   │   └── style.css      # Main stylesheet
│   └── js/                # JavaScript modules
│       ├── core/          # Core functionality (Firebase, auth)
│       ├── mqtt/          # MQTT communication
│       ├── devices/       # Device management
│       ├── charts/        # Data visualization
│       ├── export/        # Data export
│       ├── settings/      # Settings management
│       ├── ui/            # UI utilities
│       └── main.js        # Application entry point
└── README.md              # This file
```

## Pages

### index.html - Main Dashboard
The primary application interface with four main sections:

**Home Tab**
- Device grid with real-time sensor data
- Device control toggles for relay states
- Quick device actions (edit, delete)
- Add new device functionality
- Device state indicators

**Dashboard Tab**
- Interactive sensor charts
- Chart type selector (temperature/humidity/light)
- Real-time data streaming
- Historical trend visualization
- Device selector for chart data

**Data Tab**
- Historical data table
- Multi-criteria filters (device, date, sensor values)
- Quick time range filters (7/30/90 days)
- Export to Excel functionality
- Data pagination

**Settings Tab**
- MQTT broker configuration
- Firebase project configuration
- Connection testing
- Device information table
- Time synchronization
- Device reboot controls

### login.html - Authentication Page
Simple authentication interface:
- Email/password login form
- User registration toggle
- Error message display
- Redirect to dashboard on success
- Responsive design

## Architecture

### Application Flow

```
Entry Point (index.html/login.html)
    ↓
JavaScript Modules Load (main.js/login.js)
    ↓
Authentication Check
    ↓
Initialize Firebase
    ↓
Initialize MQTT Client
    ↓
Initialize Device Manager
    ↓
Render UI Components
    ↓
Start Real-time Listeners
```

### Data Flow

**Device Control:**
```
User Toggle → UI Event → sendMQTTCommand() → MQTT Broker → ESP Device
                                                              ↓
UI Update ← Firebase Listener ← Firebase Sync ← MQTT Message ← Device State
```

**Sensor Data:**
```
ESP Sensors → MQTT Publish → handleMQTTMessage() → saveSensorDataToFirebase()
                                                              ↓
                                                    Firebase Database
                                                              ↓
                                    onValue Listener → Update UI + Charts
```

### Module Organization

**Core Modules**
- Authentication and user management
- Firebase initialization and configuration
- Login page controller

**MQTT Modules**
- Client connection and management
- Message handling and routing
- Firebase synchronization

**Device Modules**
- CRUD operations
- State management
- UI rendering

**Chart Modules**
- Chart.js integration
- Data fetching and formatting
- Chart lifecycle management

**Export Module**
- Historical data retrieval
- Filtering and sorting
- Excel generation

**Settings Module**
- Configuration management
- Connection testing
- Device administration

**UI Module**
- Helper functions
- Modal management
- Navigation

## Database Structure

### Firebase Realtime Database Schema

```
{
  "devices": {
    "device001": {
      "name": "Living Room",
      "createdAt": 1234567890000,
      "sensors": {
        "temperature": 25.5,
        "humidity": 60.2,
        "light": 450
      },
      "state": {
        "relay1": true,
        "relay2": false
      },
      "info": {
        "ip": "192.168.1.100",
        "uptime": 3600,
        "version": "1.0.0"
      }
    }
  },
  "history": {
    "device001": {
      "records": {
        "1234567890000": {
          "temperature": 25.5,
          "humidity": 60.2,
          "light": 450,
          "timestamp": 1234567890000
        }
      }
    }
  }
}
```

### Data Paths
- `/devices/{deviceId}` - Device information and current state
- `/devices/{deviceId}/sensors` - Current sensor readings
- `/devices/{deviceId}/state` - Current relay states
- `/devices/{deviceId}/info` - Device metadata
- `/history/{deviceId}/records` - Historical sensor data

## MQTT Topics

### Topic Structure
```
{topic}/{deviceId}
```

### Topic Types
- **data/{deviceId}** - Sensor data from device (publish from ESP)
- **state/{deviceId}** - Device state updates (publish from ESP)
- **info/{deviceId}** - Device information (publish from ESP)
- **command/{deviceId}** - Commands to device (publish from web)

### Message Format

**Sensor Data (data/device001):**
```json
{
  "temperature": 25.5,
  "humidity": 60.2,
  "light": 450
}
```

**State Update (state/device001):**
```json
{
  "relay1": true,
  "relay2": false
}
```

**Command (command/device001):**
```json
{
  "cmd_id": 123,
  "relay1": 1
}
```

## Configuration

### MQTT Configuration
Configure in Settings page or localStorage key `mqtt_config`:
```json
{
  "host": "broker.emqx.io",
  "port": 8083,
  "path": "/mqtt",
  "useSSL": false,
  "username": "",
  "password": "",
  "keepalive": 60
}
```

### Firebase Configuration
Configure in Settings page or localStorage key `user_firebase_config`:
```json
{
  "apiKey": "your-api-key",
  "authDomain": "your-project.firebaseapp.com",
  "databaseURL": "https://your-project.firebaseio.com",
  "projectId": "your-project-id",
  "storageBucket": "your-project.appspot.com",
  "messagingSenderId": "123456789",
  "appId": "1:123456789:web:abcdef",
  "measurementId": "G-XXXXXXXXXX"
}
```

## Installation

### Prerequisites
- Web server (Apache, Nginx, or Python HTTP server)
- Modern web browser with ES6 support
- Firebase project with Realtime Database
- MQTT broker (HiveMQ Cloud, EMQX, or Mosquitto)

### Setup Steps

1. **Clone or Download**
   ```bash
   # Download the web directory to your server
   ```

2. **Configure Firebase**
   - Create a Firebase project at https://console.firebase.google.com
   - Enable Authentication (Email/Password provider)
   - Create a Realtime Database
   - Copy your Firebase configuration

3. **Configure MQTT Broker**
   - Set up MQTT broker with WebSocket support
   - Note broker host, port, and credentials
   - Ensure port 8083 (or your chosen port) is accessible

4. **Deploy Files**
   ```bash
   # Copy files to web server directory
   cp -r web/* /var/www/html/smarthome/
   ```

5. **Configure Application**
   - Open login.html in browser
   - Create user account
   - Navigate to Settings tab
   - Enter Firebase configuration
   - Enter MQTT configuration
   - Test connections

6. **Access Dashboard**
   - Navigate to index.html
   - Log in with your credentials
   - Start adding devices

### Local Development

Using Python HTTP server:
```bash
cd web
python -m http.server 8000
```

Access at: http://localhost:8000

## Usage

### Adding a Device

1. Navigate to Home tab
2. Click "Add Device" button
3. Enter Device ID (must match ESP device ID)
4. Enter Device Name
5. Click "Add"
6. Device card appears in grid

### Controlling a Device

1. Locate device card in Home tab
2. Toggle switch for relay control
3. Status updates in real-time
4. State persists in Firebase

### Viewing Sensor Data

1. Go to Dashboard tab
2. Select device from dropdown
3. Choose chart type (temperature/humidity/light)
4. View real-time graph
5. Chart updates every 5 seconds

### Exporting Data

1. Navigate to Data tab
2. Set filters (device, date range, values)
3. Click "Apply Filters"
4. Review filtered data in table
5. Click "Export to Excel"
6. Save downloaded file

### Configuring Settings

1. Go to Settings tab
2. Update MQTT or Firebase configuration
3. Click "Save Configuration"
4. Click "Test Connection" to verify
5. Settings persist in localStorage

## Responsive Design

### Breakpoints
- **Desktop:** 1024px and above - Full layout with sidebar
- **Tablet:** 768px to 1023px - Collapsible sidebar
- **Mobile:** Below 768px - Compact layout with hamburger menu

### Mobile Optimizations
- Touch-friendly controls
- Simplified navigation
- Stacked layouts
- Reduced chart complexity
- Optimized table display

## Browser Compatibility

### Supported Browsers
- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

### Required Features
- ES6 modules
- CSS custom properties
- Fetch API
- WebSocket
- Local Storage

## Security

### Authentication
- Firebase Authentication required for all pages
- Protected route mechanism
- Session token validation
- Automatic redirect on session expiry

### Data Security
- HTTPS recommended for production
- Firebase security rules enforce authentication
- MQTT credentials stored in localStorage
- No sensitive data in client-side code

### Best Practices
- Use environment-specific Firebase projects
- Implement Firebase security rules
- Use secure MQTT broker with authentication
- Enable CORS policies on server
- Implement Content Security Policy headers

## Performance

### Optimizations
- Lazy loading of charts
- State caching to reduce database reads
- Debounced user inputs
- Listener cleanup on tab switches
- Data limiting (charts show last 10 records)

### Resource Management
- Chart instances destroyed when not in use
- Firebase listeners unsubscribed on unmount
- MQTT reconnection with backoff
- Minimal DOM manipulation

## Troubleshooting

### MQTT Connection Issues
- Verify broker host and port
- Check WebSocket path (/mqtt)
- Ensure broker allows WebSocket connections
- Verify firewall rules
- Check broker authentication

### Firebase Connection Issues
- Verify Firebase configuration
- Check API key validity
- Ensure database URL is correct
- Verify authentication is enabled
- Check Firebase security rules

### Device Not Appearing
- Verify device ID matches in ESP code
- Check MQTT topic subscription
- Ensure device is publishing to correct topics
- Verify Firebase database structure
- Check browser console for errors

### Data Not Updating
- Check MQTT connection status
- Verify Firebase connection status
- Ensure real-time listeners are active
- Check device is sending data
- Verify network connectivity

## Maintenance

### Code Updates
- Edit JavaScript modules in assets/js/
- Modify styles in assets/css/
- Test changes locally before deployment
- Clear browser cache after updates
- Update documentation

### Adding Features
- Create new module in appropriate directory
- Import in main.js if needed
- Add UI elements in index.html
- Add styles in CSS modules
- Update documentation

### Database Cleanup
- Implement data retention policies
- Archive old historical data
- Remove orphaned device records
- Optimize database indexes
- Monitor database size

## Documentation

For detailed documentation on specific components:

- **[Assets Documentation](assets/README.md)** - Overview of CSS and JavaScript assets
- **[CSS Documentation](assets/css/README.md)** - Stylesheet architecture and design system
- **[JavaScript Documentation](assets/js/README.md)** - Module functionality and API reference

## Contributing

When contributing to the web dashboard:
1. Follow existing code style and conventions
2. Add comments for complex logic
3. Update documentation for new features
4. Test across multiple browsers
5. Ensure responsive design works
6. Validate accessibility

## License

This web dashboard is part of the SMART_HOME IoT project.

## Authors

- Le Quang Minh Nhat
- Thai Huu Loi
- Tran Huu Dao

## Support

For issues or questions:
1. Check browser console for errors
2. Verify configuration settings
3. Test MQTT and Firebase connectivity
4. Review module documentation
5. Check network connectivity
