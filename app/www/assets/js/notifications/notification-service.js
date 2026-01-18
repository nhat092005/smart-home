/**
 * notification-service.js
 * Notification management module
 * Handles notification display, storage and management
 */

// Notification storage key
const STORAGE_KEY = 'smart_home_notifications';

// Maximum notifications to keep
const MAX_NOTIFICATIONS = 50;

// Notification types
export const NOTIFICATION_TYPES = {
    INFO: 'info',
    WARNING: 'warning',
    SUCCESS: 'success',
    ERROR: 'error',
    DEVICE: 'device'
};

// Local notification storage
let notifications = [];

/**
 * Initialize notification service
 */
export function initializeNotificationService() {
    loadNotifications();
    updateNotificationBadge();
    console.log('[NotificationService] Initialized with', notifications.length, 'notifications');
}

/**
 * Load notifications from localStorage
 */
function loadNotifications() {
    try {
        const stored = localStorage.getItem(STORAGE_KEY);
        if (stored) {
            notifications = JSON.parse(stored);
        }
    } catch (error) {
        console.error('[NotificationService] Error loading notifications:', error);
        notifications = [];
    }
}

/**
 * Save notifications to localStorage
 */
function saveNotifications() {
    try {
        // Keep only the latest MAX_NOTIFICATIONS
        if (notifications.length > MAX_NOTIFICATIONS) {
            notifications = notifications.slice(-MAX_NOTIFICATIONS);
        }
        localStorage.setItem(STORAGE_KEY, JSON.stringify(notifications));
    } catch (error) {
        console.error('[NotificationService] Error saving notifications:', error);
    }
}

/**
 * Add a new notification
 * @param {Object} notification - Notification object
 * @param {string} notification.type - Notification type (info/warning/success/error/device)
 * @param {string} notification.title - Notification title
 * @param {string} notification.message - Notification message
 * @param {string} notification.deviceId - Optional device ID
 */
export function addNotification({ type, title, message, deviceId = null }) {
    const notification = {
        id: generateId(),
        type: type || NOTIFICATION_TYPES.INFO,
        title,
        message,
        deviceId,
        timestamp: Date.now(),
        read: false
    };
    
    notifications.push(notification);
    saveNotifications();
    updateNotificationBadge();
    
    // If notification view is open, refresh it
    refreshNotificationView();
    
    console.log('[NotificationService] Added notification:', notification);
    return notification;
}

/**
 * Add device online notification
 * @param {string} deviceId - Device ID
 * @param {string} deviceName - Device name
 */
export function notifyDeviceOnline(deviceId, deviceName) {
    addNotification({
        type: NOTIFICATION_TYPES.SUCCESS,
        title: 'Device Online',
        message: `${deviceName || deviceId} is now online`,
        deviceId
    });
}

/**
 * Add device offline notification
 * @param {string} deviceId - Device ID
 * @param {string} deviceName - Device name
 */
export function notifyDeviceOffline(deviceId, deviceName) {
    addNotification({
        type: NOTIFICATION_TYPES.WARNING,
        title: 'Device Offline',
        message: `${deviceName || deviceId} went offline`,
        deviceId
    });
}

/**
 * Add sensor alert notification
 * @param {string} deviceId - Device ID
 * @param {string} sensorType - Sensor type (temperature/humidity/light)
 * @param {number} value - Sensor value
 * @param {string} condition - Alert condition (high/low)
 */
export function notifySensorAlert(deviceId, sensorType, value, condition) {
    const icons = {
        temperature: '🌡️',
        humidity: '💧',
        light: '☀️'
    };
    
    addNotification({
        type: NOTIFICATION_TYPES.WARNING,
        title: `${sensorType.charAt(0).toUpperCase() + sensorType.slice(1)} Alert`,
        message: `${icons[sensorType] || ''} ${sensorType} is ${condition}: ${value}`,
        deviceId
    });
}

/**
 * Mark notification as read
 * @param {string} notificationId - Notification ID
 */
export function markAsRead(notificationId) {
    const notification = notifications.find(n => n.id === notificationId);
    if (notification) {
        notification.read = true;
        saveNotifications();
        updateNotificationBadge();
    }
}

/**
 * Mark all notifications as read
 */
export function markAllAsRead() {
    notifications.forEach(n => n.read = true);
    saveNotifications();
    updateNotificationBadge();
}

/**
 * Delete a notification
 * @param {string} notificationId - Notification ID
 */
