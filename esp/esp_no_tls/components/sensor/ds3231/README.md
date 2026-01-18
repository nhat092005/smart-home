# DS3231 Real-Time Clock Driver

## Overview

Driver for DS3231 extremely accurate I2C real-time clock with temperature-compensated crystal oscillator and integrated 32kHz output.

## Features

- Timekeeping with seconds to year range
- Two programmable time-of-day alarms
- Temperature-compensated oscillator (±2ppm accuracy)
- Battery backup support for time retention
- Integrated temperature sensor (-40°C to +85°C)
- Aging offset adjustment
- 32kHz square wave output

## Hardware Specifications

- I2C address: 0x68
- Supply voltage: 2.3V - 5.5V
- Timekeeping current: 100 µA
- Backup current: 70 nA
- Temperature accuracy: ±3°C

## API Functions

### Initialization

```c
esp_err_t ds3231_init_desc(ds3231_t *dev, i2c_port_t port, 
                           gpio_num_t sda_gpio, gpio_num_t scl_gpio);
esp_err_t ds3231_free_desc(ds3231_t *dev);
```

### Time Management

```c
esp_err_t ds3231_set_time(ds3231_t *dev, struct tm *time);
esp_err_t ds3231_get_time(ds3231_t *dev, struct tm *time);
esp_err_t ds3231_set_timestamp(ds3231_t *dev, uint32_t timestamp);
esp_err_t ds3231_get_timestamp(ds3231_t *dev, uint32_t *timestamp);
```

### Alarm Configuration

```c
esp_err_t ds3231_set_alarm(ds3231_t *dev, ds3231_alarm_t alarm, struct tm *time, 
                           ds3231_alarm1_rate_t rate);
esp_err_t ds3231_get_alarm(ds3231_t *dev, ds3231_alarm_t alarm, struct tm *time, 
                           ds3231_alarm1_rate_t *rate);
esp_err_t ds3231_clear_alarm_flags(ds3231_t *dev, ds3231_alarm_t alarm);
```

### Temperature Reading

```c
esp_err_t ds3231_get_temp_float(ds3231_t *dev, float *temp);
esp_err_t ds3231_get_temp_integer(ds3231_t *dev, int8_t *temp);
```

## Usage Example

```c
#include "ds3231.h"
#include <time.h>

ds3231_t rtc;

// Initialize
ds3231_init_desc(&rtc, I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);
i2c_dev_init(&rtc.i2c_dev);

// Set time (2024-01-15 14:30:00)
struct tm time = {
    .tm_year = 124,  // 2024 - 1900
    .tm_mon = 0,     // January (0-11)
    .tm_mday = 15,
    .tm_hour = 14,
    .tm_min = 30,
    .tm_sec = 0
};
ds3231_set_time(&rtc, &time);

// Read time
ds3231_get_time(&rtc, &time);

// Get Unix timestamp
uint32_t timestamp;
ds3231_get_timestamp(&rtc, &timestamp);

// Read temperature
float temp;
ds3231_get_temp_float(&rtc, &temp);

// Cleanup
ds3231_free_desc(&rtc);
```

## Alarm Types

- **DS3231_ALARM_1**: Can match seconds, minutes, hours, day
- **DS3231_ALARM_2**: Can match minutes, hours, day (no seconds)

## Dependencies

- i2cdev abstraction layer
- FreeRTOS
| `DS3231_ALARM_1` | Seconds | Full time match |
| `DS3231_ALARM_2` | Minutes | No seconds field |

## Alarm 1 Rates

| Rate | Trigger |
|------|---------|
| `DS3231_ALARM1_EVERY_SECOND` | Every second |
| `DS3231_ALARM1_MATCH_SEC` | Seconds match |
| `DS3231_ALARM1_MATCH_SECMIN` | Min:Sec match |
| `DS3231_ALARM1_MATCH_SECMINHOUR` | Hour:Min:Sec match |
| `DS3231_ALARM1_MATCH_SECMINHOURDAY` | Day + time match |
| `DS3231_ALARM1_MATCH_SECMINHOURDATE` | Date + time match |

## Usage Example

```c
#include "ds3231.h"

ds3231_t dev;
ds3231_init_desc(&dev, I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);
i2c_dev_init(&dev.i2c_dev);

// Set time from Unix timestamp
ds3231_set_timestamp(&dev, 1703980800);

// Get current timestamp
uint32_t ts;
ds3231_get_timestamp(&dev, &ts);
printf("Timestamp: %lu\n", ts);

// Get time as struct tm
struct tm time;
ds3231_get_time(&dev, &time);
printf("Time: %04d-%02d-%02d %02d:%02d:%02d\n",
       time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
       time.tm_hour, time.tm_min, time.tm_sec);
```
