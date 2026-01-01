/**
 * settings-manager.js
 * Settings management module
 * Handles device reboot, time sync, and WiFi setup guide
 */

import { db } from '../core/firebase-config.js';
import { ref, get } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js";
import { sendMQTTCommand, isMQTTConnected } from '../mqtt/mqtt-client.js';
import { updateClock, startClock, stopClock } from '../ui/ui-helpers.js';
import { isDeviceOnline } from '../devices/device-card.js';
import { escapeHtml } from '../utils/helpers.js';

// ESP32 Access Point configuration
const ESP32_AP_CONFIG = {
    ip: '192.168.4.1',
    ssid: 'SmartHome',
    password: '12345678'
};

/**
 * Initialize settings page
 */
export function initializeSettings() {
    // Start clock display
    startClock();

    // Load device info table
    loadDeviceInfoTable();
}

/**
 * Load device information table with action buttons
 */
export async function loadDeviceInfoTable() {
    const tableBody = document.getElementById('device-info-table');

    if (!tableBody || !db) {
        console.error('[Settings] Table element or Firebase not available');
        return;
    }

    try {
        const devicesRef = ref(db, 'devices');
        const snapshot = await get(devicesRef);

        if (!snapshot.exists()) {
            tableBody.innerHTML = `
                <tr>
                    <td colspan="7" class="table-empty-state">
                        <i class="fa-solid fa-inbox table-state-icon"></i>
                        No devices found
                    </td>
                </tr>
            `;
            return;
        }

        const devices = snapshot.val();
        const rows = Object.entries(devices).map(([deviceId, data], index) => {
            const name = data.name || 'Unknown';
            const online = isDeviceOnline(deviceId);
            const ip = online ? (data.ip || 'Not available') : 'Not connected';

            return `
                <tr data-device-id="${escapeHtml(deviceId)}">
                    <td class="table-cell-center">${index + 1}</td>
                    <td>${escapeHtml(name)}</td>
                    <td>${escapeHtml(deviceId)}</td>
                    <td class="device-ip-cell ${online ? '' : 'offline-ip'}">${escapeHtml(ip)}</td>
                    <td class="table-cell-center">
                        <button type="button" onclick="showTimeModalForDevice('${escapeHtml(deviceId)}')" class="btn-table btn-table-success">
                            <i class="fa-solid fa-clock"></i> Edit
                        </button>
                    </td>
                    <td class="table-cell-center">
                        <button type="button" onclick="showWiFiGuideForDevice('${escapeHtml(deviceId)}')" class="btn-table btn-table-primary">
                            <i class="fa-solid fa-wifi"></i> Configure
                        </button>
                    </td>
                    <td class="table-cell-center">
                        <button type="button" onclick="rebootDevice('${escapeHtml(deviceId)}')" class="btn-table btn-table-danger">
                            <i class="fa-solid fa-power-off"></i> Reboot
                        </button>
                    </td>
                </tr>
            `;
        }).join('');

        tableBody.innerHTML = rows;

    } catch (error) {
        console.error('[Settings] Load device info error:', error);
        tableBody.innerHTML = `
            <tr>
                <td colspan="7" class="table-error-state">
                    <i class="fa-solid fa-triangle-exclamation table-state-icon"></i>
                    Error loading devices: ${escapeHtml(error.message)}
                </td>
            </tr>
        `;
    }
}

/**
 * Reboot a specific device via MQTT
 * @param {string} deviceId - Device ID to reboot
 */
