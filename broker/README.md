# MQTT Broker

## Overview

Mosquitto MQTT broker configuration for Smart Home IoT system. Provides secure message routing between ESP32 devices, web dashboards, and mobile applications with authentication and logging capabilities.

## Directory Structure

```
broker/
├── mosquitto.conf           # Main broker configuration
├── config/                  # Configuration files
│   └── auth/                # Authentication
│       └── passwd.txt       # Password file
├── data/                    # Persistent data storage
├── log/                     # Broker logs
└── smart_home_scripts/      # Integration scripts
    └── mqtt_to_db.py        # MQTT to Firebase bridge
```

## Configuration

### mosquitto.conf

Main broker configuration file with security and networking settings.

**Key Settings:**
```
listener 1883              # Standard MQTT port
listener 8000              # WebSocket port (no TLS)
listener 8081              # WebSocket port (with TLS)
protocol websockets        # WebSocket protocol
allow_anonymous false      # Require authentication
password_file config/auth/passwd.txt
log_dest file log/mosquitto.log
persistence true
persistence_location data/
```

## Ports

| Port | Protocol | Security | Usage |
|------|----------|----------|-------|
| 1883 | MQTT | Plain | ESP32 devices (local) |
| 8000 | WebSocket | Plain | Web dashboard (no TLS) |
| 8081 | WebSocket | TLS/SSL | Web dashboard (secure) |

## Authentication

### Password File

Location: `config/auth/passwd.txt`

**Format:**
```
username:encrypted_password_hash
```

### Creating Users

```bash
# Add new user
mosquitto_passwd -b config/auth/passwd.txt username password

# Update password
mosquitto_passwd -b config/auth/passwd.txt username newpassword

# Delete user (edit passwd.txt manually)
```

### User Management

```bash
# Create password file (first time)
mosquitto_passwd -c config/auth/passwd.txt admin

# Add additional users
mosquitto_passwd -b config/auth/passwd.txt esp_device esp_password
mosquitto_passwd -b config/auth/passwd.txt web_client web_password
```

## Running the Broker

### Docker (Recommended)

```bash
# Start broker
docker run -d \
  --name mosquitto \
  -p 1883:1883 \
  -p 8000:8000 \
  -p 8081:8081 \
  -v $(pwd)/mosquitto.conf:/mosquitto/config/mosquitto.conf \
  -v $(pwd)/config:/mosquitto/config \
  -v $(pwd)/data:/mosquitto/data \
  -v $(pwd)/log:/mosquitto/log \
  eclipse-mosquitto

# Stop broker
docker stop mosquitto

# View logs
docker logs -f mosquitto
```

### Native Installation

```bash
# Install Mosquitto
sudo apt-get install mosquitto mosquitto-clients

# Copy configuration
sudo cp mosquitto.conf /etc/mosquitto/mosquitto.conf

# Start service
sudo systemctl start mosquitto

# Enable auto-start
sudo systemctl enable mosquitto

# Check status
sudo systemctl status mosquitto
```

## Logging

### Log Location
`log/mosquitto.log`

### Log Levels
```
log_type error
log_type warning
log_type notice
log_type information
```

### Sample Log Entry
```
1705561200: New connection from 192.168.1.100 on port 1883.
1705561201: New client connected from 192.168.1.100 as device_esp001
1705561205: Received PUBLISH from device_esp001 (d0, q0, r0, m0, 'esp/device_esp001/data', ... (100 bytes))
```

## Persistent Storage

### Data Directory
`data/`

**Contents:**
- mosquitto.db - Persistent message storage
- Client subscriptions
- Retained messages

### Backup
```bash
# Backup persistent data
tar -czf mosquitto_backup_$(date +%Y%m%d).tar.gz data/

# Restore
tar -xzf mosquitto_backup_20240118.tar.gz
```

## MQTT Topics

### Device Topics
```
esp/<device_id>/data          # Sensor readings (publish)
esp/<device_id>/control       # Device commands (subscribe)
esp/<device_id>/status        # Online/offline status
```

### Dashboard Topics
```
dashboard/<user_id>/command   # Control commands (publish)
dashboard/<user_id>/data      # Data updates (subscribe)
```

## Integration Scripts

### mqtt_to_db.py

Python script for bridging MQTT messages to Firebase Realtime Database.

