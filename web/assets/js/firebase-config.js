// Import Firebase SDKs from CDN
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-app.js";
import { getAuth } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-auth.js";
import { getDatabase } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js";

// Default Configuration (Placeholders)
const defaultConfig = {
    apiKey: "YOUR_API_KEY",
    authDomain: "YOUR_PROJECT_ID.firebaseapp.com",
    databaseURL: "https://YOUR_PROJECT_ID-default-rtdb.firebaseio.com",
    projectId: "YOUR_PROJECT_ID",
    storageBucket: "YOUR_PROJECT_ID.appspot.com",
    messagingSenderId: "YOUR_SENDER_ID",
    appId: "YOUR_APP_ID"
};

// Try to get config from localStorage
const savedConfig = localStorage.getItem('firebaseConfig');
const firebaseConfig = savedConfig ? JSON.parse(savedConfig) : defaultConfig;

// Initialize Firebase
let app;
let auth;
let db;

try {
    app = initializeApp(firebaseConfig);
    auth = getAuth(app);
    db = getDatabase(app);
    console.log("Firebase initialized successfully");
} catch (error) {
    console.error("Error initializing Firebase:", error);
    console.warn("Using mock mode or invalid configuration. Please configure Firebase in Settings.");
}

// Export services
export { auth, db, firebaseConfig };

// Helper to save new config
export function saveFirebaseConfig(config) {
    localStorage.setItem('firebaseConfig', JSON.stringify(config));
    window.location.reload(); // Reload to apply changes
}
