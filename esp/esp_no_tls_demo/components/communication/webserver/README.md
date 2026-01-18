# Webserver Module

## Overview

HTTP server providing WiFi provisioning interface. Serves embedded web pages for network scanning, credential configuration, and device setup.

## Features

- HTTP server on configurable port
- Embedded static files (HTML, CSS, JavaScript)
- WiFi network scanning API
- Credential configuration endpoint
- Status monitoring endpoint
- Reset credentials endpoint
- JSON API responses
- Integration with WiFi Manager

## API Functions

```c
esp_err_t webserver_start(void);
esp_err_t webserver_stop(void);
```

## REST API Endpoints

### GET /

Serves the main HTML configuration page.

**Response:** HTML content

### GET /style.css

Serves the CSS stylesheet.

**Response:** CSS content

### GET /script.js

Serves the JavaScript file.

**Response:** JavaScript content

### GET /scan

Scans for available WiFi networks.

**Response:**
```json
{
    "networks": [
        {
            "ssid": "MyNetwork",
            "rssi": -45,
            "auth": 3
        }
    ]
}
```

### POST /connect

Configures WiFi credentials and connects.

**Request:**
```json
{
    "ssid": "MyNetwork",
    "password": "MyPassword"
}
```

**Response:**
```json
{
    "status": "success",
    "message": "Connecting to WiFi..."
}
```

### GET /status

Returns current WiFi connection status.

**Response:**
```json
{
    "connected": true,
    "provisioned": true,
    "ip": "192.168.1.100",
    "rssi": -45
}
```

### POST /reset

Clears saved WiFi credentials.

**Response:**
```json
{
    "status": "success",
    "message": "Credentials cleared"
}
```

## Usage Example

```c
#include "webserver.h"
#include "wifi_manager.h"

// Start provisioning mode
wifi_manager_start_provisioning();
webserver_start();

// Server is now running on http://192.168.4.1:80
// Wait for user configuration

// After successful configuration
webserver_stop();
wifi_manager_stop_provisioning();
```

## Embedded Files

Static files are embedded during build using CMake:

- index.html: Main configuration page
- style.css: Page styling
- script.js: Client-side logic

## Configuration

HTTP server port configurable via Kconfig:
- HTTP_SERVER_PORT: Default 80

## Dependencies

- ESP-IDF HTTP server
- wifi_manager component
- utilities/json_helper

## Features

- HTTP server on configurable port (default: 80)
- Embedded static files (HTML, CSS, JS)
- WiFi network scanning endpoint
- Credential configuration endpoint
- Status monitoring endpoint
- Reset credentials endpoint
- JSON API responses
- Integration with WiFi Manager

## File Structure

```
webserver/
    CMakeLists.txt
    webserver.c             # HTTP server implementation
    include/
        webserver.h         # Public API
    web/
        index.html          # Main configuration page
        style.css           # Page styling
        script.js           # Client-side JavaScript
```

## Dependencies

- `esp_http_server` - ESP-IDF HTTP server
- `wifi_manager` - WiFi management functions
- `utilities/json_helper` - JSON response creation

## API Reference

### Functions

| Function | Return | Description |
|----------|--------|-------------|
| `webserver_start(void)` | `esp_err_t` | Start the HTTP server |
| `webserver_stop(void)` | `esp_err_t` | Stop the HTTP server |

## REST API Endpoints

### GET /

Serves the main HTML configuration page.

**Response:** HTML content (index.html)

---

### GET /style.css

Serves the CSS stylesheet.

**Response:** CSS content (style.css)

---

### GET /script.js

Serves the JavaScript file.

**Response:** JavaScript content (script.js)

---

### GET /scan

Scans for available WiFi networks.

**Response:** JSON array of networks

```json
{
    "networks": [
        {
            "ssid": "MyNetwork",
            "rssi": -45,
            "auth": 3
        },
        {
            "ssid": "OtherNetwork",
            "rssi": -72,
            "auth": 4
        }
    ]
}
```

| Field | Type | Description |
|-------|------|-------------|
| ssid | string | Network name |
| rssi | number | Signal strength (dBm) |
| auth | number | Authentication mode |

---

### POST /connect

Submits WiFi credentials and initiates connection.

**Request Body:**

```json
{
    "ssid": "MyNetwork",
    "password": "MyPassword"
}
```

**Response (Success):**

