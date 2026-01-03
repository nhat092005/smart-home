# ESP32 Smart Home - MQTT Control Commands

**Broker:** `mqtts://6ceea111b6144c71a57b21faa3553fc6.s1.eu.hivemq.cloud:8883`  
**Command Topic:** `SmartHome/esp_01/command`  
**Username:** `SmartHome`  
**Password:** `SmartHome01`

---

## Command Format

All commands must be sent in JSON format to the topic `SmartHome/esp_01/command`:

```json
{
  "id": "unique_id",
  "command": "command_name",
  "params": {
    /* parameters */
  }
}
```

**Example using mosquitto_pub:**

```bash
mosquitto_pub -h 6ceea111b6144c71a57b21faa3553fc6.s1.eu.hivemq.cloud \
  -p 8883 \
  -u SmartHome \
  -P SmartHome01 \
  -t SmartHome/esp_01/command \
  --cafile path/to/ca.pem \
  -V mqttv311 \
  -m '{"id":"001","command":"set_device","params":{"device":"fan","state":1}}'
```

---

## Available Commands

### 1. set_device - Control Single Device

**Description:** Turn on/off a single device (fan, light, or ac)

**JSON Format:**

```json
{
    "id": "unique_id",
    "command": "set_device",
    "params": {
        "device": "fan|light|ac",
        "state": 0|1
    }
}
```

**Example:**

```json
{
  "id": "cmd_001",
  "command": "set_device",
  "params": {
    "device": "fan",
    "state": 1
  }
}
```

**Supported devices:**

- `"fan"` - Fan
- `"light"` - Light
- `"ac"` - Air Conditioner

**State:**

- `0` - OFF
- `1` - ON

---

### 2. set_devices - Control Multiple Devices

**Description:** Turn on/off multiple devices simultaneously

**JSON Format:**

```json
{
    "id": "unique_id",
    "command": "set_devices",
    "params": {
        "fan": 0|1|-1,
        "light": 0|1|-1,
        "ac": 0|1|-1
    }
}
```

**Example:** Turn off fan, turn on light, leave AC unchanged

```json
{
  "id": "cmd_002",
  "command": "set_devices",
  "params": {
    "fan": 0,
    "light": 1,
    "ac": -1
  }
}
```

**State:**

- `0` - OFF
- `1` - ON
- `-1` - No change (skip)

---

### 3. set_mode - Set Operating Mode

**Description:** Set system-wide operating mode

**JSON Format:**

```json
{
    "id": "unique_id",
    "command": "set_mode",
    "params": {
        "mode": 0|1
    }
}
```

**Example:**

```json
{
  "id": "cmd_003",
  "command": "set_mode",
  "params": {
    "mode": 1
  }
}
```

**Mode:**

- `0` - OFF (System disabled)
- `1` - ON (System enabled)

---

### 4. set_interval - Change Sensor Reading Interval

**Description:** Set the time interval between sensor readings (in seconds)

**JSON Format:**

```json
{
  "id": "unique_id",
  "command": "set_interval",
  "params": {
    "interval": 5
  }
}
```

**Example:** Set sensor reading interval to every 10 seconds

```json
{
  "id": "cmd_004",
  "command": "set_interval",
  "params": {
    "interval": 10
  }
}
```

**Requirements:**

- Value must be between **5 to 3600 seconds**
- Default: **5 seconds**
- If value is out of range, error occurs and no change is made

---

### 5. get_status - Get Current Status

**Description:** Request ESP32 to send back all status information

**JSON Format:**

```json
{
  "id": "unique_id",
  "command": "get_status",
  "params": {}
}
```

**Example:**

```json
{
  "id": "cmd_005",
  "command": "get_status",
  "params": {}
}
```

**Response:** ESP32 will publish to:

- `/data` - Sensor readings (temperature, humidity, light)
- `/state` - Device status (mode, fan, light, ac, interval)
- `/info` - System information (firmware, IP, SSID)

---

### 6. reboot - Restart ESP32

**Description:** Restart the entire system (after 2 seconds)

**JSON Format:**

```json
{
  "id": "unique_id",
  "command": "reboot",
  "params": {}
}
```

**Example:**

```json
{
  "id": "cmd_006",
  "command": "reboot",
  "params": {}
}
```

**Note:** Device will restart after 2 seconds

---

### 7. factory_reset - Reset to Factory Settings

**Description:** Erase all NVS (Non-Volatile Storage) data and restart

**JSON Format:**

```json
{
  "id": "unique_id",
  "command": "factory_reset",
  "params": {}
}
```

**Example:**

```json
{
  "id": "cmd_007",
  "command": "factory_reset",
  "params": {}
}
```

**Note:**

- Warning: **Will DELETE all settings**
- Device will restart after 2 seconds
- After reset, WiFi configuration is required

---

### 8. set_timestamp - Set System Timestamp

**Description:** Set the current timestamp for the system

**JSON Format:**

```json
{
  "id": "unique_id",
  "command": "set_timestamp",
  "params": {
    "timestamp": 1734958200
  }
}
```

**Example:**

```json
{
  "id": "cmd_008",
  "command": "set_timestamp",
  "params": {
    "timestamp": 1734958200
  }
}
```

---

### 9. ping - Ping for check MQTT

**Description:** Checks whether the device is online and responsive via MQTT. The device replies immediately with a pong response.

**JSON Format:**

```json
{
  "id": "unique_id",
  "command": "ping",
  "params": {}
}
```

**Example:**

```json
{
  "id": "cmd_008",
  "command": "ping",
  "params": {}
}
```

---

## Received Data Topics (Subscribe)

### Topic: `SmartHome/esp_01/data` (QoS=0)

**Content:** Sensor data (read every 5 seconds by default)

```json
{
  "timestamp": 946690040,
  "temperature": 30.86,
  "humidity": 62.32,
  "light": 225
}
```

### Topic: `SmartHome/esp_01/state` (QoS=1, Retain)

**Content:** Current status (backup every 60 seconds)

```json
{
  "timestamp": 946690040,
  "mode": 1,
  "interval": 5,
  "fan": 0,
  "light": 1,
  "ac": 1
}
```

### Topic: `SmartHome/esp_01/info` (QoS=1, Retain)

**Content:** System information (sent on connection + network change)

```json
{
  "timestamp": 946690040,
  "id": "esp_01",
  "ssid": "Your_WiFi_SSID",
  "ip": "192.168.1.22",
  "broker": "mqtts://6ceea111b6144c71a57b21faa3553fc6.s1.eu.hivemq.cloud:8883",
  "firmware": "1.0.0"
}
```

---

## General Rules

| Component   | Details                                                  |
| ----------- | -------------------------------------------------------- |
| **id**      | Unique command ID (string, arbitrary). Used for tracking |
| **command** | Command name must be exact (case-sensitive)              |
| **params**  | Object containing parameters (can be empty `{}`)         |
| **Topic**   | Always `SmartHome/esp_01/command`                        |
| **QoS**     | Automatically 1 (at-least-once delivery)                 |

---

## Connection Testing

To verify MQTT connection is working:

```bash
# Subscribe to all topics
mosquitto_sub -h 6ceea111b6144c71a57b21faa3553fc6.s1.eu.hivemq.cloud \
  -p 8883 -u SmartHome -P SmartHome01 \
  -t "SmartHome/esp_01/#" \
  --cafile ca.pem \
  -V mqttv311 \
  -v
```

You will see:

- `/data` - Sensor data every 5 seconds
- `/state` - State backup every 60 seconds
- `/info` - System information on connection
