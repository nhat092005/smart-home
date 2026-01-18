# Smart Home Web Dashboard - Non-TLS Version

## Overview

Real-time web-based monitoring and control system for Smart Home IoT devices without SSL/TLS encryption. Implements bidirectional communication using MQTT over WebSocket (non-secure) and Firebase Realtime Database for data persistence. Designed for local network deployments.

## Key Differences from TLS Version

- **MQTT Connection**: WebSocket without SSL (ws:// instead of wss://)
- **Security Level**: No encryption for MQTT communication
- **Use Case**: Local network deployment only
- **Firebase**: Hosted on Firebase with HTTPS (Firebase always uses HTTPS)
- **Performance**: Slightly lower latency without TLS overhead

## Features

- Real-time device control via MQTT WebSocket (non-TLS)
- Live sensor data visualization (temperature, humidity, light)
- Interactive charts with Chart.js
- Historical data export to Excel
- Device management (add/edit/delete)
- MQTT and Firebase configuration
- User authentication with Firebase Auth
- Responsive single-page application

## System Architecture

### High-Level System Flowchart

```
[START]
   |
   v
(Initialize Application)
   |
   v
<Authentication Required?>----YES--->(Redirect to Login Page)
   |                                          |
   NO                                         v
   |                                    (User Login/Register)
   |                                          |
   v                                          v
(Initialize Firebase Connection)---->(Authentication Success?)
   |                                          |
   v                                         YES
(Initialize MQTT Client)                      |
   |                                          v
   v                                   (Redirect to Dashboard)
(Subscribe to Device Topics)                  |
   |<-------------------------------------
   v
(Start Real-Time Listeners)
   |
   v
<User Action?>
   |
   +--->(Device Control)--->(Send MQTT Command)--->(ESP32 Device)
   |                                                      |
   +--->(View Charts)--->(Fetch Historical Data)         v
   |                                           (Device Processes Command)
   +--->(Export Data)--->(Filter & Download)             |
   |                                                      v
## Security Warning

This version uses MQTT over WebSocket without SSL/TLS encryption. Data transmitted between browser and MQTT broker is not encrypted. Use only on trusted local networks. Not recommended for internet-facing deployments.

## Technology Stack

- HTML5 with semantic markup
- CSS3 with modern features (Grid, Flexbox, Custom Properties)
- JavaScript ES6 modules
- Chart.js for data visualization
- Firebase Realtime Database and Authentication (HTTPS)
- MQTT.js for WebSocket communication (non-TLS)
- SheetJS for Excel export
(Receive MQTT Messages)<--------------------------------+
   |
   v
(Update Firebase Database)
   |
   v
(Firebase Listeners Triggered)
   |
   v
(Update UI Components)
   |
   v
<Continue?>----YES-->(Back to User Action)
   |
   NO
   v
[END]
```

### Data Flow Architecture

```
ESP32 Devices Layer
    |
    | (MQTT Publish)
    v
MQTT Broker (Mosquitto)
    |
    | (WebSocket)
    v
Web Dashboard (JavaScript)
    |
    +---> MQTT Handler ----> Firebase Sync ----> Firebase Database
    |
    +---> UI Update <---- Firebase Listeners <---- Firebase Database
    |
    v
User Interface (HTML/CSS)
```

### Communication Protocol Flow

```
Device Control Flow:
[User Click] --> [UI Event] --> [Generate Command ID] --> [Send MQTT Command]
                                                                  |
                                                                  v
                                                         [MQTT Broker Forward]
                                                                  |
                                                                  v
                                                            [ESP32 Receives]
                                                                  |
                                                                  v
                                                         [Execute Command]
                                                                  |
                                                                  v
                                                   [Publish Response Message]
                                                                  |
                                                                  v
                                         [Dashboard Receives Response]
                                                                  |
                                                                  v
                                               [Resolve Pending Command]
                                                                  |
                                                                  v
                                                          [Update UI State]

Sensor Data Flow:
[ESP32 Sensor Reading] --> [MQTT Publish Data Topic] --> [MQTT Broker]
                                                                |
                                                                v
                                                 [Dashboard MQTT Handler]
                                                                |
                                                                v
                                                   [Parse Sensor Data]
                                                                |
                                                                v
                                          [Sync to Firebase Database]
                                                                |
                                                                v
                                              [Firebase Listener Triggered]
                                                                |
                                                                v
                                                 [Update Device Card UI]
                                                                |
                                                                v
                                                    [Update Chart Data]
```

## Core Features and Modules

### Real-time Device Control
- **Device Management** - Add, edit, and delete devices with unique identifiers
- **Relay Control** - Toggle device states (ON/OFF) with instant feedback
- **Live Sensor Data** - Real-time temperature, humidity, and light sensor readings
- **Device Status** - Connection status, uptime, IP address, and firmware version
- **MQTT Communication** - WebSocket-based bidirectional communication with ESP devices

### Data Visualization
- **Interactive Charts** - Dynamic graphs powered by Chart.js
- **Multi-sensor Support** - Separate visualizations for temperature, humidity, and light
- **Real-time Updates** - Charts refresh automatically as new data arrives
- **Historical View** - Display last 10 readings for trend analysis
- **Chart Type Switching** - Toggle between different sensor types on demand

### Data Export
- **Historical Data Retrieval** - Fetch all sensor readings from Firebase
- **Advanced Filtering** - Filter by device, date range, temperature, humidity, and light values
- **Quick Filters** - One-click access to last 7, 30, or 90 days of data
- **Excel Export** - Download filtered data in Excel format for offline analysis
- **Table View** - Browse historical data in an organized table format

### System Settings
- **MQTT Configuration** - Customize broker settings (host, port, path, credentials, SSL)
- **Firebase Configuration** - Switch between Firebase projects without code changes
- **Connection Testing** - Test MQTT and Firebase connectivity
- **Device Management** - View device information and send reboot commands
- **Time Synchronization** - Sync system time to all connected devices

### User Authentication
- **Firebase Authentication** - Secure email/password login
- **User Registration** - Sign up with email and password
- **Protected Routes** - Automatic redirect to login for unauthorized access
- **Session Management** - Persistent login across page reloads
- **Logout Functionality** - Secure session termination

## Technology Stack

### Frontend
- **HTML5** - Semantic markup with accessibility support
- **CSS3** - Modern styling with CSS custom properties, flexbox, and grid
- **JavaScript ES6** - Modular architecture with native ES6 modules

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
  "host": "raspberrypi.local",
  "port": 8083,
  "path": "/mqtt",
  "useSSL": false,
  "username": "SmartHome",
  "password": "SmartHome01",
  "keepalive": 60
}
```

**Note:** This configuration connects to a local Mosquitto broker without TLS:
- Port 1883: Standard MQTT for ESP32 devices
- Port 8083: WebSocket for web dashboard (no TLS)
- Change `host` to your broker's IP address or hostname
- Ensure your broker's `mosquitto.conf` has WebSocket listener enabled

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
- Local Mosquitto MQTT broker with WebSocket support (no TLS)
  - Port 1883 for standard MQTT (ESP32)
  - Port 8083 for WebSocket (web dashboard)

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
   - Set up local Mosquitto broker with WebSocket support
   - Configure `mosquitto.conf` with:
     - Standard MQTT listener on port 1883
     - WebSocket listener on port 8083
   - Set username/password authentication
   - Ensure firewall allows ports 1883 and 8083
   - Verify broker is accessible from web client

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
