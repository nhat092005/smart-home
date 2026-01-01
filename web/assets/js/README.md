# JavaScript Assets

## Overview

This directory contains all JavaScript modules for the Smart Home web dashboard. The codebase follows ES6 module architecture with clear separation of concerns across core functionality, device management, MQTT communication, data visualization, and UI interactions.

## Structure

```
js/
├── main.js                # Application entry point
├── core/                  # Core functionality
│   ├── firebase-config.js # Firebase initialization and config
│   ├── auth.js            # Authentication logic
│   └── login.js           # Login/signup page controller
├── mqtt/                  # MQTT communication
│   ├── mqtt-client.js     # MQTT client management
│   ├── mqtt-handlers.js   # Message handling and state caching
│   ├── mqtt-to-firebase.js # MQTT to Firebase data sync
│   └── ping-service.js    # Device online/offline ping service
├── devices/               # Device management
│   ├── device-manager.js  # Device CRUD operations
│   └── device-card.js     # Device UI rendering
├── charts/                # Data visualization
│   ├── chart-manager.js   # Chart.js integration
│   └── chart-config.js    # Chart configuration
├── export/                # Data export
│   └── data-export.js     # Historical data export to Excel
├── settings/              # Settings management
│   └── settings-manager.js # System settings and configuration
├── utils/                 # Utility functions
│   └── helpers.js         # Common utilities (escapeHtml, formatInterval)
└── ui/                    # UI utilities
    └── ui-helpers.js      # UI helper functions
```

## Module Documentation

### Core Modules

#### main.js
**Purpose:** Application entry point that coordinates all modules and initializes the application

**Key Functions:**
- Imports and initializes all application modules
- Sets up MQTT client with message handlers
- Initializes device manager and UI components
- Coordinates tab switching between views (dashboard, report, export, settings)
- Manages application lifecycle

**Dependencies:** All other modules

#### core/firebase-config.js
**Purpose:** Firebase configuration and initialization

**Key Functions:**
- `loadConfig()` - Loads Firebase config from localStorage or defaults
- `saveConfig(config)` - Saves Firebase config to localStorage
- `getConfig()` - Returns current Firebase configuration
- `isValidConfig(config)` - Validates Firebase config object

**Exports:**
- `app` - Initialized Firebase app instance
- `auth` - Firebase authentication instance
- `db` - Firebase Realtime Database instance

**Features:**
- User-configurable Firebase credentials
- Config validation
- Default demo configuration
- Local storage persistence

#### core/auth.js
**Purpose:** User authentication and authorization

**Key Functions:**
- `requireAuth()` - Checks if user is authenticated, redirects if not
- `logout()` - Signs out user and clears session
- `getCurrentUser()` - Returns current authenticated user
- `onAuthStateChanged(callback)` - Listens for auth state changes

**Features:**
- Firebase Authentication integration
- Protected route handling
- Session management
- User state monitoring

#### core/login.js
**Purpose:** Login and signup page controller

**Key Functions:**
- Handles login form submission
- Manages signup flow
- Displays authentication errors
- Redirects to dashboard after successful login

**Features:**
- Email/password authentication
- Form validation
- Toggle between login and signup modes
- Error messaging

### MQTT Modules

#### mqtt/mqtt-client.js
**Purpose:** MQTT client management and connection handling

**Key Functions:**
- `initializeMQTTClient(onMessageCallback, onConnectCallback, onConnectionLostCallback)` - Initializes MQTT client
- `subscribeToDevice(deviceId)` - Subscribes to device topics
- `sendMQTTCommand(deviceId, command, value)` - Sends commands to devices
- `loadMQTTConfig()` - Loads MQTT configuration
- `saveMQTTConfig(config)` - Saves MQTT configuration

**Constants:**
- `MQTT_TOPICS` - Topic definitions (data, state, info, command)

**Features:**
- Configurable MQTT broker settings
- Automatic reconnection with retry logic
- WebSocket over MQTT protocol
- Command counter for message tracking
- SSL/TLS support

#### mqtt/mqtt-handlers.js
**Purpose:** MQTT message handling and state management

