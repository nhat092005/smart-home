/**
 * device-card.js
 * Device card UI rendering module
 * Handles rendering and updating device cards in the grid
 */

import { sendMQTTCommand } from '../mqtt/mqtt-client.js';
import { updateSensorDisplay } from '../charts/chart-manager.js';
import { escapeHtml, formatInterval } from '../utils/helpers.js';

// Store device data for updates
let devicesData = {};

// Store device online status
const deviceOnlineStatus = {};

// Flag to control if MQTT messages can set online status
let allowMqttSetOnline = false;

/**
 * Enable MQTT to set online status (after initial ping)
 */
export function enableMqttOnlineControl() {
    allowMqttSetOnline = true;
    console.log('[DeviceCard] MQTT online control enabled');
}

/**
 * Disable MQTT online control (during initial ping)
 */
export function disableMqttOnlineControl() {
    allowMqttSetOnline = false;
    console.log('[DeviceCard] MQTT online control disabled');
}

/**
 * Check if MQTT can set online status
 * @returns {boolean} Allow status
 */
export function canMqttSetOnline() {
    return allowMqttSetOnline;
}

/**
 * Initialize all devices as offline
 * @param {Array} deviceIds - Array of device identifiers
 */
export function initializeDevicesOffline(deviceIds) {
    deviceIds.forEach(deviceId => {
        deviceOnlineStatus[deviceId] = false;
        updateDeviceStatusUI(deviceId, false);
    });
    console.log(`[DeviceCard] Initialized ${deviceIds.length} devices as offline`);
}

/**
 * Get device online status
 * @param {string} deviceId - Device identifier
 * @returns {boolean} Online status
 */
export function isDeviceOnline(deviceId) {
    return deviceOnlineStatus[deviceId] || false;
}

/**
 * Set device online status
 * @param {string} deviceId - Device identifier
 * @param {boolean} isOnline - Online status
 */
export function setDeviceOnlineStatus(deviceId, isOnline) {
    const wasOnline = deviceOnlineStatus[deviceId];
    deviceOnlineStatus[deviceId] = isOnline;
    
    // Update UI if status changed
    if (wasOnline !== isOnline) {
        updateDeviceStatusUI(deviceId, isOnline);
    }
}

/**
 * Update device status indicator UI
 * @param {string} deviceId - Device identifier
 * @param {boolean} isOnline - Online status
 */
function updateDeviceStatusUI(deviceId, isOnline) {
    const statusEl = document.getElementById(`${deviceId}-status`);
    if (!statusEl) return;
    
    if (isOnline) {
        statusEl.className = 'device-status online';
        statusEl.innerHTML = '<span class="status-dot"></span><span class="status-text">Online</span>';
        
        // Restore sensor display if we have data
        if (devicesData[deviceId]?.sensors) {
            const sensors = devicesData[deviceId].sensors;
            updateDeviceCard(deviceId, {
                temperature: sensors.temperature,
                humidity: sensors.humidity,
                light: sensors.light
            });
        }
        
        // Restore WiFi name
        const wifiEl = document.getElementById(`${deviceId}-wifi`);
        if (wifiEl && devicesData[deviceId]?.wifi) {
            wifiEl.innerHTML = `<i class="fa-solid fa-wifi"></i> ${escapeHtml(devicesData[deviceId].wifi)}`;
        }
    } else {
        statusEl.className = 'device-status offline';
        statusEl.innerHTML = '<span class="status-dot"></span><span class="status-text">Offline</span>';
        
        // Show offline values
        const tempEl = document.getElementById(`${deviceId}-temp`);
        const humidEl = document.getElementById(`${deviceId}-humid`);
        const lightEl = document.getElementById(`${deviceId}-light`);
        const wifiEl = document.getElementById(`${deviceId}-wifi`);
        
        if (tempEl) tempEl.textContent = '--°C';
        if (humidEl) humidEl.textContent = '--%';
        if (lightEl) lightEl.textContent = '-- Lux';
        if (wifiEl) wifiEl.innerHTML = '<i class="fa-solid fa-wifi"></i> N/A';
    }
}

