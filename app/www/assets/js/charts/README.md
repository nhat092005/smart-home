# Charts Module

## Overview

Data visualization module using Chart.js for displaying real-time sensor data in device detail modal.

## Files

- `chart-manager.js` - Chart initialization and updates

## chart-manager.js

### Purpose

Manages Chart.js instance for device sensor data visualization with chart type switching.

### Functions

#### initializeChart()
```javascript
initializeChart(deviceId, chartType)
```
Creates Chart.js chart for device data.

**Parameters:**
- `deviceId` - Device identifier
- `chartType` - 'temp', 'hum', or 'light'

**Example:**
```javascript
import { initializeChart } from './chart-manager.js';

initializeChart('device001', 'temp');
```

#### switchChartType()
```javascript
switchChartType(deviceId, chartType)
```
Changes chart to display different sensor type.

**Parameters:**
- `deviceId` - Device identifier
- `chartType` - 'temp', 'hum', or 'light'

**Example:**
```javascript
document.getElementById('btn-temp').addEventListener('click', () => {
    switchChartType('device001', 'temp');
    updateActiveButton('btn-temp');
});
```

#### updateChart()
```javascript
updateChart(newData)
```
Updates chart with new data point.

**Parameters:**
- `newData` - New sensor reading

**Internal function, called automatically by Firebase listener.**

#### destroyChart()
```javascript
destroyChart()
```
Cleans up chart instance and removes listeners.

**Call when closing modal.**

### Chart Configuration

```javascript
const chartConfig = {
    type: 'line',
    data: {
        labels: [],  // Timestamps
        datasets: [{
            label: 'Temperature (°C)',
            data: [],
            borderColor: '#ff6384',
            backgroundColor: 'rgba(255, 99, 132, 0.1)',
            tension: 0.4,
            fill: true
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: {
            duration: 400
        },
        plugins: {
            legend: {
                display: true,
                position: 'top'
            },
            tooltip: {
                mode: 'index',
                intersect: false
            }
        },
        scales: {
            x: {
                grid: {
                    display: false
                }
            },
            y: {
                beginAtZero: true,
                grid: {
                    color: 'rgba(255, 255, 255, 0.1)'
                }
            }
        }
    }
};
```

## Chart Types

### Temperature Chart
```javascript
{
    label: 'Temperature (°C)',
    borderColor: '#ff6384',
    backgroundColor: 'rgba(255, 99, 132, 0.1)',
    yAxisMin: 0,
    yAxisMax: 50
}
```

### Humidity Chart
```javascript
{
    label: 'Humidity (%)',
    borderColor: '#36a2eb',
    backgroundColor: 'rgba(54, 162, 235, 0.1)',
    yAxisMin: 0,
    yAxisMax: 100
}
```

### Light Chart
```javascript
{
    label: 'Light (lux)',
    borderColor: '#ffce56',
    backgroundColor: 'rgba(255, 206, 86, 0.1)',
    yAxisMin: 0,
    yAxisMax: 1000
}
```

## Data Management

### Last 10 Readings
```javascript
function updateChart(newData) {
    chartData.labels.push(newData.timestamp);
    chartData.datasets[0].data.push(newData.value);
    
    // Keep only last 10
    if (chartData.labels.length > 10) {
        chartData.labels.shift();
        chartData.datasets[0].data.shift();
    }
    
    chart.update();
}
```

### Data Fetching
```javascript
async function loadChartData(deviceId, sensorType) {
    const ref = firebase.database().ref(`history/${deviceId}`);
    const snapshot = await ref.limitToLast(10).once('value');
    
    const data = [];
    snapshot.forEach(child => {
        const value = child.val()[sensorType];
        const timestamp = child.val().datetime;
        data.push({ timestamp, value });
    });
    
    return data;
}
```

## Chart UI

### HTML Container
```html
<div class="report-chart">
    <div class="chart-buttons">
        <button id="btn-temp" class="chart-btn active">Temp</button>
        <button id="btn-hum" class="chart-btn">Humidity</button>
        <button id="btn-light" class="chart-btn">Light</button>
    </div>
    <canvas id="sensor-chart"></canvas>
</div>
```

### Button Controls
```javascript
document.querySelectorAll('.chart-btn').forEach(btn => {
    btn.addEventListener('click', () => {
        const chartType = btn.id.replace('btn-', '');
        switchChartType(currentDeviceId, chartType);
        
        // Update active state
        document.querySelectorAll('.chart-btn').forEach(b => {
            b.classList.remove('active');
        });
        btn.classList.add('active');
    });
});
```

## Real-time Updates

### Firebase Listener
```javascript
function setupChartListener(deviceId) {
    const ref = firebase.database().ref(`devices/${deviceId}`);
    
    ref.on('value', (snapshot) => {
        const data = snapshot.val();
        const currentType = getCurrentChartType();
        
        const newData = {
            timestamp: data.datetime,
            value: data[currentType]
        };
        
        updateChart(newData);
    });
}
```

### Update Throttling
```javascript
let lastUpdate = 0;
function throttledUpdate(data) {
    const now = Date.now();
    if (now - lastUpdate > 1000) {  // Max once per second
        updateChart(data);
        lastUpdate = now;
    }
}
```

## Chart Initialization Flow

```
Modal opens → initializeChart() → Fetch last 10 records
    ↓
Create Chart.js instance → Display chart
    ↓
Setup Firebase listener → Real-time updates
    ↓
Modal closes → destroyChart() → Remove listeners
```

## Performance

- Chart initialization: <100ms
- Data update: <50ms
- Animation: 400ms (smooth)
- Memory per chart: ~2MB
- Max data points: 10 (rolling window)

## Responsive Design

```javascript
window.addEventListener('resize', () => {
    if (chart) {
        chart.resize();
    }
});
```

## Error Handling

### No Data Available
```javascript
if (data.length === 0) {
    showEmptyState('No sensor data available');
    return;
}
```

### Invalid Data
```javascript
function validateChartData(data) {
    return data.filter(d => {
        return d.value !== null && 
               d.value !== undefined && 
               !isNaN(d.value);
    });
}
```

## Chart Cleanup

```javascript
function destroyChart() {
    if (chart) {
        chart.destroy();
        chart = null;
    }
    
    // Remove Firebase listener
    if (chartListener) {
        chartListener.off();
        chartListener = null;
    }
}
```

## Canvas Size

```css
#sensor-chart {
    max-height: 300px;
    width: 100%;
}

.report-chart {
    padding: 16px;
    background: var(--bg-secondary);
    border-radius: 8px;
}
```

## Browser Support

- Android 7.0+ WebView
- HTML5 Canvas
- Chart.js v3.9.1

## Dependencies

- Chart.js
- Firebase Realtime Database

## Related Documentation

- [../devices/README.md](../devices/README.md) - Device management
- [../../README.md](../../README.md) - JavaScript overview