**Key Functions:**
- `handleMQTTMessage(topic, message)` - Processes incoming MQTT messages
- `handleMQTTConnect()` - Handles successful MQTT connection
- `handleMQTTConnectionLost()` - Handles connection loss
- `getMQTTCachedState(deviceId)` - Retrieves cached device state

**Features:**
- Message parsing and validation
- State caching for offline resilience
- Topic routing (data/state/info/command)
- UI update coordination
- Firebase synchronization

#### mqtt/mqtt-to-firebase.js
**Purpose:** Syncs MQTT data to Firebase Realtime Database

**Key Functions:**
- `saveSensorDataToFirebase(deviceId, sensorData)` - Saves sensor readings
- `saveDeviceStateToFirebase(deviceId, state)` - Saves device state
- `saveDeviceInfoToFirebase(deviceId, info)` - Saves device information

**Features:**
- Real-time data synchronization
- Historical data storage
- Timestamp management
- Data structure optimization

### Device Modules

#### devices/device-manager.js
**Purpose:** Device CRUD operations and Firebase synchronization

**Key Functions:**
- `initializeDeviceManager(viewType)` - Initializes device manager
- `addDevice(deviceId, deviceName)` - Creates new device
- `updateDevice(deviceId, data)` - Updates device properties
- `deleteDevice(deviceId)` - Removes device
- `getAllDevices()` - Returns all devices
- `setViewType(type)` - Sets view mode (manage/dashboard)

**Features:**
- Firebase real-time listeners
- Device state management
- CRUD operations with validation
- View-specific rendering
- Cache management

#### devices/device-card.js
**Purpose:** Device UI rendering and control

**Key Functions:**
- `renderDeviceGrid(devices, viewType)` - Renders device grid
- `createDeviceCard(device, viewType)` - Creates single device card
- `getAllDevicesData()` - Returns all device data
- `getDeviceData(deviceId)` - Returns specific device data

**Features:**
- Dynamic card generation
- Real-time value updates
- Toggle controls for actuators
- Sensor value display (temperature, humidity, light)
- Edit and delete actions
- Modal integration

### Chart Modules

#### charts/chart-manager.js
**Purpose:** Data visualization using Chart.js

**Key Functions:**
- `initializeChart(deviceId, chartType)` - Creates chart for device
- `switchChartType(chartType)` - Changes chart type (temp/humid/light)
- `cleanupChart()` - Destroys chart instance

**Features:**
- Real-time data visualization
- Multiple chart types (temperature, humidity, light)
- Firebase data streaming
- Automatic data limiting (last 10 records)
- Chart cleanup and memory management

#### charts/chart-config.js
**Purpose:** Chart.js configuration and theming

**Exports:**
- `CHART_CONFIG` - Pre-configured chart settings for each sensor type
- `CHART_COLORS` - Color definitions for charts

**Features:**
- Responsive chart options
- Sensor-specific configurations
- Color schemes matching UI theme
- Animation settings
- Tooltip formatting

### Export Module

#### export/data-export.js
**Purpose:** Historical data export and filtering

**Key Functions:**
- `fetchAllHistoryData()` - Fetches all historical records
- `renderHistoryTable(data)` - Renders data table
- `exportTableToExcel(data, filename)` - Exports to Excel format
- `applyFilters()` - Applies data filters
- `resetFilters()` - Clears all filters
- `setQuickFilter(days)` - Sets quick time filter
- `exportFilteredData()` - Exports filtered dataset

**Features:**
- Firebase history data retrieval
- Device name resolution
- Multi-criteria filtering (device, date range, value ranges)
- Quick filters (7/30/90 days)
- Excel export with SheetJS
- Table pagination
- Data formatting

### Settings Module

#### settings/settings-manager.js
**Purpose:** System settings and configuration management

**Key Functions:**
- `initializeSettings()` - Loads all settings panels
- `loadDeviceInfoTable()` - Displays device information
- `rebootDevice(deviceId)` - Sends reboot command
- `syncTimeToAllDevices()` - Syncs time to all devices
- `updateMQTTConfig()` - Updates MQTT configuration
- `updateFirebaseConfig()` - Updates Firebase configuration
- `testFirebaseConnection()` - Tests Firebase connectivity
- `testMQTTConnection()` - Tests MQTT connectivity

