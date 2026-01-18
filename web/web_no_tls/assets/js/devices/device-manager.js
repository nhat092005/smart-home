/**
 * device-manager.js
 * Device management module
 * Handles device CRUD operations and Firebase sync
 */

import { db } from '../core/firebase-config.js';
import { ref, onValue, set, update, remove, get } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js";
import { renderDeviceGrid } from './device-card.js';

// TODO: subscribeToDevice import - subscription handled in main.js to prevent loops 

// Track active Firebase listeners
let activeListeners = [];
let currentViewType = 'manage'; // Track current view type
let cachedDevices = {}; // Cache current devices data

/**
 * Initialize device manager and listen for device changes
 * @param {string} viewType - View type: 'manage' or 'dashboard'
 * @param {Function} onInitialLoad - Callback when initial devices are loaded
 */
export function initializeDeviceManager(viewType = 'manage', onInitialLoad = null) {
    currentViewType = viewType;

    if (!db) {
        console.error('[DeviceManager] Firebase not initialized');
        if (onInitialLoad) onInitialLoad();
        return;
    }

    const devicesRef = ref(db, 'devices');

    let isFirstLoad = true;

    // Listen for device changes
    const unsubscribe = onValue(devicesRef, (snapshot) => {
        if (snapshot.exists()) {
            const devices = snapshot.val();
            cachedDevices = devices; // Cache the devices data
            console.log('[DeviceManager] Devices updated:', Object.keys(devices).length);
            renderDeviceGrid(devices, currentViewType);

            // TODO: Auto-subscribe moved to main.js to prevent infinite loop
        } else {
            console.log('[DeviceManager] No devices found');
            cachedDevices = {};
            renderDeviceGrid({}, currentViewType);
        }

        // Call onInitialLoad callback only on first load
        if (isFirstLoad && onInitialLoad) {
            isFirstLoad = false;
            console.log('[DeviceManager] Initial load complete');
            onInitialLoad();
        }
    }, (error) => {
        console.error('[DeviceManager] Firebase listener error:', error);
        if (isFirstLoad && onInitialLoad) {
            isFirstLoad = false;
            onInitialLoad();
        }
    });

    activeListeners.push(unsubscribe);
}

/**
 * Attach listener to "Add Device" button
 * Called after re-rendering grid in manage view
 */
function attachAddDeviceListener() {
    const addBtn = document.getElementById('btn-open-modal');
    if (addBtn && !addBtn.hasAttribute('data-listener-attached')) {
        addBtn.addEventListener('click', () => {
            // Show modal via global function
            if (window.showModal) {
                window.showModal('add-modal');
            }
        });
        addBtn.setAttribute('data-listener-attached', 'true');
        console.log('[DeviceManager] Add device button listener attached');
    }
}

/**
 * Set current view type and re-render device grid
 * @param {string} viewType - View type: 'manage' or 'dashboard'
 */
export function setViewType(viewType) {
    console.log('[DeviceManager] Setting view type:', viewType);
    currentViewType = viewType;

    // Re-render grid with cached devices data
    if (cachedDevices && Object.keys(cachedDevices).length > 0) {
        renderDeviceGrid(cachedDevices, currentViewType);

        // Re-attach add device button listener if in manage view
        if (viewType === 'manage') {
            setTimeout(() => {
                attachAddDeviceListener();
            }, 50);
        }
    } else {
        // No devices yet, render empty grid
        renderDeviceGrid({}, currentViewType);
    }
}

/**
 * Add new device to Firebase
 * @param {Object} deviceData - Device data object
 * @returns {Promise} Promise that resolves when device is added
 */
export async function addDevice(deviceData) {
    if (!db) {
        throw new Error('Firebase not initialized');
    }

    const { id, name, interval } = deviceData;

    // Validate input
    if (!id || !name) {
        throw new Error('Device ID and name are required');
    }

    try {
        const deviceRef = ref(db, `devices/${id}`);

        // Check if device already exists
        const snapshot = await get(deviceRef);
        if (snapshot.exists()) {
            throw new Error(`Device ${id} already exists`);
        }

        // Create device object
        const device = {
            name: name,
            interval: parseInt(interval) || 5,
            createdAt: Date.now(),
            lastUpdate: 0,
            sensors: {
                temperature: 0,
                humidity: 0,
                light: 0
            },
            states: {
                power: false,
                fan: false,
                lamp: false,
                ac: false
            }
        };

        await set(deviceRef, device);
        console.log('[DeviceManager] Device added:', id);

        return device;
    } catch (error) {
        console.error('[DeviceManager] Add device error:', error);
        throw error;
    }
}

/**
 * Update device information
 * @param {string} deviceId - Device identifier
 * @param {Object} updates - Object containing fields to update
 * @returns {Promise} Promise that resolves when device is updated
 */
export async function updateDevice(deviceId, updates) {
    if (!db) {
        throw new Error('Firebase not initialized');
    }

    try {
        const deviceRef = ref(db, `devices/${deviceId}`);
        await update(deviceRef, updates);
        console.log('[DeviceManager] Device updated:', deviceId);
    } catch (error) {
        console.error('[DeviceManager] Update device error:', error);
        throw error;
    }
}

/**
 * Delete device from Firebase
 * @param {string} deviceId - Device identifier
 * @returns {Promise} Promise that resolves when device is deleted
 */
export async function deleteDevice(deviceId) {
    if (!db) {
        throw new Error('Firebase not initialized');
    }

    try {
        const deviceRef = ref(db, `devices/${deviceId}`);
        await remove(deviceRef);
        console.log('[DeviceManager] Device deleted:', deviceId);
    } catch (error) {
        console.error('[DeviceManager] Delete device error:', error);
        throw error;
    }
}

/**
 * Get all devices from Firebase
 * @returns {Promise<Object>} Promise that resolves with devices object
 */
export async function getAllDevices() {
    if (!db) {
        throw new Error('Firebase not initialized');
    }

    try {
        const devicesRef = ref(db, 'devices');
        const snapshot = await get(devicesRef);

        if (snapshot.exists()) {
            return snapshot.val();
        }
        return {};
    } catch (error) {
        console.error('[DeviceManager] Get devices error:', error);
        throw error;
    }
}

/**
 * Get single device data
 * @param {string} deviceId - Device identifier
 * @returns {Promise<Object>} Promise that resolves with device data
 */
export async function getDevice(deviceId) {
    if (!db) {
        throw new Error('Firebase not initialized');
    }

    try {
        const deviceRef = ref(db, `devices/${deviceId}`);
        const snapshot = await get(deviceRef);

        if (snapshot.exists()) {
            return snapshot.val();
        }
        return null;
    } catch (error) {
        console.error('[DeviceManager] Get device error:', error);
        throw error;
    }
}

/**
 * Cleanup device manager listeners
 */
export function cleanupDeviceManager() {
    activeListeners.forEach(unsubscribe => unsubscribe());
    activeListeners = [];
    console.log('[DeviceManager] Listeners cleaned up');
}