```json
{
    "status": "ok",
    "message": "Connecting..."
}
```

**Behavior:**
1. Saves credentials to NVS
2. Sends success response
3. Waits 1 second
4. Restarts ESP32 to apply configuration

---

### GET /status

Gets current WiFi connection status.

**Response:**

```json
{
    "connected": true,
    "provisioned": true,
    "ip": "192.168.1.100",
    "rssi": -45
}
```

| Field | Type | Description |
|-------|------|-------------|
| connected | boolean | Currently connected to AP |
| provisioned | boolean | Credentials saved in NVS |
| ip | string | Current IP address (null if not connected) |
| rssi | number | Signal strength (0 if not connected) |

---

### POST /reset

Clears saved WiFi credentials.

**Response:**

```json
{
    "status": "ok",
    "message": "Credentials cleared"
}
```

**Behavior:**
1. Clears credentials from NVS
2. Sends success response
3. Waits 1 second
4. Restarts ESP32

## Server Configuration

```c
httpd_config_t config = HTTPD_DEFAULT_CONFIG();
config.server_port = HTTP_SERVER_PORT;  // Default: 80
config.max_uri_handlers = 8;
config.stack_size = 8192;
```

## Embedded Files

Static web files are embedded into the firmware using CMake `EMBED_FILES`:

```cmake
idf_component_register(
    SRCS "webserver.c"
    INCLUDE_DIRS "include"
    EMBED_FILES "web/index.html" "web/style.css" "web/script.js"
)
```

**Accessing Embedded Files:**

```c
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

// Send embedded file
httpd_resp_send(req, (const char *)index_html_start, 
                index_html_end - index_html_start);
```

## Usage Examples

### Basic Usage

```c
#include "wifi_manager.h"
#include "webserver.h"

void start_provisioning(void)
{
    // Start WiFi in AP mode
    wifi_manager_start_provisioning();
    
    // Start web server
    webserver_start();
    
    ESP_LOGI(TAG, "Connect to AP and open http://192.168.4.1");
}

void stop_provisioning(void)
{
    webserver_stop();
    wifi_manager_stop_provisioning();
}
```

### Integration with WiFi Manager

```c
static void wifi_callback(wifi_manager_event_t event)
{
    switch (event)
    {
        case WIFI_EVENT_PROVISIONING_START:
            webserver_start();
            break;
            
        case WIFI_EVENT_PROVISIONING_STOP:
            webserver_stop();
            break;
            
        case WIFI_EVENT_GOT_IP:
            // Optionally stop webserver after successful provisioning
            webserver_stop();
            break;
            
        default:
            break;
    }
}
```

## Provisioning Flow

```
1. User connects to ESP32 AP ("SmartHome_Setup")
          |
          v
2. DNS server redirects to 192.168.4.1
          |
          v
3. Browser loads GET / (index.html)
          |
          v
4. JavaScript calls GET /scan
          |
          v
5. User selects network and enters password
          |
          v
6. JavaScript sends POST /connect
          |
          v
7. Server saves credentials and restarts
          |
          v
8. ESP32 connects to configured network
```

## Web Interface

The embedded web interface provides:

- **Network List**: Displays scanned networks with signal strength
- **Password Input**: Secure password field
- **Connect Button**: Submits credentials
- **Status Display**: Shows current connection status
- **Reset Button**: Clears saved credentials

## Error Handling

### HTTP Error Responses

| Code | Condition |
|------|-----------|
| 400 Bad Request | Invalid POST data or JSON parse error |
| 500 Internal Server Error | Scan failed, memory allocation failed, or save failed |

### Function Return Values

| Return Value | Description |
|--------------|-------------|
| `ESP_OK` | Operation successful |
| `ESP_FAIL` | Server start/stop failed |

## Thread Safety

- HTTP server runs in its own task
- Request handlers execute sequentially
- Uses WiFi Manager thread-safe functions
- JSON helper functions are thread-safe

## Memory Considerations

- Server stack size: 8KB
- POST buffer size: 512 bytes
- Embedded files increase firmware size
- JSON responses are dynamically allocated and freed

## Notes

- Server only runs during provisioning mode
- Captive portal requires DNS server from WiFi Manager
- After successful provisioning, ESP32 restarts automatically
- Web files are embedded at compile time - changes require rebuild
- Maximum SSID: 32 characters, Maximum Password: 64 characters
