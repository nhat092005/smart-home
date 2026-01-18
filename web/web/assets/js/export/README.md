# Export Module

## Overview

Historical data export module for downloading sensor data as Excel files with flexible filtering options.

## Files

- `data-export.js` - Data retrieval, filtering, and Excel export

## Features

- Export device history to Excel
- Date range filtering
- Sensor type selection
- Formatted Excel output
- Download automation
- Error handling

## data-export.js

### Purpose

Retrieves Firebase historical data, filters by criteria, and exports to Excel format using SheetJS.

### Functions

#### exportData()
```javascript
exportData(deviceId, startDate, endDate, sensorTypes)
```
Main export function.

**Parameters**:
- `deviceId` - Device identifier (string)
- `startDate` - Start date filter (Date object or null)
- `endDate` - End date filter (Date object or null)
- `sensorTypes` - Array of sensors ['temp', 'hum', 'light'] or null

**Returns**: Promise that resolves when download completes

**Example**:
```javascript
import { exportData } from './data-export.js';

// Export all data
await exportData('device001', null, null, null);

// Export temperature only for date range
const start = new Date('2024-01-01');
const end = new Date('2024-01-31');
await exportData('device001', start, end, ['temp']);
```

#### formatDataForExport()
```javascript
formatDataForExport(rawData, sensorTypes)
```
Formats Firebase data into Excel-ready format.

**Parameters**:
- `rawData` - Firebase data object
- `sensorTypes` - Sensors to include

**Returns**: Array of formatted objects

**Internal function, called by exportData()**.

#### downloadExcelFile()
```javascript
downloadExcelFile(data, filename)
```
Generates Excel file and triggers download.

**Parameters**:
- `data` - Formatted data array
- `filename` - Output filename

**Internal function, called by exportData()**.

### Usage

```javascript
import { exportData } from './export/data-export.js';

document.getElementById('btn-export').addEventListener('click', async () => {
    const deviceId = getCurrentDevice();
    const startDate = document.getElementById('start-date').value;
    const endDate = document.getElementById('end-date').value;
    const sensors = getSelectedSensors();
    
    try {
        await exportData(deviceId, startDate, endDate, sensors);
        console.log('Export completed');
    } catch (error) {
        console.error('Export failed:', error);
    }
});
```

## Data Structure

### Firebase Input
```json
{
  "history": {
    "device001": {
      "1705561200000": {
        "temp": 25.5,
        "hum": 65.2,
        "light": 450,
        "datetime": "2024-01-18 10:00:00"
      },
      "1705561500000": {
        "temp": 25.7,
        "hum": 64.8,
        "light": 455,
        "datetime": "2024-01-18 10:05:00"
      }
    }
  }
}
```

### Excel Output Format
```javascript
[
    {
        "Timestamp": "2024-01-18 10:00:00",
        "Temperature (\u00b0C)": 25.5,
        "Humidity (%)": 65.2,
        "Light (lux)": 450
    },
    {
        "Timestamp": "2024-01-18 10:05:00",
        "Temperature (\u00b0C)": 25.7,
        "Humidity (%)": 64.8,
        "Light (lux)": 455
    }
]
```

### Filtered Output (Temperature Only)
```javascript
[
    {
        "Timestamp": "2024-01-18 10:00:00",
        "Temperature (\u00b0C)": 25.5
    },
    {
        "Timestamp": "2024-01-18 10:05:00",
        "Temperature (\u00b0C)": 25.7
    }
]
```

## Export Flow

### Full Process
```
User clicks Export → Get filters → Fetch Firebase data → Filter by date range
→ Filter by sensor types → Format for Excel → Generate XLSX → Trigger download
```

### Detailed Steps

1. **Get Parameters**
   - Device ID from current view
   - Start date from date picker
   - End date from date picker
   - Selected sensors from checkboxes

2. **Fetch Data**
   - Query Firebase `history/<deviceId>/`
   - Get all records

3. **Filter by Date**
   - Convert timestamps to Date objects
   - Keep records within [startDate, endDate]
   - If null, include all dates

4. **Filter by Sensors**
   - If sensorTypes specified, include only those columns
   - If null, include all sensors

5. **Format Data**
   - Create Excel-friendly objects
   - Use descriptive column names
   - Format numbers to 1 decimal place

6. **Generate Excel**
   - Create SheetJS workbook
   - Add worksheet
   - Apply column widths
   - Set header styles

7. **Download**
   - Convert to binary
   - Create Blob
   - Trigger browser download

## UI Components

### Export Button
```html
<button id="btn-export" class="btn btn-primary">
    Export to Excel
</button>
```

### Date Filters
```html
<div class="export-filters">
    <label>Start Date:
        <input type="date" id="start-date">
    </label>
    <label>End Date:
        <input type="date" id="end-date">
    </label>
</div>
```

