idf_component_register(
    SRCS "sht3x.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES
    driver
    i2cdev
    esp_timer
)
