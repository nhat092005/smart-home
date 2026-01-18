/**
 * login.js
 * Login page logic for Smart Home application
 * Handles user authentication (login/register)
 */

import { auth } from './firebase-config.js';
import {
    signInWithEmailAndPassword,
    createUserWithEmailAndPassword,
    onAuthStateChanged
} from "https://www.gstatic.com/firebasejs/10.7.1/firebase-auth.js";

// DOM Elements
const authForm = document.getElementById('auth-form');
const emailInput = document.getElementById('email');
const passwordInput = document.getElementById('password');
const confirmPasswordInput = document.getElementById('confirm-password');
const title = document.getElementById('form-title');
const submitBtn = document.getElementById('submit-btn');
const toggleBtn = document.getElementById('toggle-btn');
const errorMsg = document.getElementById('error-message');
const toggleMsg = document.getElementById('toggle-msg');

// State: true = login mode, false = register mode
let isLoginMode = true;

/**
 * Initialize authentication state listener
 * Redirects to dashboard if user is already logged in
 */
function initAuthStateListener() {
    // Check if Firebase auth is available
    if (!auth) {
        console.warn('[Login] Firebase auth not initialized. Please configure Firebase.');
        showError({ message: 'Firebase not configured. Please set up Firebase Key.' });
        return;
    }

    onAuthStateChanged(auth, (user) => {
        if (user) {
            console.log('[Login] User already authenticated, redirecting to dashboard');
            window.location.href = "index.html";
        }
    });
}

// Initialize auth state listener
initAuthStateListener();

/**
 * Toggle between login and register modes
 */
function toggleAuthMode() {
    isLoginMode = !isLoginMode;
    hideError();

    if (isLoginMode) {
        title.textContent = "Sign In";
        submitBtn.textContent = "Sign In";
        confirmPasswordInput.style.display = "none";
        confirmPasswordInput.required = false;
        toggleMsg.textContent = "Don't have an account?";
        toggleBtn.textContent = "Sign Up Now";
    } else {
        title.textContent = "Sign Up";
        submitBtn.textContent = "Sign Up";
        confirmPasswordInput.style.display = "block";
        confirmPasswordInput.required = true;
        toggleMsg.textContent = "Already have an account?";
        toggleBtn.textContent = "Sign In Now";
    }
}

/**
 * Handle form submission for login/register
 * @param {Event} e - Form submit event
 */
async function handleAuthSubmit(e) {
    e.preventDefault();

    // Check if Firebase auth is available
    if (!auth) {
        showError({ message: 'Firebase not configured. Please set up Firebase Key.' });
        return;
    }

    const email = emailInput.value.trim();
    const password = passwordInput.value;

    // Basic validation
    if (!email || !password) {
        showError({ message: 'Please enter both email and password.' });
        return;
    }

    // Disable submit button during request
    submitBtn.disabled = true;
    submitBtn.textContent = isLoginMode ? 'Signing in' : 'Signing up';

    try {
        if (isLoginMode) {
            // Sign In
            await signInWithEmailAndPassword(auth, email, password);
            // Redirect handled by onAuthStateChanged
        } else {
            // Sign Up - validate password confirmation
            if (password !== confirmPasswordInput.value) {
                showError({ message: "Password confirmation does not match." });
                return;
            }

            // Password strength check
            if (password.length < 6) {
                showError({ message: "Password must be at least 6 characters long." });
                return;
            }

            await createUserWithEmailAndPassword(auth, email, password);
            alert("Sign up successful!");
            // Redirect handled by onAuthStateChanged
        }
    } catch (error) {
        showError(error);
    } finally {
        // Re-enable submit button
        submitBtn.disabled = false;
        submitBtn.textContent = isLoginMode ? 'Sign In' : 'Sign Up';
    }
}

/**
 * Display error message to user
 * @param {Object} error - Error object with code and/or message
 */
function showError(error) {
    errorMsg.style.display = 'block';
    console.error('[Login] Error:', error.code || error.message);

    // Map Firebase error codes to user-friendly messages
    const errorMessages = {
        'auth/invalid-email': 'Invalid email address.',
        'auth/user-disabled': 'User account has been disabled.',
        'auth/user-not-found': 'User not found.',
        'auth/wrong-password': 'Incorrect password.',
        'auth/email-already-in-use': 'Email is already in use.',
        'auth/weak-password': 'Password is too weak. Please choose a stronger password.',
        'auth/invalid-credential': 'Email or password is incorrect.',
        'auth/too-many-requests': 'Too many failed login attempts. Please try again later.',
        'auth/network-request-failed': 'Network error. Please check your internet connection.'
    };

    errorMsg.textContent = errorMessages[error.code] || error.message || 'An unknown error occurred. Please try again.';
}

/**
 * Hide error message
 */
function hideError() {
    errorMsg.style.display = 'none';
    errorMsg.textContent = '';
}

// Event Listeners
if (toggleBtn) {
    toggleBtn.addEventListener('click', toggleAuthMode);
}

if (authForm) {
    authForm.addEventListener('submit', handleAuthSubmit);
}