### Sensor Selection
```html
<div class="sensor-filters">
    <label>
        <input type="checkbox" id="chk-temp" checked>
        Temperature
    </label>
    <label>
        <input type="checkbox" id="chk-hum" checked>
        Humidity
    </label>
    <label>
        <input type="checkbox" id="chk-light" checked>
        Light
    </label>
</div>
```

## Excel Configuration

### Workbook Creation
```javascript
const wb = XLSX.utils.book_new();
const ws = XLSX.utils.json_to_sheet(data);

XLSX.utils.book_append_sheet(wb, ws, 'Sensor Data');
```

### Column Widths
```javascript
ws['!cols'] = [
    { wch: 20 },  // Timestamp
    { wch: 15 },  // Temperature
    { wch: 15 },  // Humidity
    { wch: 15 }   // Light
];
```

### Header Styling
```javascript
const headerCell = ws['A1'];
headerCell.s = {
    font: { bold: true },
    fill: { fgColor: { rgb: "4F81BD" } },
    alignment: { horizontal: "center" }
};
```

## Filename Generation

### Format
```
<deviceName>_history_<startDate>_to_<endDate>.xlsx
```

### Examples
```javascript
// All data
"device001_history_all.xlsx"

// Date range
"device001_history_2024-01-01_to_2024-01-31.xlsx"

// Single day
"device001_history_2024-01-18.xlsx"
```

### Implementation
```javascript
function generateFilename(deviceId, startDate, endDate) {
    const deviceName = getDeviceName(deviceId) || deviceId;
    if (!startDate && !endDate) {
        return `${deviceName}_history_all.xlsx`;
    }
    const start = formatDate(startDate);
    const end = formatDate(endDate);
    return `${deviceName}_history_${start}_to_${end}.xlsx`;
}
```

## Filtering Logic

### Date Range Filter
```javascript
function filterByDate(records, startDate, endDate) {
    return records.filter(record => {
        const timestamp = new Date(record.timestamp);
        const afterStart = !startDate || timestamp >= startDate;
        const beforeEnd = !endDate || timestamp <= endDate;
        return afterStart && beforeEnd;
    });
}
```

### Sensor Filter
```javascript
function filterBySensors(record, sensorTypes) {
    if (!sensorTypes || sensorTypes.length === 0) {
        return record;  // Return all sensors
    }
    
    const filtered = { Timestamp: record.Timestamp };
    if (sensorTypes.includes('temp')) {
        filtered['Temperature (\u00b0C)'] = record['Temperature (\u00b0C)'];
    }
    if (sensorTypes.includes('hum')) {
        filtered['Humidity (%)'] = record['Humidity (%)'];
    }
    if (sensorTypes.includes('light')) {
        filtered['Light (lux)'] = record['Light (lux)'];
    }
    return filtered;
}
```

## Error Handling

### No Data Available
```javascript
if (records.length === 0) {
    throw new Error('No data available for export');
}
```

### Firebase Error
```javascript
try {
    const snapshot = await get(ref(db, `history/${deviceId}`));
    // ...
} catch (error) {
    console.error('Firebase fetch error:', error);
    throw new Error('Failed to retrieve data');
}
```

### Export Error
```javascript
try {
    XLSX.writeFile(wb, filename);
} catch (error) {
    console.error('Export error:', error);
    throw new Error('Failed to generate Excel file');
}
```

## User Feedback

### Loading State
```javascript
document.getElementById('btn-export').disabled = true;
document.getElementById('btn-export').textContent = 'Exporting...';
```

### Success Message
```javascript
showNotification('Export completed successfully', 'success');
```

### Error Message
```javascript
showNotification('Export failed: ' + error.message, 'error');
```

## Performance

- Data retrieval: <2s for 10,000 records
- Excel generation: <1s for 10,000 records
- Total export time: <3s typical
- Max supported records: 50,000
- File size: ~2MB per 10,000 records

## Optimization

### Batch Processing
```javascript
// Process large datasets in chunks
const CHUNK_SIZE = 1000;
for (let i = 0; i < records.length; i += CHUNK_SIZE) {
    const chunk = records.slice(i, i + CHUNK_SIZE);
    processChunk(chunk);
}
```

### Memory Management
```javascript
// Clear references after export
data = null;
wb = null;
```

## Browser Compatibility

- Chrome 90+ (full support)
- Firefox 88+ (full support)
- Safari 14+ (full support)
- Edge 90+ (full support)

Requires Blob and download attribute support.

## Dependencies

- SheetJS (xlsx) library
- Firebase Realtime Database

## Related Documentation

- [../devices/README.md](../devices/README.md) - Device data
- [../../README.md](../../README.md) - Asset overview
