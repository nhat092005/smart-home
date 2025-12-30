/**
 * mqtt-handlers.js
 * MQTT message handling module
 * Processes incoming MQTT messages and updates UI accordingly
 */

import { MQTT_TOPICS } from './mqtt-client.js';
import { updateDeviceCard } from '../devices/device-card.js';
import { updateStatusBadge } from '../ui/ui-helpers.js';
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

        // Parse topic: SmartHome/{deviceId}/{type}
        const topicParts = topic.split('/');
        if (topicParts.length < 3) {
            console.warn('[MQTT] Invalid topic format:', topic);
            return;
        }

        const deviceId = topicParts[1];
        const topicType = topicParts[2];

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

    // Update Quick Control toggles if modal is open for this device
    if (state.fan !== undefined && window.updateToggleUI) {
        window.updateToggleUI(deviceId, 'fan', state.fan);
        window.clearPendingToggle?.(deviceId, 'fan');
    }
    if (state.light !== undefined && window.updateToggleUI) {
        window.updateToggleUI(deviceId, 'lamp', state.light); // lamp maps to light
        window.clearPendingToggle?.(deviceId, 'lamp');
    }
    if (state.ac !== undefined && window.updateToggleUI) {
        window.updateToggleUI(deviceId, 'ac', state.ac);
        window.clearPendingToggle?.(deviceId, 'ac');
    }

    // Sync to Firebase
    syncStateToFirebase(deviceId, state);
}

/**
 * Handle device info message
 * @param {string} deviceId - Device identifier
 * @param {Object} info - Device info
 */
function handleInfoMessage(deviceId, info) {
    console.log(`[MQTT] Info from ${deviceId}:`, info);

    // Cập nhật tên WiFi (ssid)
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
 * Escape HTML to prevent XSS
 * @param {string} unsafe - Unsafe string
 * @returns {string} Escaped string
 */
function escapeHtml(unsafe) {
    if (!unsafe) return '';
    return unsafe
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#039;");
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

    // Cập nhật class và text
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

    // Format interval: 5s, 5m, 1h30m
    let intervalText;
    if (interval < 60) {
        intervalText = `${interval}s`;
    } else if (interval < 3600) {
        const minutes = Math.floor(interval / 60);
        const remainingSeconds = interval % 60;
        intervalText = remainingSeconds > 0 ? `${minutes}m${remainingSeconds}s` : `${minutes}m`;
    } else {
        const hours = Math.floor(interval / 3600);
        const minutes = Math.floor((interval % 3600) / 60);
        intervalText = minutes > 0 ? `${hours}h${minutes}m` : `${hours}h`;
    }

    intervalEl.textContent = intervalText;
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
