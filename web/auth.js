import { auth } from '../firebase-config.js';
import {
    signInWithEmailAndPassword,
    signOut,
    onAuthStateChanged,
    browserLocalPersistence,
    setPersistence
} from "https://www.gstatic.com/firebasejs/10.7.1/firebase-auth.js";

// Authentication State
let currentUser = null;

// Initialize Auth Listener
export function initAuth(onUserChanged) {
    // Check for demo user in localStorage
    const demoUser = localStorage.getItem('demoUser');
    if (demoUser) {
        currentUser = JSON.parse(demoUser);
        if (onUserChanged) onUserChanged(currentUser);
        return;
    }

    if (!auth) return;

    onAuthStateChanged(auth, (user) => {
        currentUser = user;
        if (user) {
            console.log("User signed in:", user.email);
            localStorage.setItem('user', JSON.stringify({
                uid: user.uid,
                email: user.email,
                displayName: user.displayName || 'Admin'
            }));
        } else {
            console.log("User signed out");
            localStorage.removeItem('user');
        }

        if (onUserChanged) {
            onUserChanged(user);
        }
    });
}

// Login Function
export async function login(email, password, rememberMe = false) {
    if (!auth) throw new Error("Firebase not initialized");

    try {
        const persistence = browserLocalPersistence;
        await setPersistence(auth, persistence);

        const userCredential = await signInWithEmailAndPassword(auth, email, password);
        return userCredential.user;
    } catch (error) {
        console.error("Login error:", error.code, error.message);
        throw error;
    }
}

// Demo Login Function
export async function loginDemo() {
    const demoUser = {
        uid: 'demo-123',
        email: 'demo@smarthome.com',
        displayName: 'Demo User',
        isAnonymous: true
    };

    localStorage.setItem('demoUser', JSON.stringify(demoUser));
    localStorage.setItem('user', JSON.stringify(demoUser));
    currentUser = demoUser;
    return demoUser;
}

// Logout Function
export async function logout() {
    // Clear demo user
    if (localStorage.getItem('demoUser')) {
        localStorage.removeItem('demoUser');
        localStorage.removeItem('user');
        window.location.href = 'login.html';
        return;
    }

    if (!auth) return;
    try {
        await signOut(auth);
        window.location.href = 'login.html';
    } catch (error) {
        console.error("Logout error:", error);
    }
}

// Check if user is authenticated (helper for protected pages)
export function requireAuth() {
    const user = localStorage.getItem('user');
    if (!user) {
        window.location.href = 'login.html';
    }
}

// Get current user
export function getCurrentUser() {
    return currentUser || JSON.parse(localStorage.getItem('user'));
}
