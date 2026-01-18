/**
 * main.js
 * Main application entry point
 * Coordinates all modules and initializes the application
 */

// Core modules
import { requireAuth, logout } from './core/auth.js';
import { db } from './core/firebase-config.js';
import { ref, get } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js";

// MQTT modules
import { initializeMQTTClient, subscribeToDevice, sendMQTTCommand } from './mqtt/mqtt-client.js';
import { handleMQTTMessage, handleMQTTConnect, handleMQTTConnectionLost, getMQTTCachedState } from './mqtt/mqtt-handlers.js';
import { startPingService, stopPingService, performInitialPing } from './mqtt/ping-service.js';

// Device modules
import { initializeDeviceManager, addDevice, updateDevice, deleteDevice, getAllDevices, setViewType } from './devices/device-manager.js';
import { getAllDevicesData, getDeviceData, isDeviceOnline, initializeDevicesOffline, disableMqttOnlineControl, enableMqttOnlineControl } from './devices/device-card.js';

// Chart modules
import { initializeChart, switchChartType, cleanupChart } from './charts/chart-manager.js';

// UI modules
import {
    updateStatusBadge,
    switchTab as switchTabUI,
    showModal,
    hideModal,
    initializeSidebar,
    startClock
} from './ui/ui-helpers.js';

// Export module
import {
    fetchAllHistoryData,
    renderHistoryTable,
    exportTableToExcel,
    applyFilters,
    resetFilters,
    setQuickFilter,
    exportFilteredData
} from './export/data-export.js';

// Settings module
import {
    initializeSettings,
    loadDeviceInfoTable,
    rebootDevice,
    syncTimeToAllDevices,
    syncTimestampToDevices,
    showWiFiGuideForDevice,
    closeWiFiGuideModal,
    showTimeModalForDevice,
    applyManualTime,
    closeManualTimeModal,
    updateManualTimePreview
} from './settings/settings-manager.js';

// ============================================================
// MQTT SUBSCRIPTION TRACKING
// ============================================================

// Track subscribed devices to prevent duplicate subscriptions
let subscribedDevices = new Set();

/**
 * Subscribe to device MQTT topics if not already subscribed
 * @param {string} deviceId - Device identifier
 */
function subscribeToDeviceOnce(deviceId) {
    if (!subscribedDevices.has(deviceId)) {
        subscribeToDevice(deviceId);
        subscribedDevices.add(deviceId);
        console.log('[App] Subscribed to device:', deviceId);
    }
}

// ============================================================
// APPLICATION INITIALIZATION
// ============================================================

/**
 * Initialize the application
 */
function initializeApp() {
    console.log('[App] Starting Smart Home Dashboard...');

    // Require authentication
    requireAuth();

    // Initialize UI components
    initializeLogoutButton();
    initializeSidebar();
    initializeModalHandlers();

    // Initialize Firebase sync FIRST, then MQTT
    if (db) {
        initializeFirebase();
    } else {
        console.warn('[App] Firebase not initialized. Please configure Firebase settings.');
        updateStatusBadge('db-status', 'error', 'Firebase: Not Configured');
        // Still initialize MQTT even without Firebase
        initializeMQTT();
    }

    // Make functions globally available for HTML onclick handlers
    exposeGlobalFunctions();

    console.log('[App] Initialization complete');
}

/**
 * Initialize logout button
 */
function initializeLogoutButton() {
    const logoutBtn = document.getElementById('logout-btn');
    if (logoutBtn) {
        logoutBtn.addEventListener('click', async () => {
            try {
                await logout();
                window.location.href = 'login.html';
            } catch (error) {
                console.error('[App] Logout error:', error);
                alert('Logout error: ' + error.message);
            }
        });
    }
}

/**
 * Initialize Firebase and device manager
 */
