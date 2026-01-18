/**
 * mqtt-handlers.js
 * MQTT message handling module
 * Processes incoming MQTT messages and updates UI accordingly
 */

import { MQTT_TOPICS, handleCommandResponse } from './mqtt-client.js';
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

        console.log('[MQTT] Received:', { topic, payload });

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
    
    // DO NOT set online here - ping service controls online/offline status
    // Receiving messages doesn't mean device is responding to commands

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
    
    // DO NOT set online here - ping service controls online/offline status
    // State messages are passive updates, not command responses

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
    // Note: Toggle UI updates are now handled by command response callbacks in main.js
    // State messages provide real-time sync for external changes (e.g., physical button presses)
    updateQuickControlToggles(deviceId, state);

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
    
    // DO NOT set online here - ping service is the single source of truth
    // Info messages are just metadata updates

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

    // Update class and text - show current state
    if (isPowerOn) {
        powerBtn.classList.remove('btn-danger');
        powerBtn.classList.add('btn-success');
        powerBtn.innerHTML = '<i class="fa-solid fa-power-off"></i> ON';
        powerBtn.setAttribute('onclick', `window.toggleDevicePower('${deviceId}', false)`);
    } else {
        powerBtn.classList.remove('btn-success');
        powerBtn.classList.add('btn-danger');
        powerBtn.innerHTML = '<i class="fa-solid fa-power-off"></i> OFF';
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

/**
 * Update Quick Control toggle UI from state message
 * Used to sync UI when state changes from external sources (e.g., physical button)
 * @param {string} deviceId - Device identifier
 * @param {Object} state - Device state { fan, light, ac, mode }
 */
function updateQuickControlToggles(deviceId, state) {
    // Check if Quick Control modal is open for this device
    const modalEl = document.getElementById('quickControlModal');
    if (!modalEl) return;
    
    const modalDeviceId = modalEl.dataset.deviceId;
    if (modalDeviceId !== deviceId) return;

    // Map state keys to feature names
    const stateToFeature = {
        fan: 'fan',
        light: 'lamp',  // light state maps to lamp feature
        ac: 'ac'
    };

    // Update each toggle if state is available
    for (const [stateKey, featureName] of Object.entries(stateToFeature)) {
        if (state[stateKey] !== undefined) {
            const toggle = document.getElementById(`${featureName}-toggle`);
            if (toggle) {
                const isOn = state[stateKey] === 1;
                toggle.checked = isOn;
                
                // Update label text
                const label = toggle.parentElement?.querySelector('.toggle-status');
                if (label) {
                    label.textContent = isOn ? 'ON' : 'OFF';
                }
                
                console.log(`[MQTT] Updated ${featureName} toggle for ${deviceId}: ${isOn ? 'ON' : 'OFF'}`);
            }
        }
    }
}