/**
 * Get all device online statuses
 * @returns {Object} All device online statuses
 */
export function getAllDeviceOnlineStatus() {
    return { ...deviceOnlineStatus };
}

/**
 * Render device grid with all devices
 * @param {Object} devices - Devices object from Firebase
 * @param {string} viewType - View type: 'manage' or 'dashboard'
 */
export function renderDeviceGrid(devices, viewType = 'manage') {
    devicesData = devices;
    const grid = document.getElementById('device-grid');

    if (!grid) {
        console.error('[DeviceCard] Grid container not found');
        return;
    }

    // Clear grid
    grid.innerHTML = '';

    // Render device cards first
    Object.entries(devices).forEach(([deviceId, deviceData]) => {
        const card = createDeviceCard(deviceId, deviceData, viewType);
        grid.appendChild(card);
    });

    // Add "Add Device" button at the end (right side) for manage view
    if (viewType === 'manage') {
        const addButton = document.createElement('div');
        addButton.className = 'card add-card';
        addButton.id = 'btn-open-modal';
        addButton.setAttribute('role', 'button');
        addButton.setAttribute('tabindex', '0');
        addButton.innerHTML = `
            <div class="dashed-border">
                <i class="fa-solid fa-plus" aria-hidden="true"></i>
                <span>Add Device</span>
            </div>
        `;
        grid.appendChild(addButton);
    }
}

/**
 * Create device card element
 * @param {string} deviceId - Device identifier
 * @param {Object} data - Device data
 * @param {string} viewType - View type: 'manage' or 'dashboard'
 * @returns {HTMLElement} Card element
 */
function createDeviceCard(deviceId, data, viewType = 'manage') {
    const card = document.createElement('div');
    card.className = 'card';
    card.id = `device-${deviceId}`;

    const wifiName = data.wifi || 'N/A';
    const interval = data.interval || 5; // Default to 5 seconds

    // Format interval: 5s, 5m, 1h30m
    const intervalText = formatInterval(interval);

    // Check power state (default is OFF if no data)
    const isPowerOn = data.states?.power || false;
    
    // Check online status (default is offline until ping succeeds)
    const isOnline = deviceOnlineStatus[deviceId] || false;
    const statusClass = isOnline ? 'online' : 'offline';
    const statusText = isOnline ? 'Online' : 'Offline';
    
    // Display values based on online status
    const tempDisplay = isOnline ? `${data.sensors?.temperature || 0}°C` : '--°C';
    const humidDisplay = isOnline ? `${data.sensors?.humidity || 0}%` : '--%';
    const lightDisplay = isOnline ? `${data.sensors?.light || 0} Lux` : '-- Lux';
    const wifiDisplay = isOnline ? escapeHtml(wifiName) : 'N/A';

    // Buttons based on view type
    let buttonsHTML = '';
    if (viewType === 'manage') {
        // Manage: Power On/Off + Edit
        // OFF = red (btn-danger), ON = green (btn-success)
        const powerBtnClass = isPowerOn ? 'btn-success' : 'btn-danger';
        const powerBtnText = isPowerOn ? 'ON' : 'OFF'; // Green = ON -> show "OFF", Red = OFF -> show "ON"
        const powerIcon = 'fa-power-off';

        buttonsHTML = `
            <button class="btn-sm ${powerBtnClass}" onclick="window.toggleDevicePower('${deviceId}', ${!isPowerOn})">
                <i class="fa-solid ${powerIcon}"></i> ${powerBtnText}
            </button>
            <button class="btn-sm" onclick="window.openEditModal('${deviceId}')">
                <i class="fa-solid fa-pen"></i> Edit
            </button>
        `;
    } else {
        // Dashboard: Only "Details" button
        buttonsHTML = `
            <button class="btn-sm btn-measure" onclick="window.openReportDetail('${deviceId}')">
                <i class="fa-solid fa-chart-line"></i> Details
            </button>
        `;
    }

    card.innerHTML = `
        <div class="card-header">
            <div>
                <div class="card-title">${escapeHtml(data.name)}</div>
                <div class="card-header-left-sub">
                    <span class="device-id">${escapeHtml(deviceId)}</span>
                    <span class="interval-badge" id="${deviceId}-interval">${intervalText}</span>
                </div>
            </div>
            <div class="card-header-right">
                <div class="device-status ${statusClass}" id="${deviceId}-status">
                    <span class="status-dot"></span>
                    <span class="status-text">${statusText}</span>
                </div>
                <div class="card-wifi-info" id="${deviceId}-wifi">
                    <i class="fa-solid fa-wifi"></i> ${wifiDisplay}
                </div>
            </div>
        </div>
        
        <div class="metrics">
            <div class="metric-item">
                <div class="metric-label">Temperature</div>
                <div class="metric-value" id="${deviceId}-temp">${tempDisplay}</div>
            </div>
            <div class="metric-item">
                <div class="metric-label">Humidity</div>
                <div class="metric-value" id="${deviceId}-humid">${humidDisplay}</div>
            </div>
            <div class="metric-item">
                <div class="metric-label">Light</div>
                <div class="metric-value" id="${deviceId}-light">${lightDisplay}</div>
            </div>
        </div>
        
        <div class="card-actions ${viewType === 'dashboard' ? 'single-button' : ''}">
            ${buttonsHTML}
        </div>
    `;

    return card;
}

