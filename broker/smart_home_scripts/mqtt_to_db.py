#!/usr/bin/env python3
''"""
Script: mqtt_to_db.py
"""''

import paho.mqtt.client as mqtt
from paho.mqtt.client import CallbackAPIVersion
import mysql.connector
import json
import time
import sys

# Configuration

# MQTT Broker
MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_TOPIC = "SmartHome/#"  # Subscribe to all SmartHome topics
MQTT_USERNAME = "SmartHome"  # MQTT username
MQTT_PASSWORD = "SmartHome01"  # MQTT password

# Database
DB_CONFIG = {
    'host': 'localhost',
    'user': 'SmartHome',
    'password': 'SmartHome01',
    'database': 'SMARTHOME',
    'autocommit': True
}

# DATABASE FUNCTIONS

def connect_db():
    """Create a connection to the database"""
    try:
        conn = mysql.connector.connect(**DB_CONFIG)
        return conn
    except mysql.connector.Error as err:
        print(f"Database connection error: {err}")
        return None

def save_device_info(conn, device_id, info):
    """Save/update device information"""
    try:
        cursor = conn.cursor()
        query = """
        INSERT INTO devices (device_id, ssid, ip_address, broker, firmware)
        VALUES (%s, %s, %s, %s, %s)
        ON DUPLICATE KEY UPDATE 
            ssid = VALUES(ssid),
            ip_address = VALUES(ip_address),
            broker = VALUES(broker),
            firmware = VALUES(firmware),
            last_update = CURRENT_TIMESTAMP
        """
        cursor.execute(query, (
            device_id,
            info.get('ssid'),
            info.get('ip'),
            info.get('broker'),
            info.get('firmware')
        ))
        cursor.close()
        print(f"Updated device information: {device_id}")
        return True
    except Exception as e:
        print(f"Error in save_device_info: {e}")
        return False

def save_sensor_data(conn, device_id, data):
    """Save sensor data"""
    try:
        cursor = conn.cursor()
        query = """
        INSERT INTO sensor_data (device_id, temperature, humidity, light, timestamp) 
        VALUES (%s, %s, %s, %s, %s)
        """
        cursor.execute(query, (
            device_id,
            data.get('temperature'),
            data.get('humidity'),
            data.get('light'),
            data.get('timestamp')
        ))
        cursor.close()
        print(f"Saved sensor_data: Temp={data.get('temperature')}°C, Humid={data.get('humidity')}%, Light={data.get('light')}")
        return True
    except Exception as e:
        print(f"Error in save_sensor_data: {e}")
        return False

def save_device_state(conn, device_id, state):
    """Save device state"""
    try:
        cursor = conn.cursor()
        query = """
        INSERT INTO device_states 
        (device_id, mode, fan_status, light_status, ac_status, interval_value, timestamp) 
        VALUES (%s, %s, %s, %s, %s, %s, %s)
        """
        cursor.execute(query, (
            device_id,
            state.get('mode'),
            state.get('fan'),
            state.get('light'),
            state.get('ac'),
            state.get('interval'),
            state.get('timestamp')
        ))
        cursor.close()
        print(f"Saved device_states: Mode={state.get('mode')}, Fan={state.get('fan')}, Light={state.get('light')}, AC={state.get('ac')}")
        return True
    except Exception as e:
        print(f"Error in save_device_state: {e}")
        return False

def save_command(conn, device_id, cmd):
    """Save command"""
    try:
        cursor = conn.cursor()
        query = """
        INSERT INTO commands (device_id, cmd_id, command, params)
        VALUES (%s, %s, %s, %s)
        """
        params_str = json.dumps(cmd.get('params', {}))
        cursor.execute(query, (
            device_id,
            cmd.get('id'),
            cmd.get('command'),
            params_str
        ))
        cursor.close()
        print(f"Saved command: {cmd.get('command')}")
        return True
    except Exception as e:
        print(f"Error in save_command: {e}")
        return False