export function rebootDevice(deviceId) {
    // Check if device is online
    if (!isDeviceOnline(deviceId)) {
        alert(`Device "${deviceId}" is Offline.\n\nUnable to send reboot command.`);
        return;
    }
    
    const confirmMsg = `Are you sure you want to REBOOT the device "${deviceId}"?\n\nThe device will restart and lose connection for a few seconds.`;
    if (!confirm(confirmMsg)) return;

    if (!isMQTTConnected()) {
        alert("MQTT not connected! Cannot send reboot command.");
        return;
    }

    // Find and disable the reboot button
    const rebootBtn = document.querySelector(`button[onclick*="rebootDevice('${deviceId}')"]`);
    if (rebootBtn) {
        rebootBtn.disabled = true;
        rebootBtn.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i>';
    }

    sendMQTTCommand(deviceId, 'reboot', {}, {
        onSuccess: () => {
            if (rebootBtn) {
                rebootBtn.disabled = false;
                rebootBtn.innerHTML = '<i class="fa-solid fa-power-off"></i> Reboot';
            }
            alert(`Reboot command successful for device "${deviceId}"!\n\nThe device will restart now.`);
        },
        onError: (error) => {
            if (rebootBtn) {
                rebootBtn.disabled = false;
                rebootBtn.innerHTML = '<i class="fa-solid fa-power-off"></i> Reboot';
            }
            const errorMsg = error === 'timeout' ? 'Device not responding' : 'Device error';
            alert(`${errorMsg}: Unable to reboot device "${deviceId}".`);
        },
        timeout: 5000
    });
}

/**
 * Show WiFi guide modal for specific device
 * @param {string} deviceId - Device ID
 */
export function showWiFiGuideForDevice(deviceId) {
    // Check if device is online
    if (!isDeviceOnline(deviceId)) {
        alert(`Device "${deviceId}" is Offline.\n\nUnable to configure WiFi.`);
        return;
    }
    
    // Show confirm dialog before proceeding
    const confirmMsg = `Are you sure you want to CONFIGURE WiFi for device "${deviceId}"?\n\nThe device will be reset to AP mode to configure new WiFi.`;
    if (!confirm(confirmMsg)) return;

    if (!isMQTTConnected()) {
        alert("MQTT not connected! Cannot send command.");
        return;
    }

    // Find and disable the configure button
    const configBtn = document.querySelector(`button[onclick*="showWiFiGuideForDevice('${deviceId}')"]`);
    if (configBtn) {
        configBtn.disabled = true;
        configBtn.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i>';
    }

    // Send factory_reset command with response callback
    sendMQTTCommand(deviceId, 'factory_reset', {}, {
        onSuccess: () => {
            if (configBtn) {
                configBtn.disabled = false;
                configBtn.innerHTML = '<i class="fa-solid fa-wifi"></i> Configure';
            }
            console.log(`[Settings] factory_reset successful for ${deviceId}`);

            // Show WiFi guide modal
            const modal = document.getElementById('wifi-guide-modal');
            const guideContent = document.getElementById('wifi-guide-content');
            if (modal && guideContent) {
                guideContent.innerHTML = createWiFiGuideHTML(deviceId);
                modal.style.display = 'block';
            }
        },
        onError: (error) => {
            if (configBtn) {
                configBtn.disabled = false;
                configBtn.innerHTML = '<i class="fa-solid fa-wifi"></i> Configure';
            }
            const errorMsg = error === 'timeout' ? 'Device not responding' : 'Device error';
            alert(`${errorMsg}: Unable to configure WiFi for device "${deviceId}".`);
        },
        timeout: 5000
    });
}

/**
 * Close WiFi guide modal
 */
export function closeWiFiGuideModal() {
    const modal = document.getElementById('wifi-guide-modal');
    if (modal) {
        modal.style.display = 'none';
    }
}

/**
 * Show time modal for specific device
 * @param {string} deviceId - Device ID
 */
export function showTimeModalForDevice(deviceId) {
    // Check if device is online
    if (!isDeviceOnline(deviceId)) {
        alert(`Device "${deviceId}" is Offline.\n\nUnable to synchronize time.`);
        return;
    }
    
    // Store current device ID for time sync
    window.currentTimeDeviceId = deviceId;

    const modal = document.getElementById('manual-time-modal');
    if (modal) {
        // Set current date/time as default
        const now = new Date();
        const dateInput = document.getElementById('manual-date-input');
        const timeInput = document.getElementById('manual-time-input');

        if (dateInput) {
            dateInput.value = now.toISOString().split('T')[0];
        }
        if (timeInput) {
            timeInput.value = now.toTimeString().slice(0, 5);
        }

        updateManualTimePreview();
        modal.style.display = 'block';
    }
}

