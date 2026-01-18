# JSON Helper Module

## Overview

Type-safe JSON manipulation utilities for creating and parsing MQTT message payloads in the Smart Home system.

## Features

- Safe JSON value extraction with default fallbacks
- MQTT message payload generation for sensor data, device state, and system info
- Null pointer checking and type validation
- Automatic memory management helpers

## API Functions

### Value Extraction

```c
const char *json_helper_get_string(cJSON *object, const char *key, const char *default_val);
double json_helper_get_number(cJSON *object, const char *key, double default_val);
int json_helper_get_int(cJSON *object, const char *key, int default_val);
bool json_helper_get_bool(cJSON *object, const char *key, bool default_val);
```

### Message Creation

```c
char *json_helper_create_data(uint32_t timestamp, float temperature, float humidity, int light);
char *json_helper_create_state(uint32_t timestamp, int mode, int interval, int fan, int light, int ac);
char *json_helper_create_info(uint32_t timestamp, const char *device_id, const char *ssid, 
                               const char *ip, const char *broker);
```

## Usage Example

```c
#include "json_helper.h"

// Create sensor data JSON
char *json_str = json_helper_create_data(1700000000, 25.5, 60.3, 450);
// Result: {"timestamp":1700000000,"temperature":25.5,"humidity":60.3,"light":450}

// Parse JSON safely
cJSON *root = cJSON_Parse(received_json);
int mode = json_helper_get_int(root, "mode", 0);
bool fan_state = json_helper_get_bool(root, "fan", false);

// Clean up
free(json_str);
cJSON_Delete(root);
```

## Memory Management

All `json_helper_create_*` functions return dynamically allocated strings. Caller must free the returned string using `free()`.

## Dependencies

- ESP-IDF cJSON library
- ESP-IDF logging (esp_log)

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
