idf_component_register(
    SRCS "sensor_reader.c"
    INCLUDE_DIRS "include"
    REQUIRES
    sensor_manager
    i2cdev
    ds3231
    sht3x
    bh1750
)
