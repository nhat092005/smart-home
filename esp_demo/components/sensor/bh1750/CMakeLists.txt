idf_component_register(
    SRCS "bh1750.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES
    driver
    i2cdev
)
