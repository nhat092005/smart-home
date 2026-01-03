idf_component_register(
    SRCS "mqtt_callback.c"
    INCLUDE_DIRS "include"
    REQUIRES 
    esp_event
    mqtt_manager
    json_helper
)
