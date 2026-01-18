/**
 * mqtt-client.js
 * MQTT client management module
 * Handles MQTT connection, reconnection, and message publishing
 */

// MQTT configuration constants
export const MQTT_TOPICS = {
    DATA: 'data',       //!< Sensor data topic
    STATE: 'state',     //!< Device state topic
    INFO: 'info',       //!< Device info topic
    COMMAND: 'command', //!< Command topic
    RESPONSE: 'response' //!< Response topic for command acknowledgment
};

// Pending commands waiting for response
const pendingCommands = new Map();

/**
 * Register a pending command callback
 * @param {string} cmdId - Command ID
 * @param {Object} callbacks - { onSuccess, onError, timeout }
 */
export function registerCommandCallback(cmdId, callbacks) {
    const { onSuccess, onError, timeout = 5000 } = callbacks;
    
    // Set timeout to auto-reject
    const timeoutId = setTimeout(() => {
        if (pendingCommands.has(cmdId)) {
            pendingCommands.delete(cmdId);
            if (onError) onError('timeout');
            console.warn(`[MQTT] Command ${cmdId} timed out`);
        }
    }, timeout);
    
    pendingCommands.set(cmdId, {
        onSuccess,
        onError,
        timeoutId
    });
}

/**
 * Handle response message and resolve pending command
 * @param {string} cmdId - Command ID from response
 * @param {string} status - 'success' or 'error'
 */
export function handleCommandResponse(cmdId, status) {
    console.log(`[MQTT] handleCommandResponse('${cmdId}', '${status}')`);
    console.log('[MQTT] Pending commands:', Array.from(pendingCommands.keys()));
    
    const pending = pendingCommands.get(cmdId);
    if (!pending) {
        console.warn(`[MQTT] No pending command found for ${cmdId}`);
        console.warn('[MQTT] Available pending:', Array.from(pendingCommands.keys()));
        return;
    }
    
    console.log(`[MQTT] Found pending command for ${cmdId}, clearing timeout`);
    clearTimeout(pending.timeoutId);
    pendingCommands.delete(cmdId);
    
    if (status === 'success') {
        console.log(`[MQTT] Calling onSuccess for ${cmdId}`);
        if (pending.onSuccess) pending.onSuccess();
    } else {
        console.log(`[MQTT] Calling onError for ${cmdId}`);
        if (pending.onError) pending.onError(status);
    }
}

const RETRY_CONFIG = {
    MQTT_RETRY_DELAY: 5000, //!< 5 seconds between reconnection attempts
    MAX_RETRY_COUNT: 10     //!< Maximum reconnection attempts
};

// MQTT client state
let mqttClient = null;
let mqttRetryCount = 0;
let commandCounter = 0;

/**
 * Load MQTT configuration from localStorage or use defaults
 * @returns {Object} MQTT configuration object
 */
function loadMQTTConfig() {
    const savedConfig = localStorage.getItem('mqtt_config');

    if (savedConfig) {
        try {
            const config = JSON.parse(savedConfig);
            return {
                host: config.host || "broker.emqx.io",
                port: config.port || 8083,
                path: config.path || "/mqtt",
                useSSL: config.useSSL || false,
                username: config.username || "",
                password: config.password || "",
                keepalive: config.keepalive || 60,
                clientId: generateClientId()
            };
        } catch (error) {
            console.error('[MQTT] Error loading config:', error.message);
        }
    }

    // Default configuration - HiveMQ Cloud
    return {
        host: "6ceea111b6144c71a57b21faa3553fc6.s1.eu.hivemq.cloud",
        port: 8884,
        path: "/mqtt",
        useSSL: true,
        username: "SmartHome",
        password: "SmartHome01",
        keepalive: 60,
        clientId: generateClientId()
    };
}

/**
 * Generate unique MQTT client ID
 * @returns {string} Unique client ID
 */
function generateClientId() {
    return "WebDashboard_" + Math.random().toString(16).substr(2, 8);
}

/**
 * Initialize MQTT client and connect
 * @param {Function} onConnect - Callback when connected
 * @param {Function} onMessage - Callback when message received
 * @param {Function} onConnectionLost - Callback when connection lost
 * @returns {Object} MQTT client instance
 */
