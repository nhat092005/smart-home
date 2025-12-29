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

// Device modules
import { initializeDeviceManager, addDevice, updateDevice, deleteDevice, getAllDevices, setViewType } from './devices/device-manager.js';
import { getAllDevicesData, getDeviceData } from './devices/device-card.js';

// Chart modules
import { initializeChart, switchChartType, cleanupChart } from './charts/chart-manager.js';

// UI modules
import {
    updateStatusBadge,
    switchTab,
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

    // Initialize Firebase sync
    if (db) {
        initializeFirebase();
    } else {
        console.warn('[App] Firebase not initialized. Please configure Firebase settings.');
        updateStatusBadge('db-status', 'error', 'Firebase: Not Configured');
    }

    // Initialize MQTT
    initializeMQTT();

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
    initializeDeviceManager('manage');
}

/**
 * Initialize MQTT client
 */
function initializeMQTT() {
    const onConnect = () => {
        handleMQTTConnect(() => Object.keys(getAllDevicesData()));

        // Subscribe to all existing devices (only once per device)
        const devices = getAllDevicesData();
        Object.keys(devices).forEach(deviceId => {
            subscribeToDeviceOnce(deviceId);
        });
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

    try {
        // Update Firebase
        await updateDevice(deviceId, { name, interval });

        // Send MQTT command to update interval on ESP32
        const mqttSuccess = sendMQTTCommand(deviceId, 'set_interval', { interval: interval });

        if (mqttSuccess) {
            alert(`Updated device!\n\nSent set_interval command (${interval}s) to ESP32.`);
        } else {
            alert(`Updated device in Firebase!\n\nUnable to send MQTT command (check connection).`);
        }

        hideModal('edit-modal');
    } catch (error) {
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
        const viewType = switchTab(tabName);
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
        try {
            // Send MQTT command - button will be updated automatically when ESP32 responds
            const success = sendMQTTCommand(deviceId, 'set_mode', { mode: newPowerState ? 1 : 0 });

            if (success) {
                console.log(`[App] Sent power command to ${deviceId}: ${newPowerState ? 'ON' : 'OFF'}`);
                // REMOVED: Button update logic - mqtt-handlers.js will handle it when receiving state response
            } else {
                console.error('[App] Failed to send MQTT command');
                alert('Unable to send control command. Check MQTT connection.');
            }
        } catch (error) {
            console.error('[App] Toggle device power error:', error);
            alert('Device control error: ' + error.message);
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
            // 2. Fallback: Lấy từ Firebase
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

    // Track pending toggle commands for timeout checking
    const pendingToggles = new Map();

    // Expose function to clear pending toggles from MQTT handlers
    window.clearPendingToggle = (deviceId, feature) => {
        const key = `${deviceId}_${feature}`;
        const pending = pendingToggles.get(key);
        if (pending) {
            clearTimeout(pending.timeoutId);
            pendingToggles.delete(key);
            console.log(`[App] Cleared pending toggle for ${feature}`);
        }
    };

    // Expose function to update toggle UI from MQTT state
    window.updateToggleUI = (deviceId, feature, state) => {
        if (deviceId !== window.currentDetailDeviceId) return;

        const toggleId = `toggle-${feature}`;
        const toggleEl = document.getElementById(toggleId);
        if (toggleEl) {
            toggleEl.checked = state === 1 || state === true;
            console.log(`[App] Updated ${feature} toggle to ${state}`);
        }
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

        console.log(`[App] Toggle ${deviceName}: ${newState ? 'ON' : 'OFF'}`);

        // Send MQTT command
        const success = sendMQTTCommand(deviceId, 'set_device', {
            device: deviceName,
            state: newState
        });

        if (!success) {
            alert('Unable to send toggle command. Check MQTT connection.');
            // Revert toggle
            toggleEl.checked = !toggleEl.checked;
            return;
        }

        // Start timeout timer (3 seconds)
        const timeoutId = setTimeout(() => {
            // If still in pending map after 3s, device didn't respond
            if (pendingToggles.has(`${deviceId}_${feature}`)) {
                console.warn(`[App] Device ${deviceId} no response for ${feature}`);
                alert(`Device did not respond to ${feature === 'fan' ? 'Fan' : feature === 'lamp' ? 'Light' : 'AC'} command`);

                // Revert toggle to previous state
                toggleEl.checked = !toggleEl.checked;

                // Remove from pending
                pendingToggles.delete(`${deviceId}_${feature}`);
            }
        }, 3000);

        // Store pending toggle
        pendingToggles.set(`${deviceId}_${feature}`, {
            timeoutId,
            expectedState: newState,
            toggleEl
        });
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