**Features:**
- MQTT broker configuration
- Firebase configuration
- Device management
- Time synchronization
- Connection testing
- Configuration validation
- Settings persistence

### UI Module

#### ui/ui-helpers.js
**Purpose:** UI utility functions and interactions

**Key Functions:**
- `updateStatusBadge(badgeId, status, text)` - Updates status indicators
- `switchTab(tabName)` - Switches between application views
- `showModal(modalId)` - Opens modal dialogs
- `hideModal(modalId)` - Closes modal dialogs
- `initializeSidebar()` - Sets up sidebar interactions
- `startClock()` - Starts real-time clock

**Features:**
- Tab navigation
- Modal management
- Status badge updates
- Sidebar collapse/expand
- Real-time clock
- Page title updates

## Data Flow

### Device Control Flow
1. User clicks toggle in UI
2. `device-card.js` captures event
3. `mqtt-client.js` publishes command to MQTT broker
4. ESP device receives command and executes
5. Device publishes state update
6. `mqtt-handlers.js` receives state update
7. `mqtt-to-firebase.js` saves to Firebase
8. UI updates in real-time

### Sensor Data Flow
1. ESP device publishes sensor data via MQTT
2. `mqtt-handlers.js` receives and parses data
3. `mqtt-to-firebase.js` saves to Firebase (current + history)
4. Firebase listeners trigger UI updates
5. `device-card.js` updates sensor values
6. `chart-manager.js` updates charts if active

### Authentication Flow
1. User enters credentials in login page
2. `login.js` validates and submits to Firebase Auth
3. `auth.js` processes authentication
4. On success, redirect to dashboard
5. `main.js` initializes application
6. `requireAuth()` protects all subsequent operations

## Configuration

### MQTT Configuration
Stored in localStorage as `mqtt_config`:
- `host` - MQTT broker hostname
- `port` - WebSocket port (typically 8083)
- `path` - WebSocket path (typically /mqtt)
- `useSSL` - Enable SSL/TLS
- `username` - MQTT username
- `password` - MQTT password
- `keepalive` - Connection keepalive interval

### Firebase Configuration
Stored in localStorage as `user_firebase_config`:
- `apiKey` - Firebase API key
- `authDomain` - Auth domain
- `databaseURL` - Realtime Database URL
- `projectId` - Project ID
- `storageBucket` - Storage bucket
- `messagingSenderId` - Messaging sender ID
- `appId` - App ID
- `measurementId` - Analytics measurement ID

## Dependencies

### External Libraries
- Firebase JS SDK v10.7.1 (app, auth, database)
- Paho MQTT v1.0.1 (WebSocket MQTT client)
- Chart.js v3.9.1 (data visualization)
- SheetJS (xlsx) (Excel export)
- Font Awesome 6.4.0 (icons)

### Import Method
All dependencies are loaded via CDN using ES6 modules:
- Firebase: `https://www.gstatic.com/firebasejs/10.7.1/`
- Paho MQTT: `https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/`
- Chart.js: `https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.9.1/`

## Error Handling

### Pattern
All modules implement consistent error handling:
```javascript
try {
    // Operation
} catch (error) {
    console.error('[ModuleName] Error description:', error.message);
    // User feedback
}
```

### User Feedback
- Connection errors shown in status badges
- Operation errors displayed in modals
- Console logging for debugging
- Retry mechanisms for transient failures

## Best Practices

### Module Design
- Each module has single responsibility
- Clear import/export boundaries
- Minimal global state
- Event-driven architecture

### Code Style
- JSDoc comments for all functions
- Descriptive variable names
- Constants in UPPER_SNAKE_CASE
- Async/await for promises
- Error handling in all async operations

### Performance
- Lazy loading of charts
- Debounced user inputs
- Cached device states
- Cleanup of listeners on tab switch
- Memory management for chart instances

## Usage

Import main.js as ES6 module in HTML:

```html
<script type="module" src="assets/js/main.js"></script>
```

For login page:

```html
<script type="module" src="assets/js/core/login.js"></script>
```

## Maintenance

When extending functionality:
1. Create new module in appropriate directory
2. Export functions explicitly
3. Import in main.js if needed at startup
4. Follow established naming conventions
5. Add error handling and logging
6. Update this documentation
