/**
 * ping-service.js
 * Device ping service module
 * Periodically sends ping commands to check device online status
 */

import { sendMQTTCommand, isMQTTConnected } from './mqtt-client.js';
import { setDeviceOnlineStatus, getAllDevicesData } from '../devices/device-card.js';

// Ping configuration
const PING_CONFIG = {
    INTERVAL: 3000,         // Ping every 3 seconds
    TIMEOUT: 2500,          // Response timeout 2.5 seconds
    COMMAND: 'ping',
    INITIAL_PING_TIMEOUT: 5000  // Initial ping timeout (longer for first check)
};

// Ping state tracking
let pingIntervalId = null;

/**
 * Perform initial ping check on all devices
 * @param {Function} onComplete - Callback when all devices have been pinged
 * @returns {Promise} Promise that resolves when initial ping is complete
 */
export function performInitialPing(onComplete) {
    return new Promise((resolve) => {
        if (!isMQTTConnected()) {
            console.log('[PingService] MQTT not connected, skipping initial ping');
            if (onComplete) onComplete();
            resolve();
            return;
        }
        
        const devices = getAllDevicesData();
        const deviceIds = Object.keys(devices);
        
        if (deviceIds.length === 0) {
            console.log('[PingService] No devices to ping');
            if (onComplete) onComplete();
            resolve();
            return;
        }
        
        console.log(`[PingService] Starting initial ping for ${deviceIds.length} devices`);
        
        let completedPings = 0;
        const totalDevices = deviceIds.length;
        
        const onPingComplete = () => {
            completedPings++;
            if (completedPings === totalDevices) {
                console.log('[PingService] Initial ping completed');
                if (onComplete) onComplete();
                resolve();
            }
        };
        
        deviceIds.forEach(deviceId => {
            pingDevice(deviceId, PING_CONFIG.INITIAL_PING_TIMEOUT, onPingComplete);
        });
    });
}

/**
 * Start ping service for all devices
 */
export function startPingService() {
    if (pingIntervalId) {
        console.log('[PingService] Already running');
        return;
    }
    
    console.log('[PingService] Starting periodic ping service');
    
    // Set interval for periodic pings
    pingIntervalId = setInterval(() => {
        pingAllDevices();
    }, PING_CONFIG.INTERVAL);
}

/**
 * Stop ping service
 */
export function stopPingService() {
    if (pingIntervalId) {
        clearInterval(pingIntervalId);
        pingIntervalId = null;
        console.log('[PingService] Stopped');
    }
}

/**
 * Ping all devices
 */
function pingAllDevices() {
    if (!isMQTTConnected()) {
        console.log('[PingService] MQTT not connected, skipping ping');
        return;
    }
    
    const devices = getAllDevicesData();
    const deviceIds = Object.keys(devices);
    
    if (deviceIds.length === 0) {
        return;
    }
    
    deviceIds.forEach(deviceId => {
        pingDevice(deviceId, PING_CONFIG.TIMEOUT);
    });
}

/**
 * Ping a single device
 * @param {string} deviceId - Device identifier
 * @param {number} timeout - Timeout in milliseconds
 * @param {Function} onComplete - Callback when ping completes (success or timeout)
 */
function pingDevice(deviceId, timeout = PING_CONFIG.TIMEOUT, onComplete = null) {
    // Use sendMQTTCommand directly - it handles ID and callbacks
    const result = sendMQTTCommand(deviceId, PING_CONFIG.COMMAND, {}, {
        onSuccess: () => {
            console.log(`[PingService] Response from ${deviceId}`);
            setDeviceOnlineStatus(deviceId, true);
            if (onComplete) onComplete();
        },
        onError: (error) => {
            console.log(`[PingService] ${error === 'timeout' ? 'Timeout' : 'Error'} for ${deviceId}`);
            setDeviceOnlineStatus(deviceId, false);
            if (onComplete) onComplete();
        },
        timeout: timeout
    });
    
    // If command failed to send (MQTT not connected)
    if (!result) {
        console.log(`[PingService] Failed to send ping to ${deviceId}`);
        setDeviceOnlineStatus(deviceId, false);
        if (onComplete) onComplete();
    }
}

/**
 * Check if ping service is running
 * @returns {boolean} Running status
 */
export function isPingServiceRunning() {
    return pingIntervalId !== null;
}
