/**
 * helpers.js
 * Common utility functions
 */

/**
 * Escape HTML to prevent XSS attacks
 * @param {string} unsafe - Unsafe string
 * @returns {string} Escaped string
 */
export function escapeHtml(unsafe) {
    if (!unsafe) return '';
    return unsafe
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#039;");
}

/**
 * Format interval time to readable string
 * @param {number} seconds - Interval in seconds
 * @returns {string} Formatted interval (e.g., "5s", "5m", "1h30m")
 */
export function formatInterval(seconds) {
    if (seconds < 60) {
        return `${seconds}s`;
    } else if (seconds < 3600) {
        const minutes = Math.floor(seconds / 60);
        const remainingSeconds = seconds % 60;
        return remainingSeconds > 0 ? `${minutes}m${remainingSeconds}s` : `${minutes}m`;
    } else {
        const hours = Math.floor(seconds / 3600);
        const minutes = Math.floor((seconds % 3600) / 60);
        return minutes > 0 ? `${hours}h${minutes}m` : `${hours}h`;
    }
}
