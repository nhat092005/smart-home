# Utilities Component

Utility libraries for Smart Home ESP32 project.

## Structure

```
utilities/
  json_helper/     - JSON parsing and creation utilities
```

## Components

| Component | Description |
|-----------|-------------|
| json_helper | Safe JSON operations using cJSON library |

## Dependencies

- ESP-IDF cJSON component

## Usage

Add to your component CMakeLists.txt:

```cmake
REQUIRES utilities
```
