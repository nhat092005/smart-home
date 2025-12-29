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
 * Switch between tabs (dashboard, report, export, settings)
 * @param {string} tabName - Tab name to switch to
 */
export function switchTab(tabName) {
    const tabConfig = {
        dashboard: {
            title: 'Quản lý',
            views: ['device-grid'],
            hideViews: ['report-view', 'report-list', 'export-view', 'setting-view'],
            viewType: 'manage' // Full controls: add, edit, power toggle
        },
        report: {
            title: 'Dashboard',
            views: ['device-grid'], // Reuse device-grid with dashboard viewType
            hideViews: ['report-view', 'report-list', 'export-view', 'setting-view'],
            viewType: 'dashboard' // View only: chi tiết button only
        },
        export: {
            title: 'Dữ liệu',
            views: ['export-view'],
            hideViews: ['device-grid', 'report-view', 'report-list', 'setting-view'],
            viewType: null
        },
        setting: {
            title: 'Cấu hình',
            views: ['setting-view'],
            hideViews: ['device-grid', 'report-view', 'report-list', 'export-view'],
            viewType: null
        }
    };
    
    const config = tabConfig[tabName];
    if (!config) {
        console.error('[UI] Unknown tab:', tabName);
        return;
    }
    
    // Update title
    const header = document.querySelector('header h1');
    if (header) {
        header.textContent = config.title;
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
    document.querySelectorAll('.sidebar .menu a').forEach(link => {
        link.classList.remove('active');
    });
    
    // Find and activate corresponding menu item
    const menuLink = document.querySelector(`.sidebar .menu a[onclick*="${tabName}"]`);
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
 * Initialize sidebar toggle functionality
 */
export function initializeSidebar() {
    const sidebar = document.getElementById('sidebar');
    const mainContent = document.querySelector('.main-content');
    const toggleBtn = document.getElementById('sidebar-toggle');
    
    if (!toggleBtn || !sidebar || !mainContent) {
        console.error('[UI] Sidebar elements not found');
        return;
    }
    
    toggleBtn.addEventListener('click', () => {
        sidebar.classList.toggle('collapsed');
        mainContent.classList.toggle('expanded');
        console.log('[UI] Sidebar toggled');
    });
}

/**
 * Update clock display
 */
export function updateClock() {
    const dateEl = document.getElementById('current-date');
    const timeEl = document.getElementById('current-time');
    
    if (!dateEl || !timeEl) return;
    
    const now = new Date();
    const days = ['Chủ Nhật', 'Thứ Hai', 'Thứ Ba', 'Thứ Tư', 'Thứ Năm', 'Thứ Sáu', 'Thứ Bảy'];
    
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
