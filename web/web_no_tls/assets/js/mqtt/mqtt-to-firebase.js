/**
 * mqtt-to-firebase.js
 * Synchronizes MQTT data to Firebase Realtime Database
 * Ensures data persistence and consistency between MQTT and Firebase
 */

import { db } from '../core/firebase-config.js';
import { ref, set, update, push } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js";

/**
 * Sync sensor data from MQTT to Firebase
 * @param {string} deviceId - Device identifier
 * @param {Object} sensorData - Sensor data from MQTT
 * @param {number} sensorData.temperature - Temperature value
 * @param {number} sensorData.humidity - Humidity value
 * @param {number} sensorData.light - Light value
 * @param {number} sensorData.timestamp - Data timestamp
 */
export async function syncSensorDataToFirebase(deviceId, sensorData) {
    if (!db) {
        console.error('[MQTT to Firebase] Database not initialized');
        return;
    }

    try {
        const timestamp = sensorData.timestamp || Date.now();

        // 1. Update current device sensors in /devices/{deviceId}/sensors
        const deviceSensorsRef = ref(db, `devices/${deviceId}/sensors`);
        await update(deviceSensorsRef, {
            temperature: parseFloat(sensorData.temperature || 0),
            humidity: parseFloat(sensorData.humidity || 0),
            light: parseInt(sensorData.light || 0),
            lastUpdate: timestamp
        });

        // 2. Store historical data in /history/{deviceId}/records
        const historyRef = ref(db, `history/${deviceId}/records`);
        const newRecordRef = push(historyRef);
        await set(newRecordRef, {
            temperature: parseFloat(sensorData.temperature || 0),
            humidity: parseFloat(sensorData.humidity || 0),
            light: parseInt(sensorData.light || 0),
            timestamp: timestamp
        });

        // 3. Update SmartHome/{deviceId}/data (mirror MQTT structure)
        const smartHomeDataRef = ref(db, `SmartHome/${deviceId}/data`);
        await set(smartHomeDataRef, {
            temperature: parseFloat(sensorData.temperature || 0),
            humidity: parseFloat(sensorData.humidity || 0),
            light: parseInt(sensorData.light || 0),
            timestamp: timestamp
        });

        console.log(`[MQTT to Firebase] Sensor data synced for ${deviceId}`);
    } catch (error) {
        console.error('[MQTT to Firebase] Error syncing sensor data:', error);
    }
}

/**
 * Sync device state from MQTT to Firebase
 * @param {string} deviceId - Device identifier
 * @param {Object} stateData - State data from MQTT
 * @param {number} stateData.mode - Device mode (0=off, 1=on)
 * @param {number} stateData.interval - Data reporting interval in seconds
 * @param {number} stateData.fan - Fan state (0=off, 1=on)
 * @param {number} stateData.lamp - Lamp state (0=off, 1=on)
 * @param {number} stateData.ac - AC state (0=off, 1=on)
 */
export async function syncStateToFirebase(deviceId, stateData) {
    if (!db) {
        console.error('[MQTT to Firebase] Database not initialized');
        return;
    }

    try {
        const timestamp = Date.now();

        // 1. Update device states in /devices/{deviceId}/states
        const deviceStatesRef = ref(db, `devices/${deviceId}/states`);
        await update(deviceStatesRef, {
            power: stateData.mode === 1, // Convert mode to boolean
            fan: stateData.fan === 1,
            lamp: stateData.light === 1, // MQTT send 'light', UI use 'lamp'
            ac: stateData.ac === 1,
            lastUpdate: timestamp
        });

        // 2. Update interval in /devices/{deviceId}/interval
        if (stateData.interval !== undefined) {
            const deviceRef = ref(db, `devices/${deviceId}`);
            await update(deviceRef, {
                interval: parseInt(stateData.interval || 5)
            });
        }

        // 3. Update SmartHome/{deviceId}/state (mirror MQTT structure)
        const smartHomeStateRef = ref(db, `SmartHome/${deviceId}/state`);
        await set(smartHomeStateRef, {
            mode: stateData.mode || 0,
            interval: parseInt(stateData.interval || 5),
            fan: stateData.fan || 0,
            light: stateData.light || 0, // Keep the name 'light' as in MQTT
            ac: stateData.ac || 0,
            timestamp: timestamp
        });

        console.log(`[MQTT to Firebase] State synced for ${deviceId}`);
    } catch (error) {
        console.error('[MQTT to Firebase] Error syncing state:', error);
    }
}

/**
 * Sync device info from MQTT to Firebase
 * @param {string} deviceId - Device identifier
 * @param {Object} infoData - Device info from MQTT
 * @param {string} infoData.ssid - WiFi SSID
 * @param {string} infoData.ip - Device IP address
 * @param {string} infoData.broker - MQTT broker address
 * @param {string} infoData.firmware - Firmware version
 * @param {number} infoData.uptime - Device uptime in seconds
 */
export async function syncInfoToFirebase(deviceId, infoData) {
    if (!db) {
        console.error('[MQTT to Firebase] Database not initialized');
        return;
    }

    try {
        const timestamp = Date.now();

        // 1. Update device info in /devices/{deviceId}
        const deviceRef = ref(db, `devices/${deviceId}`);
        const updates = {
            lastUpdate: timestamp
        };

        if (infoData.ssid) updates.wifi = infoData.ssid;
        if (infoData.ip) updates.ip = infoData.ip;
        if (infoData.firmware) updates.firmware = infoData.firmware;

        await update(deviceRef, updates);

        // 2. Update SmartHome/{deviceId}/info (mirror MQTT structure)
        const smartHomeInfoRef = ref(db, `SmartHome/${deviceId}/info`);
        await set(smartHomeInfoRef, {
            ssid: infoData.ssid || '',
            ip: infoData.ip || '',
            broker: infoData.broker || '',
            firmware: infoData.firmware || '',
            uptime: infoData.uptime || 0,
            timestamp: timestamp
        });

        console.log(`[MQTT to Firebase] Info synced for ${deviceId}`);
    } catch (error) {
        console.error('[MQTT to Firebase] Error syncing info:', error);
    }
}

/**
 * Initialize Firebase sync for a device (create initial structure if needed)
 * @param {string} deviceId - Device identifier
 * @param {string} deviceName - Device name
 */
export async function initializeDeviceInFirebase(deviceId, deviceName) {
    if (!db) {
        console.error('[MQTT to Firebase] Database not initialized');
        return;
    }

    try {
        const deviceRef = ref(db, `devices/${deviceId}`);
        const smartHomeRef = ref(db, `SmartHome/${deviceId}`);

        // Create default structure if device doesn't exist
        await update(deviceRef, {
            id: deviceId,
            name: deviceName,
            interval: 5,
            lastUpdate: Date.now()
        });

        console.log(`[MQTT to Firebase] Device ${deviceId} initialized`);
    } catch (error) {
        console.error('[MQTT to Firebase] Error initializing device:', error);
    }
}
