/**
 * mqtt-handlers.js
 * MQTT message handling module
 * Processes incoming MQTT messages and updates UI accordingly
 */

import { MQTT_TOPICS, handleCommandResponse } from './mqtt-client.js';
import { updateDeviceCard, setDeviceOnlineStatus, canMqttSetOnline } from '../devices/device-card.js';
import { updateStatusBadge } from '../ui/ui-helpers.js';
import { escapeHtml, formatInterval } from '../utils/helpers.js';
import {
    syncSensorDataToFirebase,
    syncStateToFirebase,
    syncInfoToFirebase
} from './mqtt-to-firebase.js';

/* Cache for MQTT states */
const mqttStateCache = {};

/**
 * Get cached MQTT state for a device
 * @param {string} deviceId - Device identifier
 * @returns {Object|null} Cached state object { fan, light, ac, mode, interval, timestamp }
 */
export function getMQTTCachedState(deviceId) {
    return mqttStateCache[deviceId] || null;
}

/**
 * Get all cached MQTT states
 * @returns {Object} All cached states
 */
export function getAllMQTTCachedStates() {
    return { ...mqttStateCache };
}

/**
 * Handle incoming MQTT message
 * @param {Object} message - MQTT message object
 */
export function handleMQTTMessage(message) {
    try {
        const topic = message.destinationName;
        const payload = JSON.parse(message.payloadString);

        console.log('[MQTT] Received:', { topic, payload });

        // Parse topic: SmartHome/{deviceId}/{type}
        const topicParts = topic.split('/');
        if (topicParts.length < 3) {
            console.warn('[MQTT] Invalid topic format:', topic);
            return;
        }

        const deviceId = topicParts[1];
        const topicType = topicParts[2];
        
        console.log('[MQTT] Parsed:', { deviceId, topicType, expected_RESPONSE: MQTT_TOPICS.RESPONSE });

        // Route to appropriate handler
        switch (topicType) {
            case MQTT_TOPICS.DATA:
                handleDataMessage(deviceId, payload);
                break;
            case MQTT_TOPICS.STATE:
                handleStateMessage(deviceId, payload);
                break;
            case MQTT_TOPICS.INFO:
                handleInfoMessage(deviceId, payload);
                break;
            case MQTT_TOPICS.RESPONSE:
                handleResponseMessage(deviceId, payload);
                break;
            default:
                console.warn('[MQTT] Unknown topic type:', topicType);
        }
    } catch (error) {
        console.error('[MQTT] Message handling error:', error);
    }
}

/**
 * Handle sensor data message
 * @param {string} deviceId - Device identifier
 * @param {Object} data - Sensor data
 */
function handleDataMessage(deviceId, data) {
    console.log(`[MQTT] Data from ${deviceId}:`, data);
    
    // Mark device as online when receiving data (only if allowed)
    if (canMqttSetOnline()) {
        setDeviceOnlineStatus(deviceId, true);
    }

    // Update device card UI with real data from ESP32
    updateDeviceCard(deviceId, {
        temperature: parseFloat(data.temperature || 0).toFixed(2),
        humidity: parseFloat(data.humidity || 0).toFixed(2),
        light: parseInt(data.light || 0),
        timestamp: data.timestamp || Date.now()
    });

    // Sync to Firebase
    syncSensorDataToFirebase(deviceId, {
        temperature: parseFloat(data.temperature || 0),
        humidity: parseFloat(data.humidity || 0),
        light: parseInt(data.light || 0),
        timestamp: data.timestamp || Date.now()
    });
}

/**
 * Handle device state message
 * @param {string} deviceId - Device identifier
 * @param {Object} state - Device state
 */
function handleStateMessage(deviceId, state) {
    console.log(`[MQTT] State from ${deviceId}:`, state);
    
    // Mark device as online when receiving state (only if allowed)
    if (canMqttSetOnline()) {
        setDeviceOnlineStatus(deviceId, true);
    }

    /* Cache the state */
    mqttStateCache[deviceId] = {
        fan: state.fan,           // 0 = OFF, 1 = ON
        light: state.light,       // 0 = OFF, 1 = ON  
        ac: state.ac,             // 0 = OFF, 1 = ON
        mode: state.mode,         // 0 = OFF, 1 = ON (power)
        interval: state.interval,
        timestamp: state.timestamp || Date.now(),
        receivedAt: Date.now()    // Time received from MQTT
    };
    console.log(`[MQTT] Cached state for ${deviceId}:`, mqttStateCache[deviceId]);

    // mode: 0 = OFF, 1 = ON
    const isPowerOn = state.mode === 1;

    // Update power button in device card
    updateDevicePowerButton(deviceId, isPowerOn);

    // Update interval if available
    if (state.interval !== undefined) {
        updateDeviceInterval(deviceId, state.interval);
    }

    // Sync to Firebase
    syncStateToFirebase(deviceId, state);
}

