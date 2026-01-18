# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# Add nested component directories
set(EXTRA_COMPONENT_DIRS
    # Application components
    # Tasks components
    "components/application/task_manager"
    "components/application/task_init"
    "components/application/task_button"
    "components/application/task_status"
    "components/application/task_wifi"
    "components/application/task_mqtt"
    "components/application/task_mode"
    "components/application/task_display"

    # Other application components
    "components/application/mqtt_callback"
    "components/application/mode_manager"
    "components/application/shared_sensor"

    # Hardware components
    "components/hardware/button_handler"
    "components/hardware/device_control"
    "components/hardware/status_led"

    # Communication components
    "components/communication/wifi_manager"
    "components/communication/webserver"
    "components/communication/mqtt_manager"

    # Sensor components
    "components/sensor/i2cdev"
    "components/sensor/sensor_manager"
    "components/sensor/sensor_reader"
    "components/sensor/bh1750"
    "components/sensor/ds3231"
    "components/sensor/sht3x"
    "components/sensor/sh1106"

    # Utilities components
    "components/utilities/json_helper"
)

set(PARTITION_CSV_PATH "${CMAKE_SOURCE_DIR}/partitions.csv")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(SMART_HOME)
