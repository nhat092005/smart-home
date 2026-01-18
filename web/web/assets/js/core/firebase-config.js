/**
 * firebase-config.js
 * Firebase configuration and initialization module
 * Handles loading config from localStorage or using defaults
 */

import { initializeApp } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-app.js";
import { getAuth } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-auth.js";
import { getDatabase } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js";

// Storage key constant for consistency
const FIREBASE_CONFIG_KEY = 'user_firebase_config';

// Default Firebase configuration (empty - requires user setup)
const defaultConfig = {
    apiKey: "AIzaSyBs8FHNVvdwQVoLO1SGHY19AJ5lv601fMM",                  //!< Default API key for demo purposes
    authDomain: "smarthome-14f3c.firebaseapp.com",                      //!< Default auth domain
    databaseURL: "https://smarthome-14f3c-default-rtdb.firebaseio.com", //!< Default database URL
    projectId: "smarthome-14f3c",                                       //!< Default project ID
    storageBucket: "smarthome-14f3c.firebasestorage.app",               //!< Default storage bucket
    messagingSenderId: "416507681974",                                  //!< Default sender ID
    appId: "1:416507681974:web:30d64fc4e6d40dbfeb5a17",                 //!< Default app ID
    measurementId: "G-WV5KTYDZX9"                                       //!< Default measurement ID
};

/**
 * Validates Firebase configuration object
 * @param {Object} config - Firebase config object to validate
 * @returns {boolean} - True if config has required fields
 */
function isValidConfig(config) {
    return config &&
        typeof config.apiKey === 'string' &&
        config.apiKey.length > 0 &&
        typeof config.databaseURL === 'string' &&
        config.databaseURL.length > 0;
}

/**
 * Loads Firebase config from localStorage
 * @returns {Object} - Firebase configuration object
 */
function loadConfig() {
    const savedConfigString = localStorage.getItem(FIREBASE_CONFIG_KEY);

    if (savedConfigString) {
        try {
            const parsedConfig = JSON.parse(savedConfigString);
            if (isValidConfig(parsedConfig)) {
                console.log('[Firebase] Using user-configured settings');
                return parsedConfig;
            }
            console.warn('[Firebase] Saved config is invalid, using defaults');
        } catch (error) {
            console.error('[Firebase] Error parsing saved config:', error.message);
        }
    }

    return defaultConfig;
}

// Load configuration
const finalConfig = loadConfig();

// Initialize Firebase instances
let app = null;
let auth = null;
let db = null;

// Only initialize if we have a valid API key
if (isValidConfig(finalConfig)) {
    try {
        app = initializeApp(finalConfig);
        auth = getAuth(app);
        db = getDatabase(app);
        console.log('[Firebase] Successfully initialized');
    } catch (error) {
        console.error('[Firebase] Initialization error:', error.message);
        // Keep auth/db as null so the rest of the app can check
    }
} else {
    console.warn('[Firebase] Missing required config (apiKey/databaseURL). Please configure in settings.');
}

// Export Firebase instances
export { auth, db };