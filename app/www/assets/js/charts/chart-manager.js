/**
 * chart-manager.js
 * Chart management module
 * Handles Chart.js rendering and data visualization
 */

import { db } from '../core/firebase-config.js';
import { ref, query, limitToLast, onValue } from "https://www.gstatic.com/firebasejs/10.7.1/firebase-database.js";
import { CHART_CONFIG } from './chart-config.js';

let myChartInstance = null;
let currentChartType = 'temp';
let currentDeviceId = null;
let chartListener = null;

/**
 * Initialize chart for a device
 * @param {string} deviceId - Device identifier
 * @param {string} chartType - Chart type (temp/humid/light)
 */
export function initializeChart(deviceId, chartType = 'temp') {
    currentDeviceId = deviceId;
    currentChartType = chartType;

    // Clear existing listener
    if (chartListener) {
        chartListener();
        chartListener = null;
    }

    // Setup Firebase listener for chart data
    setupChartListener(deviceId, chartType);
}

/**
 * Setup Firebase listener for chart data
 * @param {string} deviceId - Device identifier
 * @param {string} chartType - Chart type
 */
function setupChartListener(deviceId, chartType) {
    if (!db) {
        console.error('[Chart] Firebase not initialized');
        return;
    }

    // Fix: History data is at /history/{deviceId}/records
    const historyRef = query(
        ref(db, `history/${deviceId}/records`),
        limitToLast(CHART_CONFIG.MAX_DATA_POINTS)
    );

    chartListener = onValue(historyRef, (snapshot) => {
        if (!snapshot.exists()) {
            console.log('[Chart] No history data found for', deviceId);
            updateChart(chartType, [], []);
            return;
        }

        const data = snapshot.val();
        const { labels, values } = processChartData(data, chartType);

        console.log(`[Chart] Loaded ${values.length} data points for ${chartType}`);
        updateChart(chartType, labels, values);
    }, (error) => {
        console.error('[Chart] Firebase listener error:', error);
    });
}

/**
 * Process raw Firebase data for chart
 * @param {Object} data - Raw Firebase data
 * @param {string} chartType - Chart type
 * @returns {Object} Processed labels and values
 */
function processChartData(data, chartType) {
    const labels = [];
    const values = [];

    const dataField = chartType === 'temp' ? 'temperature' :
        chartType === 'humid' ? 'humidity' : 'light';

    Object.values(data).forEach(entry => {
        if (entry[dataField] !== undefined) {
            // Format timestamp for label
            const date = new Date(entry.timestamp || Date.now());
            const timeLabel = date.toLocaleTimeString('vi-VN', {
                hour: '2-digit',
                minute: '2-digit'
            });

            labels.push(timeLabel);
            values.push(entry[dataField]);
        }
    });

    return { labels, values };
}

/**
 * Update or create chart
 * @param {string} chartType - Chart type (temp/humid/light)
 * @param {Array} labels - Chart labels
 * @param {Array} values - Chart values
 */
function updateChart(chartType, labels, values) {
    const canvas = document.getElementById('myChart');
    if (!canvas) {
        console.error('[Chart] Canvas element not found');
        return;
    }

    const ctx = canvas.getContext('2d');
    const config = CHART_CONFIG.types[chartType];

    if (!config) {
        console.error('[Chart] Unknown chart type:', chartType);
        return;
    }

    // Destroy existing chart
    if (myChartInstance) {
        myChartInstance.destroy();
    }

    // Create new chart
    myChartInstance = new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [{
                label: config.label,
                data: values,
                borderColor: config.color,
                backgroundColor: config.bgColor,
                borderWidth: 2,
                tension: 0.4,
                fill: true
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            animation: CHART_CONFIG.animation,
            plugins: {
                legend: {
                    display: true,
                    position: 'top'
                },
                title: {
                    display: true,
                    text: config.title
                }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    title: {
                        display: true,
                        text: config.unit
                    }
                },
                x: {
                    title: {
                        display: true,
                        text: 'Time'
                    }
                }
            }
        }
    });
}

/**
 * Switch chart type
 * @param {string} newType - New chart type (temp/humid/light)
 */
export function switchChartType(newType) {
    if (!currentDeviceId) {
        console.error('[Chart] No device selected');
        return;
    }

    currentChartType = newType;

    // Update button active states
    updateChartButtonStates(newType);

    // Reinitialize chart with new type
    initializeChart(currentDeviceId, newType);
}

/**
 * Update chart button active states
 * @param {string} activeType - Active chart type
 */
function updateChartButtonStates(activeType) {
    const buttons = {
        temp: document.getElementById('btn-chart-temp'),
        humid: document.getElementById('btn-chart-humid'),
        light: document.getElementById('btn-chart-light')
    };

    // Remove active class from all
    Object.values(buttons).forEach(btn => {
        if (btn) btn.classList.remove('active-chart');
    });

    // Add active class to selected
    if (buttons[activeType]) {
        buttons[activeType].classList.add('active-chart');
    }
}

/**
 * Update current sensor values display
 * @param {string} deviceId - Device identifier
 * @param {Object} data - Sensor data
 */
export function updateSensorDisplay(deviceId, data) {
    if (deviceId !== currentDeviceId) return;

    const elements = {
        temp: document.getElementById('detail-temp'),
        humid: document.getElementById('detail-humid'),
        light: document.getElementById('detail-light')
    };

    if (elements.temp && data.temperature !== undefined) {
        elements.temp.textContent = `${data.temperature} °C`;
    }
    if (elements.humid && data.humidity !== undefined) {
        elements.humid.textContent = `${data.humidity} %`;
    }
    if (elements.light && data.light !== undefined) {
        elements.light.textContent = `${data.light} Lux`;
    }
}

/**
 * Cleanup chart resources
 */
export function cleanupChart() {
    if (myChartInstance) {
        myChartInstance.destroy();
        myChartInstance = null;
    }

    if (chartListener) {
        chartListener();
        chartListener = null;
    }

    currentDeviceId = null;
    currentChartType = 'temp';
}

/**
 * Get current chart state
 * @returns {Object} Current chart state
 */
export function getChartState() {
    return {
        deviceId: currentDeviceId,
        chartType: currentChartType,
        isActive: myChartInstance !== null
    };
}
