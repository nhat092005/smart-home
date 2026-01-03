idf_component_register(
    SRCS "sensor_manager.c"
    INCLUDE_DIRS "include"
    REQUIRES
    i2cdev
    ds3231
    sht3x
    bh1750
    sh1106
)