def update_command_response(conn, device_id, response):
    """Update command execution result"""
    try:
        cursor = conn.cursor()
        query = """
        UPDATE commands 
        SET status = %s 
        WHERE device_id = %s AND cmd_id = %s
        ORDER BY created_at DESC
        LIMIT 1
        """
        cursor.execute(query, (
            response.get('status'),
            device_id,
            response.get('cmd_id')
        ))
        cursor.close()
        print(f"Updated command response: {response.get('cmd_id')} → {response.get('status')}")
        return True
    except Exception as e:
        print(f"Error in update_command_response: {e}")
        return False

# MQTT CALLBACKS

def on_connect(client, userdata, flags, rc, properties=None):
    """Callback when MQTT connection is successful"""
    if rc == 0:
        print("=" * 60)
        print("MQTT BROKER CONNECTION SUCCESSFUL!")
        print("=" * 60)
        client.subscribe(MQTT_TOPIC)
        print(f"Subscribed to topic: {MQTT_TOPIC}")
        print("Waiting for data from devices...")
        print("-" * 60)
    else:
        print(f"MQTT connection error. Error code: {rc}")
        sys.exit(1)

def on_message(client, userdata, msg):
    """Callback when receiving message from MQTT"""
    topic = msg.topic
    payload = msg.payload.decode('utf-8')
    
    # Display message information
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
    print(f"\n[{timestamp}] Received new message:")
    print(f"  Topic: {topic}")
    print(f"  Payload: {payload[:150]}{'...' if len(payload) > 150 else ''}")
    
    try:
        # Parse JSON
        data = json.loads(payload)
        
        # Extract device_id and message_type from topic
        # Example: SmartHome/esp_02/data → device_id = esp_02, message_type = data
        parts = topic.split('/')
        if len(parts) < 3:
            print(f"Invalid topic, skipping")
            return
        
        device_id = parts[1]
        message_type = parts[2]
        
        print(f"  Device: {device_id}")
        print(f"  Type: {message_type}")
        
        # Connect to database
        db = connect_db()
        if not db:
            print(f"Cannot connect to database")
            return
        
        # Process based on message type
        success = False
        
        if message_type == "info":
            success = save_device_info(db, device_id, data)
        
        elif message_type == "data":
            success = save_sensor_data(db, device_id, data)
        
        elif message_type == "state":
            success = save_device_state(db, device_id, data)
        
        elif message_type == "command":
            success = save_command(db, device_id, data)
        
        elif message_type == "response":
            success = update_command_response(db, device_id, data)
        
        else:
            print(f"Unknown message type: {message_type}")
        
        # Close database connection
        db.close()
        
        if success:
            print(f"Processing successful!")
        
    except json.JSONDecodeError:
        print(f"Payload is not valid JSON")
    except Exception as e:
        print(f"Error processing message: {e}")
        import traceback
        traceback.print_exc()

def on_disconnect(client, userdata, rc, properties=None):
    """Callback when MQTT connection is lost"""
    if rc != 0:
        print(f"\nMQTT connection lost! Trying to reconnect...")

# MAIN

def main():
    """Main function"""
    print("=" * 60)
    print("SMART HOME - MQTT TO DATABASE LOGGER")
    print("=" * 60)
    print(f"MQTT Broker: {MQTT_BROKER}:{MQTT_PORT}")
    print(f"Database: {DB_CONFIG['database']}@{DB_CONFIG['host']}")
    print(f"Subscribed Topic: {MQTT_TOPIC}")
    print("=" * 60)
    
    # Check database connection
    print("\nChecking database connection...")
    db = connect_db()
    if db:
        print("Database OK")
        db.close()
    else:
        print("Cannot connect to database. Check configuration!")
        sys.exit(1)
    
    # Initialize MQTT client
    client = mqtt.Client(CallbackAPIVersion.VERSION2)
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_disconnect = on_disconnect
    
    # Connect to MQTT
    try:
        print("\nConnecting to MQTT Broker...")
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_forever()
    
    except KeyboardInterrupt:
        print("\n\nUser stopped the program (Ctrl+C)")
        client.disconnect()
        print("Goodbye!")
    
    except Exception as e:
        print(f"\nError: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()