/**
 * Handle command response message
 * @param {string} deviceId - Device identifier
 * @param {Object} response - Response data { cmd_id, status }
 */
function handleResponseMessage(deviceId, response) {
    console.log(`[MQTT] Response from ${deviceId}:`, response);
    
    const { cmd_id, status } = response;
    
    console.log('[MQTT] Extracted:', { cmd_id, status });
    
    if (!cmd_id) {
        console.warn('[MQTT] Response missing cmd_id');
        return;
    }
    
    console.log(`[MQTT] Calling handleCommandResponse('${cmd_id}', '${status}')`);
    
    // Route to command callback handler
    handleCommandResponse(cmd_id, status);
}

/**
 * Handle device info message
 * @param {string} deviceId - Device identifier
 * @param {Object} info - Device info
 */
function handleInfoMessage(deviceId, info) {
    console.log(`[MQTT] Info from ${deviceId}:`, info);
    
    // Mark device as online when receiving info (only if allowed)
    if (canMqttSetOnline()) {
        setDeviceOnlineStatus(deviceId, true);
    }

    // Update WiFi name (ssid)
    if (info.ssid) {
        updateDeviceWiFiName(deviceId, info.ssid);
    }

    // Update device info if needed
    if (info.ip || info.firmware || info.uptime) {
        updateDeviceCard(deviceId, {
            ip: info.ip,
            firmware: info.firmware,
            uptime: info.uptime
        });
    }

    // Sync to Firebase
    syncInfoToFirebase(deviceId, info);
}

/**
 * Update device WiFi name display
 * @param {string} deviceId - Device identifier
 * @param {string} ssid - WiFi SSID
 */
function updateDeviceWiFiName(deviceId, ssid) {
    const card = document.getElementById(`device-${deviceId}`);
    if (!card) return;

    const wifiEl = card.querySelector('.fa-wifi')?.parentElement;
    if (wifiEl) {
        wifiEl.innerHTML = `<i class="fa-solid fa-wifi"></i> ${escapeHtml(ssid)}`;
    }
}

/**
 * Update device power button state
 * @param {string} deviceId - Device identifier
 * @param {boolean} isPowerOn - Power state (true = ON, false = OFF)
 */
function updateDevicePowerButton(deviceId, isPowerOn) {
    const card = document.getElementById(`device-${deviceId}`);
    if (!card) return;

    const powerBtn = card.querySelector('button[onclick*="toggleDevicePower"]');
    if (!powerBtn) return;

    // Update class and text
    if (isPowerOn) {
        powerBtn.classList.remove('btn-danger');
        powerBtn.classList.add('btn-success');
        powerBtn.innerHTML = '<i class="fa-solid fa-power-off"></i> OFF'; // ON -> show OFF button (green)
        powerBtn.setAttribute('onclick', `window.toggleDevicePower('${deviceId}', false)`);
    } else {
        powerBtn.classList.remove('btn-success');
        powerBtn.classList.add('btn-danger');
        powerBtn.innerHTML = '<i class="fa-solid fa-power-off"></i> ON'; // OFF -> show ON button (red)
        powerBtn.setAttribute('onclick', `window.toggleDevicePower('${deviceId}', true)`);
    }
}

/**
 * Update device interval display
 * @param {string} deviceId - Device identifier
 * @param {number} interval - Interval in seconds
 */
function updateDeviceInterval(deviceId, interval) {
    const card = document.getElementById(`device-${deviceId}`);
    if (!card) return;

    const intervalEl = card.querySelector(`#${deviceId}-interval`);
    if (!intervalEl) return;

    intervalEl.textContent = formatInterval(interval);
}

/**
 * Handle MQTT connection success
 * @param {Function} onDeviceList - Callback to get device list for subscription
 */
export function handleMQTTConnect(onDeviceList) {
    console.log('[MQTT] Connection established');
    updateStatusBadge('mqtt-status', 'success', 'MQTT: Connected');

    // Subscribe to all devices
    const devices = onDeviceList();
    console.log(`[MQTT] Subscribing to ${devices.length} devices`);
}

/**
 * Handle MQTT connection lost
 * @param {Object} response - Connection lost response
 */
export function handleMQTTConnectionLost(response) {
    console.error('[MQTT] Connection lost:', response.errorMessage);
    updateStatusBadge('mqtt-status', 'error', 'MQTT: Disconnected');
}