/**
 * Update device card with new data
 * @param {string} deviceId - Device identifier
 * @param {Object} updates - Data to update
 */
export function updateDeviceCard(deviceId, updates) {
    // Update stored data
    if (devicesData[deviceId]) {
        if (updates.temperature !== undefined) {
            devicesData[deviceId].sensors = devicesData[deviceId].sensors || {};
            devicesData[deviceId].sensors.temperature = updates.temperature;
        }
        if (updates.humidity !== undefined) {
            devicesData[deviceId].sensors = devicesData[deviceId].sensors || {};
            devicesData[deviceId].sensors.humidity = updates.humidity;
        }
        if (updates.light !== undefined) {
            devicesData[deviceId].sensors = devicesData[deviceId].sensors || {};
            devicesData[deviceId].sensors.light = updates.light;
        }

        devicesData[deviceId].lastUpdate = Date.now();
    }

    // Update device card UI
    const tempEl = document.getElementById(`${deviceId}-temp`);
    const humidEl = document.getElementById(`${deviceId}-humid`);
    const lightEl = document.getElementById(`${deviceId}-light`);

    if (tempEl && updates.temperature !== undefined) {
        tempEl.textContent = `${updates.temperature}°C`;
    }
    if (humidEl && updates.humidity !== undefined) {
        humidEl.textContent = `${updates.humidity}%`;
    }
    if (lightEl && updates.light !== undefined) {
        lightEl.textContent = `${updates.light} Lux`;
    }

    // Update popup sensor display (if popup is open for this device)
    updateSensorDisplay(deviceId, updates);
}

/**
 * Toggle device feature (fan, lamp, ac)
 * @param {string} deviceId - Device identifier
 * @param {string} feature - Feature name (fan/lamp/ac)
 * @param {boolean} state - New state
 */
export function toggleDeviceFeature(deviceId, feature, state) {
    const featureConfig = {
        fan: { command: 'set_fan', param: 'fan' },
        lamp: { command: 'set_lamp', param: 'lamp' },
        ac: { command: 'set_ac', param: 'ac' }
    };

    const config = featureConfig[feature];
    if (!config) {
        console.error('[DeviceCard] Unknown feature:', feature);
        return;
    }

    const params = {};
    params[config.param] = state;

    sendMQTTCommand(deviceId, config.command, params);
}

/**
 * Get current device data
 * @param {string} deviceId - Device identifier
 * @returns {Object} Device data
 */
export function getDeviceData(deviceId) {
    return devicesData[deviceId] || null;
}

/**
 * Get all devices data
 * @returns {Object} All devices data
 */
export function getAllDevicesData() {
    return devicesData;
}
