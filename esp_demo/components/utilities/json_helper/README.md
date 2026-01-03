# JSON Helper

Safe JSON parsing and creation utilities for MQTT communication.

## Features

- Safe value extraction with default fallback
- Sensor data JSON creation
- Device state JSON creation
- WiFi scan result formatting
- Command parsing for MQTT messages

## API Reference

### Value Extraction

```c
const char *json_helper_get_string(cJSON *object, const char *key, const char *default_val);
double json_helper_get_number(cJSON *object, const char *key, double default_val);
int json_helper_get_int(cJSON *object, const char *key, int default_val);
bool json_helper_get_bool(cJSON *object, const char *key, bool default_val);
```

### JSON Creation

| Function | Purpose | Format |
|----------|---------|--------|
| `json_helper_create_data()` | Sensor readings | `{"timestamp", "temperature", "humidity", "light"}` |
| `json_helper_create_state()` | Device state | `{"timestamp", "mode", "interval", "fan", "light", "ac"}` |
| `json_helper_create_info()` | Device info | `{"timestamp", "id", "ssid", "ip", "broker", "firmware"}` |
| `json_helper_create_wifi_status()` | WiFi status | `{"connected", "provisioned", "ip", "rssi"}` |
| `json_helper_create_simple_response()` | Generic response | `{"status", "message"}` |

### Parsing Functions

```c
cJSON *json_helper_parse_command(const char *json_str, char *cmd_id, size_t cmd_id_len,
                                 char *command, size_t command_len);
esp_err_t json_helper_parse_wifi_credentials(const char *json_str,
                                             char *ssid_out, size_t ssid_len,
                                             char *password_out, size_t password_len);
char *json_helper_create_wifi_scan_result(const void *ap_list, uint16_t ap_count);
```

## Usage Example

```c
#include "json_helper.h"

// Create sensor data JSON
char *json = json_helper_create_data(timestamp, 25.5f, 60.0f, 500);
// Result: {"timestamp": 1234567890, "temperature": 25.5, "humidity": 60.0, "light": 500}

// Parse command
char cmd_id[8], command[32];
cJSON *root = json_helper_parse_command(json_str, cmd_id, sizeof(cmd_id), command, sizeof(command));
cJSON *params = cJSON_GetObjectItem(root, "params");
cJSON_Delete(root);

// Always free returned strings
free(json);
```

## Notes

- All create functions return heap-allocated strings - caller must free
- Parse functions return NULL on error
- Temperature and humidity are rounded to 2 decimal places