/**
 * Update manual time preview display
 */
export function updateManualTimePreview() {
    const dateInput = document.getElementById('manual-date-input');
    const timeInput = document.getElementById('manual-time-input');
    const preview = document.getElementById('manual-time-preview');

    if (!dateInput || !timeInput || !preview) return;

    if (dateInput.value && timeInput.value) {
        const selectedDate = new Date(`${dateInput.value}T${timeInput.value}`);
        const days = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
        const dayName = days[selectedDate.getDay()];
        preview.textContent = `${dayName}, ${selectedDate.toLocaleString('en-US')}`;
    } else {
        preview.textContent = 'N/A';
    }
}

/**
 * Apply manual time to current device
 */
export async function applyManualTime() {
    const dateInput = document.getElementById('manual-date-input');
    const timeInput = document.getElementById('manual-time-input');

    if (!dateInput?.value || !timeInput?.value) {
        alert('Please select both date and time!');
        return;
    }

    const selectedDate = new Date(`${dateInput.value}T${timeInput.value}`);
    const timestamp = Math.floor(selectedDate.getTime() / 1000);

    const deviceId = window.currentTimeDeviceId;

    if (deviceId) {
        // Sync to specific device
        await syncTimestampToDevice(deviceId, timestamp);
    } else {
        // Sync to all devices
        await syncTimestampToDevices(timestamp);
    }
}

/**
 * Sync timestamp to a specific device
 * @param {string} deviceId - Device ID
 * @param {number} timestamp - Unix timestamp
 */
export async function syncTimestampToDevice(deviceId, timestamp) {
    if (!isMQTTConnected()) {
        alert("MQTT not connected! Cannot send command.");
        return;
    }

    // Add 7 hours (7 * 60 * 60 = 25200 seconds) to compensate for hardware timezone
    const adjustedTimestamp = timestamp + 25200;

    // Get apply button to show loading state
    const applyBtn = document.querySelector('#manual-time-modal button[onclick*="applyManualTime"]');
    if (applyBtn) {
        applyBtn.disabled = true;
        applyBtn.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i> Applying';
    }

    sendMQTTCommand(deviceId, 'set_timestamp', { timestamp: adjustedTimestamp }, {
        onSuccess: () => {
            if (applyBtn) {
                applyBtn.disabled = false;
                applyBtn.innerHTML = '<i class="fa-solid fa-check"></i> Apply';
            }
            const timeStr = new Date(adjustedTimestamp * 1000).toLocaleString('en-US');
            alert(`Time synced to device "${deviceId}"!\n\nTime (UTC+7): ${timeStr}`);
            closeManualTimeModal();
        },
        onError: (error) => {
            if (applyBtn) {
                applyBtn.disabled = false;
                applyBtn.innerHTML = '<i class="fa-solid fa-check"></i> Apply';
            }
            const errorMsg = error === 'timeout' ? 'Device not responding' : 'Device error';
            alert(`${errorMsg}: Unable to sync time to device "${deviceId}".`);
        },
        timeout: 5000
    });
}

/**
 * Sync time to all devices (using current web time)
 */
export async function syncTimeToAllDevices() {
    await syncTimestampToDevices(Math.floor(Date.now() / 1000));
}

/**
 * Send timestamp to all devices
 * @param {number} timestamp - Unix timestamp in seconds
 */
