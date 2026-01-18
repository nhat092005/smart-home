/**
 * auth.js
 * Authentication utility functions for the Smart Home dashboard
 * Handles user session management and authentication state
 */

import { auth } from './firebase-config.js';
import { onAuthStateChanged, signOut } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-auth.js";

/**
 * Requires user authentication to access the page
 * Redirects to login page if user is not authenticated
 * Updates UI with user email when authenticated
 */
export function requireAuth() {
    // Check if Firebase auth is initialized
    if (!auth) {
        console.error('[Auth] Firebase auth not initialized');
        window.location.href = "login.html";
        return;
    }

    onAuthStateChanged(auth, (user) => {
        if (!user) {
            console.log('[Auth] No user session, redirecting to login');
            window.location.href = "login.html";
        } else {
            console.log('[Auth] User authenticated:', user.email);
            updateUserDisplay(user.email);
        }
    });
}

/**
 * Updates the user email display in the header
 * @param {string} email - User's email address
 */
function updateUserDisplay(email) {
    const userInfo = document.getElementById('user-email-display');
    if (userInfo) {
        userInfo.textContent = email || 'Unknown User';
    }
}

/**
 * Signs out the current user
 * @returns {Promise} - Promise that resolves when sign out is complete
 */
export function logout() {
    // Check if Firebase auth is initialized
    if (!auth) {
        console.error('[Auth] Firebase auth not initialized');
        return Promise.reject(new Error('Auth not initialized'));
    }

    return signOut(auth)
        .then(() => {
            console.log('[Auth] User signed out successfully');
        })
        .catch((error) => {
            console.error('[Auth] Sign out error:', error.message);
            throw error;
        });
}