export function initializeMQTTClient(onConnect, onMessage, onConnectionLost) {
    const config = loadMQTTConfig();

    mqttClient = new Paho.MQTT.Client(
        config.host,
        Number(config.port),
        config.path,
        config.clientId
    );

    // Setup callbacks
    mqttClient.onConnectionLost = (response) => {
        console.error('[MQTT] Connection lost:', response.errorMessage);
        onConnectionLost(response);
    };

    mqttClient.onMessageArrived = (message) => {
        onMessage(message);
    };

    // Connection options
    const connectOptions = {
        useSSL: config.useSSL,
        timeout: 10,
        keepAliveInterval: config.keepalive,
        cleanSession: true,
        onSuccess: () => {
            console.log('[MQTT] Connected successfully');
            mqttRetryCount = 0;
            onConnect();
        },
        onFailure: (error) => {
            console.error('[MQTT] Connection failed:', error.errorMessage);
            handleConnectionFailure();
        }
    };

    // Add credentials if provided
    if (config.username) {
        connectOptions.userName = config.username;
        connectOptions.password = config.password;
    }

    // Connect
    console.log('[MQTT] Connecting to', config.host);
    mqttClient.connect(connectOptions);

    return mqttClient;
}

/**
 * Handle MQTT connection failure with retry logic
 */
function handleConnectionFailure() {
    mqttRetryCount++;

    if (mqttRetryCount >= RETRY_CONFIG.MAX_RETRY_COUNT) {
        console.error('[MQTT] Max retry count reached. Stopping reconnection attempts.');
        return;
    }

    console.log(`[MQTT] Retry attempt ${mqttRetryCount}/${RETRY_CONFIG.MAX_RETRY_COUNT} in ${RETRY_CONFIG.MQTT_RETRY_DELAY / 1000}s`);

    setTimeout(() => {
        if (mqttClient) {
            try {
                mqttClient.connect();
            } catch (error) {
                console.error('[MQTT] Reconnection error:', error);
            }
        }
    }, RETRY_CONFIG.MQTT_RETRY_DELAY);
}

/**
 * Subscribe to device topics
 * @param {string} deviceId - Device identifier
 */
export function subscribeToDevice(deviceId) {
    if (!mqttClient || !mqttClient.isConnected()) {
        console.error('[MQTT] Client not connected');
        return false;
    }

    const topics = [
        `SmartHome/${deviceId}/${MQTT_TOPICS.DATA}`,
        `SmartHome/${deviceId}/${MQTT_TOPICS.STATE}`,
        `SmartHome/${deviceId}/${MQTT_TOPICS.INFO}`,
        `SmartHome/${deviceId}/${MQTT_TOPICS.RESPONSE}`
    ];

    topics.forEach(topic => {
        try {
            mqttClient.subscribe(topic);
            console.log('[MQTT] Subscribed to:', topic);
        } catch (error) {
            console.error('[MQTT] Subscribe error:', error);
        }
    });

    return true;
}

/**
 * Send MQTT command to device
 * @param {string} deviceId - Device identifier
 * @param {string} command - Command name
 * @param {Object} params - Command parameters
 * @param {Object} callbacks - Optional callbacks { onSuccess, onError, timeout }
 * @returns {string|boolean} Command ID if successful with callbacks, true if sent without callbacks, false if failed
 */
export function sendMQTTCommand(deviceId, command, params, callbacks = null) {
    if (!mqttClient || !mqttClient.isConnected()) {
        console.error('[MQTT] Client not connected');
        if (callbacks?.onError) callbacks.onError('not_connected');
        return false;
    }

    const topic = `SmartHome/${deviceId}/${MQTT_TOPICS.COMMAND}`;
    commandCounter++;

    const cmdId = `cmd_${commandCounter.toString().padStart(3, '0')}`;
    const payload = {
        id: cmdId,
        command: command,
        params: params
    };

    try {
        const message = new Paho.MQTT.Message(JSON.stringify(payload));
        message.destinationName = topic;
        mqttClient.send(message);
        console.log(`[MQTT] Sent ${command} to ${deviceId} (${cmdId})`);
        
        // Register callback if provided
        if (callbacks) {
            registerCommandCallback(cmdId, callbacks);
            return cmdId;
        }
        
        return true;
    } catch (error) {
        console.error(`[MQTT] Failed to send ${command} to ${deviceId}:`, error);
        if (callbacks?.onError) callbacks.onError('send_failed');
        return false;
    }
}

/**
 * Check if MQTT client is connected
 * @returns {boolean} Connection status
 */
export function isMQTTConnected() {
    return mqttClient && mqttClient.isConnected();
}

/**
 * Get MQTT client instance
 * @returns {Object} MQTT client
 */
export function getMQTTClient() {
    return mqttClient;
}
