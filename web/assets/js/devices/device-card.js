/**
 * device-card.js
 * Device card UI rendering module
 * Handles rendering and updating device cards in the grid
 */

import { sendMQTTCommand } from '../mqtt/mqtt-client.js';

// Store device data for updates
let devicesData = {};

/**
 * Escape HTML to prevent XSS attacks
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
                <div class="card-wifi-info" id="${deviceId}-wifi">
                    <i class="fa-solid fa-wifi"></i> ${escapeHtml(wifiName)}
                </div>
            </div>
        </div>
        
        <div class="metrics">
            <div class="metric-item">
                <div class="metric-label">Temperature</div>
                <div class="metric-value" id="${deviceId}-temp">${data.sensors?.temperature || 0}°C</div>
            </div>
            <div class="metric-item">
                <div class="metric-label">Humidity</div>
                <div class="metric-value" id="${deviceId}-humid">${data.sensors?.humidity || 0}%</div>
            </div>
            <div class="metric-item">
                <div class="metric-label">Light</div>
                <div class="metric-value" id="${deviceId}-light">${data.sensors?.light || 0} Lux</div>
            </div>
        </div>
        
        <div class="card-actions ${viewType === 'dashboard' ? 'single-button' : ''}">
            ${buttonsHTML}
        </div>
    `;

    return card;
}

/**
 * Format interval time to readable string
 * @param {number} seconds - Interval in seconds
 * @returns {string} Formatted interval (e.g., "5s", "5m", "1h30m")
 */
function formatInterval(seconds) {
    if (seconds < 60) {
        return `${seconds}s`;
    } else if (seconds < 3600) {
        const minutes = Math.floor(seconds / 60);
        const remainingSeconds = seconds % 60;
        return remainingSeconds > 0 ? `${minutes}m${remainingSeconds}s` : `${minutes}m`;
    } else {
        const hours = Math.floor(seconds / 3600);
        const minutes = Math.floor((seconds % 3600) / 60);
        return minutes > 0 ? `${hours}h${minutes}m` : `${hours}h`;
    }
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

    // Update UI
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
