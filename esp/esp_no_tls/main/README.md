# Main Application Entry Point

## Overview

Minimal application entry point for ESP32 Smart Home system. Contains app_main() function that delegates system initialization to task_init and maintains main execution context.

## Features

- Single entry point architecture
- Task manager initialization
- Infinite main loop for FreeRTOS scheduler
- System logging
- Clean separation of concerns

## File Structure

```
main/
    CMakeLists.txt      # Build configuration
    Kconfig.projbuild   # Menuconfig options
    main.c              # Application entry point
    main.h              # Main header file
    partitions.csv      # Flash partition table
    include/
        main.h          # Header file directory
```

## Application Code

```c
#include "main.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Smart Home Application");

    // Initialize system components
    task_init();

    // Main loop
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Execution Flow

1. **ESP-IDF Startup**: ROM bootloader → 2nd stage bootloader → app_main()
2. **Task Initialization**: Call task_init() to create all FreeRTOS tasks
3. **Main Loop**: Idle loop allowing FreeRTOS scheduler to run tasks

## Build Configuration

### CMakeLists.txt

```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "include"
    REQUIRES 
        application
        communication
        hardware
        sensor
        utilities
)
```

### Kconfig.projbuild

Defines menuconfig options for:
- Application version
- WiFi manager settings
- MQTT manager configuration
- Button handler GPIO pins
- Device control GPIO pins
- Status LED GPIO pins
- I2C interface settings

## Partition Table

Flash memory layout defined in partitions.csv:

| Name | Type | SubType | Offset | Size | Description |
|------|------|---------|--------|------|-------------|
| nvs | data | nvs | 0x9000 | 24K | Non-volatile storage |
| phy_init | data | phy | 0xf000 | 4K | RF calibration |
| factory | app | factory | 0x10000 | 2944K | Application firmware |
| storage | data | spiffs | 0x300000 | 960K | File storage |
| coredump | data | coredump | 0x3F0000 | 64K | Core dump partition |

## Dependencies

- task_manager (application layer)
- FreeRTOS kernel
- ESP-IDF core components

## Usage Example

Standard ESP-IDF application structure:

```bash
# Build project
idf.py build

# Flash to device
idf.py -p COM3 flash

# Monitor output
idf.py -p COM3 monitor
```

## System Initialization

The task_init() function orchestrates:
1. NVS flash initialization
2. WiFi manager setup
3. Mode manager initialization
4. Sensor subsystem initialization
5. Hardware GPIO configuration
6. MQTT callback registration
7. FreeRTOS task creation

## Main Loop Purpose

The infinite while loop in app_main() serves to:
- Prevent app_main() from returning (required by ESP-IDF)
- Allow FreeRTOS scheduler to manage all tasks
- Provide optional low-priority background processing point

## Configuration Options

Accessible via `idf.py menuconfig`:

### Smart Home Device Configuration
- Application version string
- Data publish interval
- WiFi AP SSID/password
- MQTT broker settings
- GPIO pin assignments
- I2C bus configuration

## Memory Requirements

- Stack usage: Minimal (main task deleted after initialization)
- Heap usage: Determined by component allocations
- Flash usage: ~2.9MB application partition

## Logging

ESP-IDF logging system:
```c
ESP_LOGI(TAG, "Smart Home Application");  // Info level
ESP_LOGW(TAG, "Warning message");         // Warning level
ESP_LOGE(TAG, "Error message");           // Error level
```

## Related Documentation

- [../README.md](../README.md) - Project overview
- [../components/application/task_manager/README.md](../components/application/task_manager/README.md) - Task creation
- [../components/application/task_init/README.md](../components/application/task_init/README.md) - System initialization
    task_init();
    
    // Main loop (idle)
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

### main.h

Includes task_manager.h which aggregates all component headers.

### Kconfig.projbuild

Centralized menuconfig that includes all component Kconfig files:

```
SMART HOME DEVICE CONFIGURATION
    |-- Application Configuration
    |       task_manager/Kconfig
    |
    |-- Communication Layer Configuration
    |       wifi_manager/Kconfig
    |       mqtt_manager/Kconfig
    |
    |-- Hardware Layer Configuration
    |       button_handler/Kconfig
    |       device_control/Kconfig
    |       status_led/Kconfig
    |
    |-- Hardware Protocol Configuration
            i2cdev/Kconfig
```

### partitions.csv

Custom flash partition layout:

| Name | Type | Offset | Size |
|------|------|--------|------|
| nvs | data | 0x9000 | 24KB |
| phy_init | data | 0xF000 | 4KB |
| factory | app | 0x10000 | 2.9MB |
| storage | data | 0x300000 | 960KB |
| coredump | data | 0x3F0000 | 64KB |

## Build Configuration

### CMakeLists.txt

```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "include"
    REQUIRES task_manager
)
```

**Note:** Only requires `task_manager` which transitively includes all other components.

## Startup Sequence

```
app_main()
    |
    +-- task_init()
    |       |
    |       +-- NVS initialization
    |       +-- Status LED initialization
    |       +-- Button handler initialization
    |       +-- I2C and sensor initialization
    |       +-- Mode manager initialization
    |       +-- Display initialization
    |       +-- WiFi initialization
    |       +-- MQTT initialization
    |
    +-- Main loop (1s delay idle)
```

## Dependencies

- `task_manager` - Provides access to all application components
- `esp_log` - Logging
- `freertos` - Task delay