function initializeFirebase() {
    updateStatusBadge('db-status', 'success', 'Firebase: Connected');

    // Initialize with 'manage' view by default (Manage tab)
    // Wait for initial device load before initializing MQTT
    initializeDeviceManager('manage', () => {
        console.log('[App] Devices loaded, initializing MQTT...');
        initializeMQTT();
    });
}

/**
 * Initialize MQTT client
 */
function initializeMQTT() {
    const onConnect = () => {
        handleMQTTConnect(() => Object.keys(getAllDevicesData()));

        // Get all device IDs
        const devices = getAllDevicesData();
        const deviceIds = Object.keys(devices);

        console.log('[App] MQTT connected, initializing...');

        // Step 1: Initialize all devices as offline first
        initializeDevicesOffline(deviceIds);

        // Step 2: Disable MQTT from setting online status (block retained messages)
        disableMqttOnlineControl();

        // Step 3: Subscribe to all devices FIRST (required before sending commands)
        console.log('[App] Subscribing to device topics...');
        deviceIds.forEach(deviceId => {
            subscribeToDeviceOnce(deviceId);
        });

        // Step 4: Wait a moment for subscriptions to complete, then ping
        setTimeout(() => {
            console.log('[App] Performing initial ping check...');

            // Perform initial ping to check device status
            performInitialPing(() => {
                console.log('[App] Initial ping completed, enabling MQTT online control...');

                // Step 5: After initial ping, allow MQTT to control online status
                enableMqttOnlineControl();

                // Step 6: Start periodic ping service
                startPingService();

                console.log('[App] MQTT initialization complete');
            });
        }, 500); // Small delay to ensure subscriptions are ready
    };

    initializeMQTTClient(onConnect, handleMQTTMessage, handleMQTTConnectionLost);
}

/**
 * Initialize modal handlers
 */
function initializeModalHandlers() {
    // Add device modal
    const addModal = document.getElementById('add-modal');
    const addBtn = document.getElementById('btn-open-modal');
    const addForm = document.getElementById('add-form');
    const addCloseBtn = addModal?.querySelector('.close');

    if (addBtn && addModal) {
        addBtn.addEventListener('click', () => {
            showModal('add-modal');
        });
    }

    if (addCloseBtn) {
        addCloseBtn.addEventListener('click', () => {
            hideModal('add-modal');
        });
    }

    if (addForm) {
        addForm.addEventListener('submit', handleAddDevice);
    }

    // Edit device modal
    const editModal = document.getElementById('edit-modal');
    const editCloseBtn = editModal?.querySelector('.closeBtn');
    const editForm = document.getElementById('edit-form');
    const deleteBtn = document.getElementById('btn-delete-device');

    if (editCloseBtn) {
        editCloseBtn.addEventListener('click', () => {
            hideModal('edit-modal');
        });
    }

    if (editForm) {
        editForm.addEventListener('submit', handleEditDevice);
    }

    if (deleteBtn) {
        deleteBtn.addEventListener('click', handleDeleteDevice);
    }
}

/**
 * Handle add device form submission
 */
async function handleAddDevice(e) {
    e.preventDefault();

    const name = document.getElementById('dev-name').value.trim();
    const id = document.getElementById('dev-id').value.trim();
    const interval = document.getElementById('dev-interval').value;

    try {
        await addDevice({ id, name, interval });
        alert('Added new device!');
        hideModal('add-modal');
        document.getElementById('add-form').reset();

        // Subscribe to new device MQTT topics (only once)
        subscribeToDeviceOnce(id);
    } catch (error) {
        alert('Error: ' + error.message);
    }
}

/**
 * Handle edit device form submission
 */
