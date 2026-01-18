/**
 * data-export.js
 * Data export module
 * Handles fetching and exporting historical data
 */

import { db } from '../core/firebase-config.js';
import { ref, get } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js";
import { escapeHtml } from '../utils/helpers.js';

/**
 * Fetch all historical data from Firebase
 * @returns {Promise<Array>} Promise that resolves with history data array
 */
export async function fetchAllHistoryData() {
    if (!db) {
        throw new Error('Firebase not initialized');
    }

    try {
        // Fetch history data from /history/{deviceId}/records
        const historyRef = ref(db, 'history');
        const historySnapshot = await get(historyRef);

        // Fetch device names from /devices
        const devicesRef = ref(db, 'devices');
        const devicesSnapshot = await get(devicesRef);

        if (!historySnapshot.exists()) {
            console.log('[Export] No history data found');
            return [];
        }

        // Build device name map
        const deviceNames = {};
        if (devicesSnapshot.exists()) {
            const devicesData = devicesSnapshot.val();
            Object.entries(devicesData).forEach(([deviceId, device]) => {
                deviceNames[deviceId] = device.name || deviceId;
            });
        }

        const historyData = historySnapshot.val();
        const flatData = [];

        // Flatten nested structure: history/{deviceId}/records/{pushId}
        Object.entries(historyData).forEach(([deviceId, deviceHistory]) => {
            if (deviceHistory && deviceHistory.records && typeof deviceHistory.records === 'object') {
                Object.values(deviceHistory.records).forEach(entry => {
                    flatData.push({
                        deviceId: deviceId,
                        deviceName: deviceNames[deviceId] || deviceId,
                        timestamp: entry.timestamp || Date.now(),
                        temperature: parseFloat(entry.temperature) || 0,
                        humidity: parseFloat(entry.humidity) || 0,
                        light: parseInt(entry.light) || 0
                    });
                });
            }
        });

        // Sort by timestamp (newest first)
        flatData.sort((a, b) => b.timestamp - a.timestamp);

        console.log('[Export] Fetched history entries:', flatData.length);
        return flatData;

    } catch (error) {
        console.error('[Export] Fetch error:', error);
        throw error;
    }
}

/**
 * Render history data table
 * @param {Array} data - History data array
 */
export function renderHistoryTable(data) {
    const tableBody = document.getElementById('table-body');

    if (!tableBody) {
        console.error('[Export] Table body element not found');
        return;
    }

    if (!data || data.length === 0) {
        tableBody.innerHTML = `
            <tr>
                <td colspan="6" class="table-no-data">
                    No data available
                </td>
            </tr>
        `;
        return;
    }

    const rows = data.map((entry, index) => {
        const date = new Date(entry.timestamp);
        const dateStr = date.toLocaleString('en-US');

        return `
            <tr>
                <td class="table-cell-center">${index + 1}</td>
                <td>${escapeHtml(entry.deviceName)}</td>
                <td class="table-cell-center">${dateStr}</td>
                <td class="table-cell-center">${entry.temperature.toFixed(1)}</td>
                <td class="table-cell-center">${entry.humidity.toFixed(1)}</td>
                <td class="table-cell-center">${entry.light}</td>
            </tr>
        `;
    }).join('');

    tableBody.innerHTML = rows;
}

/**
 * Export table data to Excel (CSV format)
 */
export function exportTableToExcel() {
    const table = document.querySelector('.data-table');

    if (!table) {
        alert('Data table not found!');
        return;
    }

    let csv = [];
    const rows = table.querySelectorAll('tr');

    // Process each row
    rows.forEach(row => {
        const cols = row.querySelectorAll('td, th');
        const csvRow = [];

        cols.forEach(col => {
            // Clean and escape text
            let text = col.textContent.trim();
            text = text.replace(/"/g, '""'); // Escape quotes
            csvRow.push(`"${text}"`);
        });

        csv.push(csvRow.join(','));
    });

    // Create CSV content
    const csvContent = csv.join('\n');

    // Add BOM for Excel UTF-8 support
    const BOM = '\uFEFF';
    const blob = new Blob([BOM + csvContent], { type: 'text/csv;charset=utf-8;' });

    // Create download link
    const link = document.createElement('a');
    const url = URL.createObjectURL(blob);

    link.setAttribute('href', url);
    link.setAttribute('download', `SmartHome_Data_${Date.now()}.csv`);
    link.style.visibility = 'hidden';

    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);

    console.log('[Export] Data exported to CSV');
}

// Store all data and filtered data
let allHistoryData = [];
let filteredData = [];

