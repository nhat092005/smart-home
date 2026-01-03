# DS3231 Real-Time Clock Driver

High-precision RTC module driver for ESP32.

## Specifications

- Accuracy: +/- 2ppm (0 to +40 C)
- Battery backup support
- Two programmable alarms
- Temperature compensated crystal oscillator

## I2C Address

Fixed address: 0x68

## API Reference

### Initialization

```c
esp_err_t ds3231_init_desc(ds3231_t *dev, i2c_port_t port,
                           gpio_num_t sda_gpio, gpio_num_t scl_gpio);
esp_err_t ds3231_free_desc(ds3231_t *dev);
```

### Time Operations

```c
esp_err_t ds3231_set_time(ds3231_t *dev, struct tm *time);
esp_err_t ds3231_get_time(ds3231_t *dev, struct tm *time);
esp_err_t ds3231_set_timestamp(ds3231_t *dev, uint32_t timestamp);
esp_err_t ds3231_get_timestamp(ds3231_t *dev, uint32_t *timestamp);
```

### Alarm Functions

```c
esp_err_t ds3231_set_alarm(ds3231_t *dev, ds3231_alarm_t alarms,
                           struct tm *time1, ds3231_alarm1_rate_t option1,
                           struct tm *time2, ds3231_alarm2_rate_t option2);
esp_err_t ds3231_get_alarm_flags(ds3231_t *dev, ds3231_alarm_t *alarms);
esp_err_t ds3231_clear_alarm_flags(ds3231_t *dev, ds3231_alarm_t alarms);
esp_err_t ds3231_enable_alarm_ints(ds3231_t *dev, ds3231_alarm_t alarms);
esp_err_t ds3231_disable_alarm_ints(ds3231_t *dev, ds3231_alarm_t alarms);
```

## Alarm Types

| Alarm | Precision | Description |
|-------|-----------|-------------|
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
