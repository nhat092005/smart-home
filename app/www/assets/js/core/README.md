# Core Module

## Overview

Core application module handling Firebase initialization, authentication, and login functionality for the mobile application.

## Files

- `firebase-config.js` - Firebase configuration
- `auth.js` - Authentication management
- `login.js` - Login page logic

## firebase-config.js

### Purpose

Initializes Firebase SDK with project credentials and exports database and authentication references.

### Configuration

```javascript
const firebaseConfig = {
    apiKey: "YOUR_API_KEY",
    authDomain: "your-project.firebaseapp.com",
    databaseURL: "https://your-project.firebaseio.com",
    projectId: "your-project",
    storageBucket: "your-project.appspot.com",
    messagingSenderId: "123456789",
    appId: "1:123456789:web:abcdef"
};

firebase.initializeApp(firebaseConfig);
```

### Exports

```javascript
export const db = firebase.database();
export const auth = firebase.auth();
```

## auth.js

### Purpose

Manages user authentication state, session persistence, and route protection.

### Functions

#### requireAuth()
```javascript
requireAuth()
```
Redirects to login if user not authenticated. Call on protected pages.

**Usage:**
```javascript
import { requireAuth } from './auth.js';

// Check auth on page load
requireAuth();
```

#### getCurrentUser()
```javascript
getCurrentUser()
```
Returns current Firebase user or null.

**Returns:** User object or null

#### signOut()
```javascript
signOut()
```
Signs out current user and redirects to login.

### Auth State Listener

```javascript
auth.onAuthStateChanged((user) => {
    if (user) {
        console.log('User logged in:', user.email);
    } else {
        console.log('User logged out');
    }
});
```

## login.js

### Purpose

Handles login page UI interactions, form validation, and authentication requests.

### Functions

#### handleLogin()
```javascript
handleLogin(email, password)
```
Authenticates user with email and password.

**Parameters:**
- `email` - User email
- `password` - User password

**Returns:** Promise resolving to user or error

**Example:**
```javascript
const email = document.getElementById('email').value;
const password = document.getElementById('password').value;

await handleLogin(email, password);
// Redirects to dashboard on success
```

#### handleSignUp()
```javascript
handleSignUp(email, password)
```
Creates new user account.

**Parameters:**
- `email` - User email
- `password` - User password

**Returns:** Promise resolving to user or error

#### showError()
```javascript
showError(message)
```
Displays error message on login page.

**Parameters:**
- `message` - Error message text

### Event Handlers

```javascript
document.getElementById('login-form').addEventListener('submit', async (e) => {
    e.preventDefault();
    const email = document.getElementById('email').value;
    const password = document.getElementById('password').value;
    
    try {
        await handleLogin(email, password);
        window.location.href = 'index.html';
    } catch (error) {
        showError(error.message);
    }
});
```

## Authentication Flow

### Login Process
```
User enters credentials → handleLogin() → Firebase Auth
    ↓
Success → Store session → Redirect to dashboard
    ↓
Error → Display error message
```

### Protected Page Access
```
Page load → requireAuth() → Check auth state
    ↓
Authenticated → Continue loading
    ↓
Not authenticated → Redirect to login
```

### Session Management
```
Firebase Auth → Automatic session persistence
    ↓
LocalStorage stores auth token
    ↓
Auto-refresh on token expiration
```

## Error Handling

### Login Errors
```javascript
try {
    await handleLogin(email, password);
} catch (error) {
    switch (error.code) {
        case 'auth/user-not-found':
            showError('No account found with this email');
            break;
        case 'auth/wrong-password':
            showError('Incorrect password');
            break;
        case 'auth/invalid-email':
            showError('Invalid email format');
            break;
        default:
            showError('Login failed. Please try again.');
    }
}
```

### Network Errors
```javascript
if (error.code === 'auth/network-request-failed') {
    showError('Network error. Check your connection.');
}
```

## Security

### Password Requirements
```javascript
function validatePassword(password) {
    return password.length >= 6;  // Firebase minimum
}
```

### Email Validation
```javascript
function validateEmail(email) {
    const pattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return pattern.test(email);
}
```

## Session Persistence

### Auto Login
```javascript
// User stays logged in across app restarts
auth.setPersistence(firebase.auth.Auth.Persistence.LOCAL);
```

### Manual Logout
```javascript
document.getElementById('logout-btn').addEventListener('click', async () => {
    await signOut();
    window.location.href = 'login.html';
});
```

## UI Integration

### Login Form HTML
```html
<form id="login-form">
    <input type="email" id="email" placeholder="Email" required />
    <input type="password" id="password" placeholder="Password" required />
    <button type="submit">Sign In</button>
</form>
<div id="error-message" class="error" style="display: none;"></div>
```

### Loading State
```javascript
function setLoading(isLoading) {
    const btn = document.querySelector('button[type="submit"]');
    btn.disabled = isLoading;
    btn.textContent = isLoading ? 'Signing in...' : 'Sign In';
}
```

## Performance

- Auth state cached in memory
- Auto session refresh
- Minimal Firebase calls
- Token validation: <100ms

## Browser Support

- Android 7.0+ WebView
- Firebase JS SDK v10.7.1
- LocalStorage support

## Dependencies

- Firebase JS SDK (Authentication, Database)

## Related Documentation

- [../mqtt/README.md](../mqtt/README.md) - MQTT client
- [../devices/README.md](../devices/README.md) - Device management
- [../../README.md](../../README.md) - JavaScript overview
