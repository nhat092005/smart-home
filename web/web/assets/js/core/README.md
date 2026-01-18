# Core Module

## Overview

Authentication and Firebase initialization module. Handles user login, registration, session management, and Firebase service initialization.

## Files

- `firebase-config.js` - Firebase configuration and initialization
- `auth.js` - Authentication utilities and session management
- `login.js` - Login page controller

## Features

- Firebase Authentication with email/password
- User registration and login
- Session persistence
- Protected route handling
- Automatic redirection
- Logout functionality

## firebase-config.js

### Purpose

Initializes Firebase services (Authentication and Realtime Database).

### Configuration

```javascript
const firebaseConfig = {
    apiKey: "YOUR_API_KEY",
    authDomain: "YOUR_AUTH_DOMAIN",
    databaseURL: "YOUR_DATABASE_URL",
    projectId: "YOUR_PROJECT_ID",
    storageBucket: "YOUR_STORAGE_BUCKET",
    messagingSenderId: "YOUR_SENDER_ID",
    appId: "YOUR_APP_ID"
};
```

### Exports

```javascript
export const app;      // Firebase app instance
export const auth;     // Firebase auth service
export const db;       // Firebase database service
```

## auth.js

### Purpose

Provides authentication utilities and session management.

### Functions

#### requireAuth()
```javascript
requireAuth()
```
Protects routes by checking authentication status. Redirects to login if not authenticated.

#### logout()
```javascript
logout()
```
Signs out user and redirects to login page.

### Usage

```javascript
import { requireAuth, logout } from './core/auth.js';

// Protect page
requireAuth();

// Logout button
document.getElementById('logout-btn').addEventListener('click', logout);
```

## login.js

### Purpose

Controls login page functionality including login and registration forms.

### Features

- Form validation
- Email/password login
- User registration
- Error handling
- Redirect after success

### Event Handlers

#### Login Form
```javascript
loginForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const email = emailInput.value;
    const password = passwordInput.value;
    // Handle login
});
```

#### Register Form
```javascript
registerForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const email = registerEmailInput.value;
    const password = registerPasswordInput.value;
    // Handle registration
});
```

## Authentication Flow

```
Page Load → Check Auth State → Authenticated? 
                                    ↓
                              Yes → Allow Access
                                    ↓
                              No → Redirect to Login
                                    ↓
                            Login/Register
                                    ↓
                            Success? → Yes → Redirect to Dashboard
                                    ↓
                                   No → Show Error
```

## Error Handling

Common Firebase Auth errors:
- `auth/invalid-email` - Invalid email format
- `auth/user-not-found` - User does not exist
- `auth/wrong-password` - Incorrect password
- `auth/email-already-in-use` - Email already registered
- `auth/weak-password` - Password too weak (< 6 characters)

## Security

- Passwords never stored locally
- Session managed by Firebase
- Automatic token refresh
- Secure HTTPS communication with Firebase
- Protected routes with authentication check

## Dependencies

- Firebase Authentication (v10.7.1)
- Firebase Realtime Database (v10.7.1)

## Usage Example

```javascript
// In login.js
import { auth } from './firebase-config.js';
import { signInWithEmailAndPassword } from 'firebase/auth';

async function login(email, password) {
    try {
        await signInWithEmailAndPassword(auth, email, password);
        window.location.href = 'index.html';
    } catch (error) {
        console.error('Login failed:', error.message);
    }
}

// In dashboard (index.html)
import { requireAuth } from './core/auth.js';

requireAuth(); // Protect page
```

## Configuration Steps

1. Create Firebase project
2. Enable Email/Password authentication
3. Enable Realtime Database
4. Copy configuration to firebase-config.js
5. Set database rules:

```json
{
  "rules": {
    ".read": "auth != null",
    ".write": "auth != null"
  }
}
```

## Related Documentation

- [../mqtt/README.md](../mqtt/README.md) - MQTT communication
- [../devices/README.md](../devices/README.md) - Device management