async function handleEditDevice(e) {
    e.preventDefault();

    const deviceId = document.getElementById('edit-dev-id').value;
    const name = document.getElementById('edit-dev-name').value.trim();
    const interval = parseInt(document.getElementById('edit-dev-interval').value);

    // Get save button to show loading state
    const saveBtn = document.querySelector('#edit-form button[type="submit"]');
    if (saveBtn) {
        saveBtn.disabled = true;
        saveBtn.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i> Saving';
    }

    try {
        // Update Firebase first
        await updateDevice(deviceId, { name, interval });

        // Send MQTT command with response callback
        sendMQTTCommand(deviceId, 'set_interval', { interval: interval }, {
            onSuccess: () => {
                if (saveBtn) {
                    saveBtn.disabled = false;
                    saveBtn.innerHTML = 'Save Changes';
                }
                alert(`Updated device!\n\nInterval changed to ${interval}s.`);
                hideModal('edit-modal');
            },
            onError: (error) => {
                if (saveBtn) {
                    saveBtn.disabled = false;
                    saveBtn.innerHTML = 'Save Changes';
                }
                const errorMsg = error === 'timeout' ? 'Device not responding' : 'Device error';
                alert(`Firebase updated, but ${errorMsg}.\n\nDevice interval may not be synced.`);
                hideModal('edit-modal');
            },
            timeout: 5000
        });
    } catch (error) {
        if (saveBtn) {
            saveBtn.disabled = false;
            saveBtn.innerHTML = 'Save Changes';
        }
        alert('Error: ' + error.message);
    }
}

/**
 * Handle delete device
 */
async function handleDeleteDevice() {
    const deviceId = document.getElementById('edit-dev-id').value;

    if (!confirm(`Delete device "${deviceId}"?\n\nThis action cannot be undone!`)) {
        return;
    }

    try {
        await deleteDevice(deviceId);
        alert('Deleted device!');
        hideModal('edit-modal');
    } catch (error) {
        alert('Error: ' + error.message);
    }
}

/**
 * Expose functions to global window object for HTML onclick handlers
 */
