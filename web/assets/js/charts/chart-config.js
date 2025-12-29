/**
 * chart-config.js
 * Chart configuration constants
 */

export const CHART_CONFIG = {
    MAX_DATA_POINTS: 20,    //!< Maximum points to show on chart
    animation: false,       //!< Disable animation for smooth real-time updates

    types: {
        temp: {
            label: 'Temperature',
            title: 'Temperature Chart',
            unit: '°C',
            color: 'rgb(239, 68, 68)',
            bgColor: 'rgba(239, 68, 68, 0.1)'
        },
        humid: {
            label: 'Humidity',
            title: 'Humidity Chart',
            unit: '%',
            color: 'rgb(59, 130, 246)',
            bgColor: 'rgba(59, 130, 246, 0.1)'
        },
        light: {
            label: 'Light',
            title: 'Light Chart',
            unit: 'Lux',
            color: 'rgb(234, 179, 8)',
            bgColor: 'rgba(234, 179, 8, 0.1)'
        }
    }
};
