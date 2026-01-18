idf_component_register(
    SRCS "ds3231.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES
    driver
    i2cdev
)