function exposeGlobalFunctions() {
    // Tab switching - wrap to update device view type
    window.switchTab = (tabName) => {
        const viewType = switchTabUI(tabName);
        // Update device manager view type when switching to tabs with device grid
        if (viewType) {
            setViewType(viewType);
        }
        // Initialize settings when switching to setting tab
        if (tabName === 'setting') {
            initializeSettings();
        }
    };

    // Modal operations (for dynamic buttons)
    window.showModal = showModal;
    window.hideModal = hideModal;

    // Device power control
    window.toggleDevicePower = async (deviceId, newPowerState) => {
        // Check if device is online first
        if (!isDeviceOnline(deviceId)) {
            alert(`Device "${deviceId}" is Offline.\n\nUnable to send control command.`);
            return;
        }

        // Get button element to update UI
        const card = document.getElementById(`device-${deviceId}`);
        const powerBtn = card?.querySelector('button[onclick*="toggleDevicePower"]');

        if (powerBtn) {
            // Disable button and show loading
            powerBtn.disabled = true;
            powerBtn.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i>';
        }

        // Send MQTT command with response callback
        const result = sendMQTTCommand(deviceId, 'set_mode', { mode: newPowerState ? 1 : 0 }, {
            onSuccess: () => {
                console.log(`[App] Power command successful for ${deviceId}`);
                // Update UI on success
                if (powerBtn) {
                    powerBtn.disabled = false;
                    if (newPowerState) {
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
            },
            onError: (error) => {
                console.error(`[App] Power command failed for ${deviceId}:`, error);
                // Restore button state
                if (powerBtn) {
                    powerBtn.disabled = false;
                    // Restore to previous state (opposite of what we tried to set)
                    if (!newPowerState) {
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
                const errorMsg = error === 'timeout' ? 'Device not responding' : 'Device error';
                alert(`${errorMsg}: Unable to ${newPowerState ? 'turn ON' : 'turn OFF'} device.`);
            },
            timeout: 5000
        });

        if (!result) {
            // Command failed to send
            if (powerBtn) {
                powerBtn.disabled = false;
                // Restore original state
                if (!newPowerState) {
                    powerBtn.classList.remove('btn-danger');
                    powerBtn.classList.add('btn-success');
                    powerBtn.innerHTML = '<i class="fa-solid fa-power-off"></i> ON';
                } else {
                    powerBtn.classList.remove('btn-success');
                    powerBtn.classList.add('btn-danger');
                    powerBtn.innerHTML = '<i class="fa-solid fa-power-off"></i> OFF';
                }
            }
            alert('Unable to send control command. Check MQTT connection.');
        }
    };

    // Device operations
    window.openEditModal = async (deviceId) => {
        const device = getDeviceData(deviceId);
        if (!device) {
            alert('Device not found!');
            return;
        }

        document.getElementById('edit-dev-id').value = deviceId;
        document.getElementById('edit-dev-name').value = device.name;
        document.getElementById('edit-dev-interval').value = device.interval;

        showModal('edit-modal');
    };

    // Chart/Report operations
    window.openReportDetail = async (deviceId) => {
        const device = getDeviceData(deviceId);
        if (!device) {
            alert('Device not found!');
            return;
        }

        // Store current device ID globally for toggleFeature
        window.currentDetailDeviceId = deviceId;

        // Update modal title
        document.getElementById('report-title').textContent = device.name;

        // Update sensor values in stat boxes
        const tempEl = document.getElementById('detail-temp');
        const humidEl = document.getElementById('detail-humid');
        const lightEl = document.getElementById('detail-light');

        if (tempEl) tempEl.textContent = `${device.sensors?.temperature || 0} °C`;
        if (humidEl) humidEl.textContent = `${device.sensors?.humidity || 0} %`;
        if (lightEl) lightEl.textContent = `${device.sensors?.light || 0} Lux`;

        // Determine initial Quick Control toggle states
        let fanState = false, lightState = false, acState = false;
        let stateSource = 'none';

        // 1. MQTT Cached State (realtime from broker)
        const mqttState = getMQTTCachedState(deviceId);
        if (mqttState) {
            console.log('[App] Using MQTT cached state (realtime from hardware):', mqttState);
            fanState = mqttState.fan === 1;
            lightState = mqttState.light === 1;
            acState = mqttState.ac === 1;
            stateSource = 'mqtt-cache';
        } else {
            // 2. Fallback: Get from Firebase
            try {
                const stateRef = ref(db, `SmartHome/${deviceId}/state`);
                const stateSnapshot = await get(stateRef);

                if (stateSnapshot.exists()) {
                    const hardwareState = stateSnapshot.val();
                    console.log('[App] Using Firebase SmartHome state:', hardwareState);
                    fanState = hardwareState.fan === 1;
                    lightState = hardwareState.light === 1;
                    acState = hardwareState.ac === 1;
                    stateSource = 'firebase';
                } else {
                    console.warn('[App] No state found, using device cache');
                    fanState = device.states?.fan || false;
                    lightState = device.states?.lamp || false;
                    acState = device.states?.ac || false;
                    stateSource = 'device-cache';
                }
            } catch (error) {
                console.error('[App] Error fetching state:', error);
                fanState = device.states?.fan || false;
                lightState = device.states?.lamp || false;
                acState = device.states?.ac || false;
                stateSource = 'device-cache-error';
            }
        }

        console.log(`[App] Quick Control loaded from: ${stateSource} | fan=${fanState}, light=${lightState}, ac=${acState}`);

        // Update toggle switches with hardware state
        const fanToggle = document.getElementById('toggle-fan');
        const lampToggle = document.getElementById('toggle-lamp');
        const acToggle = document.getElementById('toggle-ac');

        if (fanToggle) fanToggle.checked = fanState;
        if (lampToggle) lampToggle.checked = lightState;
        if (acToggle) acToggle.checked = acState;

        // Reset active states and set temp as default active
        document.getElementById('btn-chart-temp')?.classList.add('active-chart');
        document.getElementById('btn-chart-humid')?.classList.remove('active-chart');
        document.getElementById('btn-chart-light')?.classList.remove('active-chart');

        showModal('report-detail');

        // Initialize chart with temp as default
        initializeChart(deviceId, 'temp');
    };

    window.closeReportDetail = () => {
        hideModal('report-detail');
        cleanupChart();
        window.currentDetailDeviceId = null; // Clear device ID
    };

    window.selectChartType = (type) => {
        switchChartType(type);
    };

    window.toggleFeature = (feature) => {
        const deviceId = window.currentDetailDeviceId;
        if (!deviceId) {
            console.error('[App] No device selected for toggle');
            return;
        }

        // Map feature names: lamp → light for MQTT command
        const deviceName = feature === 'lamp' ? 'light' : feature;

        // Get current toggle state (what user just clicked)
        const toggleId = `toggle-${feature}`;
        const toggleEl = document.getElementById(toggleId);
        if (!toggleEl) return;

        const newState = toggleEl.checked ? 1 : 0;
        const previousState = !toggleEl.checked;

        // Immediately revert toggle - will be set correctly on success
        toggleEl.checked = previousState;
        toggleEl.disabled = true;

        console.log(`[App] Toggle ${deviceName}: ${newState ? 'ON' : 'OFF'}`);

        // Send MQTT command with response callback
        const result = sendMQTTCommand(deviceId, 'set_device', {
            device: deviceName,
            state: newState
        }, {
            onSuccess: () => {
                console.log(`[App] Toggle ${deviceName} successful`);
                toggleEl.disabled = false;
                toggleEl.checked = newState === 1;
            },
            onError: (error) => {
                console.error(`[App] Toggle ${deviceName} failed:`, error);
                toggleEl.disabled = false;
                toggleEl.checked = previousState;
                const featureNames = { fan: 'Fan', lamp: 'Lamp', ac: 'Air Conditioner' };
                const errorMsg = error === 'timeout' ? 'Device not responding' : 'Device error';
                alert(`${errorMsg}: Unable to control ${featureNames[feature]}.`);
            },
            timeout: 3000
        });

        if (!result) {
            toggleEl.disabled = false;
            toggleEl.checked = previousState;
            alert('Unable to send toggle command. Check MQTT connection.');
        }
    };

    // Export operations
    window.applyFilters = async () => {
        try {
            await applyFilters();
        } catch (error) {
            alert('Error filtering data: ' + error.message);
        }
    };

    window.resetFilters = () => {
        resetFilters();
    };

    window.setQuickFilter = (filterType) => {
        setQuickFilter(filterType);
    };

    window.exportFilteredData = () => {
        exportFilteredData();
    };

    // Legacy export functions (kept for compatibility)
    window.fetchAllHistoryData = async () => {
        try {
            const data = await fetchAllHistoryData();
            renderHistoryTable(data);
            alert(`Loaded ${data.length} records`);
        } catch (error) {
            alert('Error loading data: ' + error.message);
        }
    };

    window.exportTableToExcel = exportTableToExcel;

    // Settings operations - per device actions
    window.rebootDevice = rebootDevice;
    window.showWiFiGuideForDevice = showWiFiGuideForDevice;
    window.closeWiFiGuideModal = closeWiFiGuideModal;
    window.showTimeModalForDevice = showTimeModalForDevice;
    window.applyManualTime = applyManualTime;
    window.syncTimeToAllDevices = syncTimeToAllDevices;
    window.closeManualTimeModal = closeManualTimeModal;

    // Update manual time preview when inputs change
    const dateInput = document.getElementById('manual-date-input');
    const timeInput = document.getElementById('manual-time-input');
    if (dateInput) dateInput.addEventListener('change', updateManualTimePreview);
    if (timeInput) timeInput.addEventListener('change', updateManualTimePreview);
}

// ============================================================
// START APPLICATION
// ============================================================

// Initialize when DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initializeApp);
} else {
    initializeApp();
}
