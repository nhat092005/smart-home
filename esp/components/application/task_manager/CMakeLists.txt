idf_component_register(
    SRCS 
    "task_status.c" 
    "task_wifi.c" 
    "task_button.c" 
    "task_init.c"
    "task_mode.c"
    INCLUDE_DIRS "include"
    REQUIRES
    nvs_flash
    button_handler
    device_control
    status_led
    mode_manager
    sensor_manager
    wifi_manager
)