/**
 * Filter data by date range
 * @param {Array} data - History data array
 * @param {Date} fromDate - Start date
 * @param {Date} toDate - End date
 * @returns {Array} Filtered data
 */
export function filterDataByDateRange(data, fromDate, toDate) {
    if (!fromDate && !toDate) {
        return data;
    }

    const fromTimestamp = fromDate ? fromDate.getTime() : 0;
    const toTimestamp = toDate ? toDate.setHours(23, 59, 59, 999) : Date.now();

    return data.filter(entry => {
        return entry.timestamp >= fromTimestamp && entry.timestamp <= toTimestamp;
    });
}

/**
 * Apply filters and render table
 */
export async function applyFilters() {
    try {
        // Fetch all data if not already loaded
        if (allHistoryData.length === 0) {
            allHistoryData = await fetchAllHistoryData();
        }

        const fromDateInput = document.getElementById('filter-from-date');
        const toDateInput = document.getElementById('filter-to-date');

        const fromDate = fromDateInput.value ? new Date(fromDateInput.value) : null;
        const toDate = toDateInput.value ? new Date(toDateInput.value) : null;

        filteredData = filterDataByDateRange(allHistoryData, fromDate, toDate);

        renderHistoryTable(filteredData);

        console.log(`[Export] Filtered ${filteredData.length} / ${allHistoryData.length} entries`);

    } catch (error) {
        console.error('[Export] Apply filters error:', error);
        alert('Error applying filters: ' + error.message);
    }
}

/**
 * Reset filters to default
 */
export function resetFilters() {
    document.getElementById('filter-from-date').value = '';
    document.getElementById('filter-to-date').value = '';

    filteredData = allHistoryData;
    renderHistoryTable(filteredData);

    console.log('[Export] Filters reset');
}

/**
 * Set quick filter (Today, This Week, This Month, All Time)
 * @param {string} filterType - Filter type
 */
export function setQuickFilter(filterType) {
    const now = new Date();
    const today = new Date(now.getFullYear(), now.getMonth(), now.getDate());

    let fromDate, toDate;

    switch (filterType) {
        case 'today':
            fromDate = today;
            toDate = now;
            break;

        case 'week':
            // Start of week (Monday)
            const dayOfWeek = now.getDay();
            const diffToMonday = dayOfWeek === 0 ? -6 : 1 - dayOfWeek;
            fromDate = new Date(today);
            fromDate.setDate(today.getDate() + diffToMonday);
            toDate = now;
            break;

        case 'month':
            fromDate = new Date(now.getFullYear(), now.getMonth(), 1);
            toDate = now;
            break;

        case 'all':
            fromDate = null;
            toDate = null;
            break;

        default:
            return;
    }

    // Update date inputs
    const fromDateInput = document.getElementById('filter-from-date');
    const toDateInput = document.getElementById('filter-to-date');

    if (fromDate) {
        fromDateInput.value = fromDate.toISOString().split('T')[0];
    } else {
        fromDateInput.value = '';
    }

    if (toDate && filterType !== 'all') {
        toDateInput.value = toDate.toISOString().split('T')[0];
    } else {
        toDateInput.value = '';
    }

    // Apply filters automatically
    applyFilters();
}

/**
 * Export filtered data to Excel
 */
export function exportFilteredData() {
    if (filteredData.length === 0) {
        alert('No data to export. Please apply filters first.');
        return;
    }

    // Create CSV header
    const headers = ['No.', 'Room', 'Date & Time', 'Temperature (°C)', 'Humidity (%)', 'Light (Lux)'];
    let csv = [headers.map(h => `"${h}"`).join(',')];

    // Add data rows
    filteredData.forEach((entry, index) => {
        const date = new Date(entry.timestamp);
        const dateStr = date.toLocaleString('en-US');

        const row = [
            index + 1,
            entry.deviceName,
            dateStr,
            entry.temperature.toFixed(1),
            entry.humidity.toFixed(1),
            entry.light
        ];

        csv.push(row.map(cell => `"${cell}"`).join(','));
    });

    // Create CSV content
    const csvContent = csv.join('\n');

    // Add BOM for Excel UTF-8 support
    const BOM = '\uFEFF';
    const blob = new Blob([BOM + csvContent], { type: 'text/csv;charset=utf-8;' });

    // Create download link
    const link = document.createElement('a');
    const url = URL.createObjectURL(blob);

    const timestamp = new Date().toISOString().split('T')[0];
    link.setAttribute('href', url);
    link.setAttribute('download', `SmartHome_Filtered_Data_${timestamp}.csv`);
    link.style.visibility = 'hidden';

    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);

    console.log(`[Export] Exported ${filteredData.length} filtered entries to CSV`);
}