export async function syncTimestampToDevices(timestamp) {
    if (!isMQTTConnected()) {
        alert("MQTT not connected! Cannot send command.");
        return;
    }

    if (!db) {
        alert("Firebase not initialized!");
        return;
    }

    // Add 7 hours (7 * 60 * 60 = 25200 seconds) to compensate for hardware timezone
    const adjustedTimestamp = timestamp + 25200;

    // Get sync button to show loading state
    const syncBtn = document.querySelector('#manual-time-modal button[onclick*="syncTimeToAllDevices"]');
    if (syncBtn) {
        syncBtn.disabled = true;
        syncBtn.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i> Syncing';
    }

    try {
        const snapshot = await get(ref(db, 'devices'));

        if (!snapshot.exists()) {
            if (syncBtn) {
                syncBtn.disabled = false;
                syncBtn.innerHTML = '<i class="fa-solid fa-globe"></i> Sync Internet';
            }
            alert("No devices found!");
            return;
        }

        const devices = snapshot.val();
        const deviceIds = Object.keys(devices);
        let successCount = 0;
        let pendingCount = deviceIds.length;

        for (const deviceId of deviceIds) {
            sendMQTTCommand(deviceId, 'set_timestamp', { timestamp: adjustedTimestamp }, {
                onSuccess: () => {
                    successCount++;
                    pendingCount--;
                    checkAllDone();
                },
                onError: () => {
                    pendingCount--;
                    checkAllDone();
                },
                timeout: 5000
            });
        }

        function checkAllDone() {
            if (pendingCount === 0) {
                if (syncBtn) {
                    syncBtn.disabled = false;
                    syncBtn.innerHTML = '<i class="fa-solid fa-globe"></i> Sync Internet';
                }
                const currentTime = new Date(adjustedTimestamp * 1000).toLocaleString('en-US');
                alert(`Time synced to ${successCount}/${deviceIds.length} devices!\n\nTime (UTC+7): ${currentTime}`);
                closeManualTimeModal();
            }
        }

    } catch (error) {
        console.error('[Settings] Time sync error:', error);
        if (syncBtn) {
            syncBtn.disabled = false;
            syncBtn.innerHTML = '<i class="fa-solid fa-globe"></i> Sync Internet';
        }
        alert("Error sending time sync command: " + error.message);
    }
}

/**
 * Close manual time modal
 */
export function closeManualTimeModal() {
    const modal = document.getElementById('manual-time-modal');
    if (modal) {
        modal.style.display = 'none';
    }
    window.currentTimeDeviceId = null;
}

/**
 * Create WiFi setup guide HTML content
 * @param {string} deviceId - Device ID (optional)
 * @returns {string} HTML string for the guide
 */
function createWiFiGuideHTML(deviceId = '') {
    const { ip, ssid, password } = ESP32_AP_CONFIG;
    const deviceTitle = deviceId ? ` cho "${deviceId}"` : '';

    return `
        <h4 class="wifi-guide-title">
            <i class="fa-solid fa-circle-info"></i> Các bước cấu hình WiFi${deviceTitle}
        </h4>
        
        <div class="wifi-guide-step">
            <strong>Step 1: Connect to the ESP32 WiFi</strong>
            <ol>
                <li>Open the WiFi list on your phone/computer</li>
                <li>Find and connect to the WiFi network: <code class="wifi-guide-code">${ssid}</code></li>
                <li>Password (if any): <code class="wifi-guide-code">${password}</code></li>
            </ol>
        </div>

        <div class="wifi-guide-step">
            <strong>Step 2: Open browser and access</strong>
            <p>After connecting to the ESP32 WiFi, open a browser and go to:</p>
            <div class="wifi-guide-link-wrapper">
                <a href="http://${ip}" target="_blank" class="wifi-guide-link">
                    <i class="fa-solid fa-external-link-alt"></i> http://${ip}
                </a>
            </div>
        </div>

        <div class="wifi-guide-step">
            <strong>Step 3: Enter your home WiFi information</strong>
            <ol>
                <li>Select your home WiFi name from the list</li>
                <li>Enter your WiFi password</li>
                <li>Click <strong>"Save"</strong> or <strong>"Connect"</strong></li>
            </ol>
        </div>

        <div class="wifi-guide-success">
            <strong>
                <i class="fa-solid fa-check-circle"></i> After configuration
            </strong>
            <p>ESP32 will automatically connect to your home WiFi.</p>
        </div>
    `;
}


/**
 * Cleanup settings resources
 */
export function cleanupSettings() {
    stopClock();
}
