/**
 * ui-helpers.js
 * UI utility functions module
 * Common UI operations and helpers
 */

/**
 * Update status badge
 * @param {string} elementId - Badge element ID
 * @param {string} type - Badge type (success/warning/error)
 * @param {string} text - Badge text
 */
export function updateStatusBadge(elementId, type, text) {
    const badge = document.getElementById(elementId);
    if (!badge) return;

    // Remove old classes
    badge.classList.remove('success', 'warning', 'error');

    // Add new class
    badge.classList.add(type);

    // Update text
    badge.textContent = text;
}

/**
 * Switch between tabs (dashboard, report, notification, settings)
 * @param {string} tabName - Tab name to switch to
 */
export function switchTab(tabName) {
    const tabConfig = {
        dashboard: {
            title: 'Home',
            views: ['device-grid'],
            hideViews: ['setting-view', 'notification-view'],
            viewType: 'manage' // Full controls: add, edit, power toggle
        },
        report: {
            title: 'Dashboard',
            views: ['device-grid'], // Reuse device-grid with dashboard viewType
            hideViews: ['setting-view', 'notification-view'],
            viewType: 'dashboard' // View only: chi tiết button only
        },
        notification: {
            title: 'Notifications',
            views: ['notification-view'],
            hideViews: ['device-grid', 'setting-view'],
            viewType: null
        },
        setting: {
            title: 'Settings',
            views: ['setting-view'],
            hideViews: ['device-grid', 'notification-view'],
            viewType: null
        }
    };

    const config = tabConfig[tabName];
    if (!config) {
        console.error('[UI] Unknown tab:', tabName);
        return;
    }

    // FIRST: Hide all views that should be hidden
    config.hideViews.forEach(viewId => {
        const view = document.getElementById(viewId);
        if (view) {
            view.style.display = 'none';
        }
    });

    // THEN: Show selected views
    config.views.forEach(viewId => {
        const view = document.getElementById(viewId);
        if (view) {
            // Use proper display type for each container
            if (viewId === 'device-grid') {
                view.style.display = 'grid'; // Grid container needs display:grid
            } else {
                view.style.display = 'block';
            }
        }
    });

    // Update menu active state
    document.querySelectorAll('.bottom-nav .nav-item').forEach(link => {
        link.classList.remove('active');
    });

    // Find and activate corresponding menu item
    const menuLink = document.querySelector(`.bottom-nav .nav-item[onclick*="${tabName}"]`);
    if (menuLink) {
        menuLink.classList.add('active');
    }

    console.log('[UI] Switched to tab:', tabName, 'View type:', config.viewType);

    // Return viewType for device manager to use
    return config.viewType;
}

/**
 * Show modal
 * @param {string} modalId - Modal element ID
 */
export function showModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
        modal.style.display = 'flex';
        document.body.style.overflow = 'hidden';
    }
}

/**
 * Hide modal
 * @param {string} modalId - Modal element ID
 */
export function hideModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
        modal.style.display = 'none';
        document.body.style.overflow = '';
    }
}

/**
 * Show notification/alert
 * @param {string} message - Message to show
 * @param {string} type - Type (success/error/warning/info)
 */
export function showNotification(message, type = 'info') {
    // Simple alert for now - can be enhanced with custom notification UI
    alert(message);
}

/**
 * Format timestamp to local string
 * @param {number} timestamp - Unix timestamp in milliseconds
 * @returns {string} Formatted date string
 */
export function formatTimestamp(timestamp) {
    if (!timestamp) return 'N/A';
    const date = new Date(timestamp);
    return date.toLocaleString('vi-VN');
}

/**
 * Initialize bottom navigation functionality
 * No toggle needed for bottom nav
 */
export function initializeSidebar() {
    // Bottom nav doesn't need toggle functionality
    console.log('[UI] Bottom navigation initialized');
}

/**
 * Update clock display
 */
export function updateClock() {
    const dateEl = document.getElementById('current-date');
    const timeEl = document.getElementById('current-time');

    if (!dateEl || !timeEl) return;

    const now = new Date();
    const days = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];

    const dayName = days[now.getDay()];
    const dateStr = now.toLocaleDateString('vi-VN');
    const timeStr = now.toLocaleTimeString('vi-VN');

    dateEl.textContent = `${dayName}, ${dateStr}`;
    timeEl.textContent = timeStr;
}

/**
 * Start clock update interval
 * @returns {number} Interval ID
 */
export function startClock() {
    updateClock(); // Initial update
    const intervalId = setInterval(updateClock, 1000);
    window.clockInterval = intervalId;
    return intervalId;
}

/**
 * Stop clock update interval
 */
export function stopClock() {
    if (window.clockInterval) {
        clearInterval(window.clockInterval);
        window.clockInterval = null;
    }
}