**Location:** `smart_home_scripts/mqtt_to_db.py`

**Function:**
- Subscribes to MQTT topics
- Parses sensor data
- Writes to Firebase
- Maintains sync

**Usage:**
```bash
cd smart_home_scripts
python mqtt_to_db.py
```

**Configuration:**
```python
MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_USERNAME = "bridge_user"
MQTT_PASSWORD = "bridge_password"
FIREBASE_CREDENTIALS = "firebase-credentials.json"
```

## Testing

### Publish Test Message
```bash
mosquitto_pub -h localhost -p 1883 \
  -u username -P password \
  -t "test/topic" \
  -m "Hello MQTT"
```

### Subscribe to Topic
```bash
mosquitto_sub -h localhost -p 1883 \
  -u username -P password \
  -t "esp/#" \
  -v
```

### WebSocket Test
```bash
# Install wscat
npm install -g wscat

# Connect to WebSocket
wscat -c ws://localhost:8000
```

## Security

### TLS/SSL Configuration

**Certificate Setup:**
```bash
# Generate self-signed certificate
openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365 -nodes

# Add to mosquitto.conf
listener 8081
protocol websockets
certfile /path/to/cert.pem
keyfile /path/to/key.pem
```

### Access Control Lists (ACL)

**acl.conf:**
```
# User: esp_device
user esp_device
topic write esp/+/data
topic read esp/+/control

# User: web_client
user web_client
topic read esp/#
topic write dashboard/+/command
```

**Enable in mosquitto.conf:**
```
acl_file config/acl.conf
```

## Monitoring

### Connection Stats
```bash
# View active connections
mosquitto_sub -h localhost -p 1883 -u admin -P password -t '$SYS/broker/clients/active' -v

# View message rate
mosquitto_sub -h localhost -p 1883 -u admin -P password -t '$SYS/broker/messages/received' -v
```

### System Topics
```
$SYS/broker/clients/connected
$SYS/broker/clients/active
$SYS/broker/messages/received
$SYS/broker/messages/sent
$SYS/broker/uptime
```

## Performance

- Max connections: 1000+ (default)
- Message throughput: 10,000+ msg/sec
- Latency: <10ms (local network)
- Memory usage: ~50MB idle
- CPU usage: <5% normal load

## Troubleshooting

### Connection Refused
```bash
# Check if broker is running
sudo systemctl status mosquitto

# Check port binding
sudo netstat -tulpn | grep mosquitto

# Check firewall
sudo ufw allow 1883
sudo ufw allow 8000
sudo ufw allow 8081
```

### Authentication Failed
```bash
# Verify password file
cat config/auth/passwd.txt

# Regenerate password
mosquitto_passwd -b config/auth/passwd.txt username newpassword

# Restart broker
sudo systemctl restart mosquitto
```

### WebSocket Issues
```bash
# Test WebSocket connection
wscat -c ws://localhost:8000

# Check mosquitto.conf
protocol websockets  # Must be set for WebSocket listeners
```

## Maintenance

### Rotate Logs
```bash
# Add to crontab
0 0 * * 0 gzip log/mosquitto.log && > log/mosquitto.log
```

### Clean Persistent Data
```bash
# Stop broker
sudo systemctl stop mosquitto

# Clean old data
rm -rf data/*

# Restart
sudo systemctl start mosquitto
```

### Update Configuration
```bash
# Edit configuration
nano mosquitto.conf

# Validate syntax
mosquitto -c mosquitto.conf -v

# Reload (without restart)
sudo systemctl reload mosquitto
```

## Migration

### From Cloud to Local
1. Update device MQTT settings
2. Point to local broker IP
3. Update web dashboard settings
4. Migrate authentication credentials

### Scaling
- Use clustered Mosquitto instances
- Implement load balancer
- Shared persistent storage
- Redis for session state

## Dependencies

- Mosquitto 2.0+
- Python 3.8+ (for integration scripts)
- Firebase Admin SDK (for mqtt_to_db.py)
- OpenSSL (for TLS certificates)

## Related Documentation

- [ESP32 Firmware](../esp/README.md) - Device MQTT client
- [Web Dashboard](../web/README.md) - Dashboard MQTT client
- [Mobile App](../app/README.md) - Mobile MQTT client
- [MQTT Commands](../documents/MQTT_COMMANDS.md) - Topic structure
