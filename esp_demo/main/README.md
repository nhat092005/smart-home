# Main Application

## Overview

Application entry point for ESP32 Smart Home system. Contains `app_main()` function that initializes all system components via task_init and runs the main loop.

## File Structure

```
main/
    CMakeLists.txt      # Build configuration
    Kconfig.projbuild   # Menuconfig options
    main.c              # Application entry point
    partitions.csv      # Flash partition table
    include/
        main.h          # Main header file
```

## Files Description

### main.c

Application entry point with minimal code:

```c
void app_main(void)
{
    ESP_LOGI(TAG, "Smart Home Application");
    
    // Initialize all system components
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