export function deleteNotification(notificationId) {
    notifications = notifications.filter(n => n.id !== notificationId);
    saveNotifications();
    updateNotificationBadge();
    refreshNotificationView();
}

/**
 * Clear all notifications
 */
export function clearAllNotifications() {
    notifications = [];
    saveNotifications();
    updateNotificationBadge();
    refreshNotificationView();
}

/**
 * Get all notifications
 * @returns {Array} Array of notifications (newest first)
 */
export function getAllNotifications() {
    return [...notifications].reverse();
}

/**
 * Get unread notification count
 * @returns {number} Unread count
 */
export function getUnreadCount() {
    return notifications.filter(n => !n.read).length;
}

/**
 * Update notification badge in bottom nav
 */
function updateNotificationBadge() {
    const badge = document.getElementById('notification-badge');
    if (!badge) return;
    
    const unreadCount = getUnreadCount();
    
    if (unreadCount > 0) {
        badge.textContent = unreadCount > 99 ? '99+' : unreadCount;
        badge.style.display = 'flex';
    } else {
        badge.style.display = 'none';
    }
}

/**
 * Refresh notification view if visible
 */
function refreshNotificationView() {
    const notificationView = document.getElementById('notification-view');
    if (notificationView && notificationView.style.display !== 'none') {
        renderNotificationList();
    }
}

/**
 * Render notification list
 */
export function renderNotificationList() {
    const listContainer = document.getElementById('notification-list');
    if (!listContainer) return;
    
    const allNotifications = getAllNotifications();
    
    if (allNotifications.length === 0) {
        listContainer.innerHTML = `
            <div class="notification-empty">
                <i class="fa-regular fa-bell-slash"></i>
                <p>No notifications yet</p>
            </div>
        `;
        return;
    }
    
    listContainer.innerHTML = allNotifications.map(notification => {
        const timeAgo = formatTimeAgo(notification.timestamp);
        const unreadClass = notification.read ? '' : 'unread';
        
        return `
            <div class="notification-item ${unreadClass}" data-id="${notification.id}">
                <div class="notification-icon ${notification.type}">
                    <i class="${getNotificationIcon(notification.type)}"></i>
                </div>
                <div class="notification-content">
                    <div class="notification-text">
                        <strong>${escapeHtml(notification.title)}</strong><br>
                        ${escapeHtml(notification.message)}
                    </div>
                    <div class="notification-time">${timeAgo}</div>
                </div>
                <div class="notification-actions">
                    <button class="btn-dismiss" onclick="window.dismissNotification('${notification.id}')" title="Dismiss">
                        <i class="fa-solid fa-xmark"></i>
                    </button>
                </div>
            </div>
        `;
    }).join('');
    
    // Mark visible notifications as read after a short delay
    setTimeout(() => {
        markAllAsRead();
    }, 1000);
}

/**
 * Get icon class for notification type
 * @param {string} type - Notification type
 * @returns {string} Icon class
 */
function getNotificationIcon(type) {
    const icons = {
        info: 'fa-solid fa-info',
        warning: 'fa-solid fa-triangle-exclamation',
        success: 'fa-solid fa-check',
        error: 'fa-solid fa-xmark',
        device: 'fa-solid fa-microchip'
    };
    return icons[type] || icons.info;
}

/**
 * Format timestamp to relative time
 * @param {number} timestamp - Unix timestamp
 * @returns {string} Relative time string
 */
function formatTimeAgo(timestamp) {
    const now = Date.now();
    const diff = now - timestamp;
    
    const seconds = Math.floor(diff / 1000);
    const minutes = Math.floor(seconds / 60);
    const hours = Math.floor(minutes / 60);
    const days = Math.floor(hours / 24);
    
    if (days > 0) return `${days} day${days > 1 ? 's' : ''} ago`;
    if (hours > 0) return `${hours} hour${hours > 1 ? 's' : ''} ago`;
    if (minutes > 0) return `${minutes} min${minutes > 1 ? 's' : ''} ago`;
    return 'Just now';
}

/**
 * Generate unique ID
 * @returns {string} Unique ID
 */
function generateId() {
    return 'notif_' + Date.now().toString(36) + Math.random().toString(36).substr(2, 5);
}

/**
 * Escape HTML to prevent XSS
 * @param {string} unsafe - Unsafe string
 * @returns {string} Escaped string
 */
function escapeHtml(unsafe) {
    if (!unsafe) return '';
    return unsafe
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#039;");
}
