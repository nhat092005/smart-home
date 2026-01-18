/**
 * @file ds3231.c
 *
 * @brief DS3231 Real-Time Clock (RTC) Driver Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "ds3231.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

/* Private defines macro -----------------------------------------------------*/

#define CHECK(x)                                                           \
    do                                                                     \
    {                                                                      \
        esp_err_t __err = (x);                                             \
        if (__err != ESP_OK)                                               \
        {                                                                  \
            ESP_LOGE(TAG, "Operation failed: %s", esp_err_to_name(__err)); \
            return __err;                                                  \
        }                                                                  \
    } while (0)

#define CHECK_ARG(ARG)                         \
    do                                         \
    {                                          \
        if (!(ARG))                            \
        {                                      \
            ESP_LOGE(TAG, "Invalid argument"); \
            return ESP_ERR_INVALID_ARG;        \
        }                                      \
    } while (0)

/* Private defines -----------------------------------------------------------*/

// Status register bits
#define DS3231_STAT_OSCILLATOR 0x80 ///< Oscillator stop flag
#define DS3231_STAT_32KHZ 0x08      ///< 32kHz output enable
#define DS3231_STAT_ALARM_2 0x02    ///< Alarm 2 flag
#define DS3231_STAT_ALARM_1 0x01    ///< Alarm 1 flag

// Control register bits
#define DS3231_CTRL_OSCILLATOR 0x80 ///< Oscillator enable/disable
#define DS3231_CTRL_TEMPCONV 0x20   ///< Force temperature conversion
#define DS3231_CTRL_ALARM_INTS 0x04 ///< Alarm interrupt enable
#define DS3231_CTRL_ALARM2_INT 0x02 ///< Alarm 2 interrupt enable
#define DS3231_CTRL_ALARM1_INT 0x01 ///< Alarm 1 interrupt enable

// Alarm configuration bits
#define DS3231_ALARM_WDAY 0x40   ///< Use day of week for alarm
#define DS3231_ALARM_NOTSET 0x80 ///< Alarm field not set

// Register addresses
#define DS3231_ADDR_TIME 0x00    ///< Time registers start
#define DS3231_ADDR_ALARM1 0x07  ///< Alarm 1 registers start
#define DS3231_ADDR_ALARM2 0x0b  ///< Alarm 2 registers start
#define DS3231_ADDR_CONTROL 0x0e ///< Control register
#define DS3231_ADDR_STATUS 0x0f  ///< Status register
#define DS3231_ADDR_AGING 0x10   ///< Aging offset register
#define DS3231_ADDR_TEMP 0x11    ///< Temperature registers start

// Time format flags
#define DS3231_12HOUR_FLAG 0x40 ///< 12-hour mode flag
#define DS3231_12HOUR_MASK 0x1f ///< 12-hour value mask
#define DS3231_PM_FLAG 0x20     ///< PM flag in 12-hour mode
#define DS3231_MONTH_MASK 0x1f  ///< Month value mask

// I2C configuration
#define I2C_FREQ_HZ I2C_MASTER_FREQ_HZ

/* Private types ------------------------------------------------------------- */

enum
{
    DS3231_SET = 0,
    DS3231_CLEAR,
    DS3231_REPLACE
};

/* Private variables --------------------------------------------------------- */

static const char *TAG = "DS3231";

static const int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const int days_per_month_leap_year[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* Private functions prototypes ---------------------------------------------- */

/**
 * @brief Convert BCD to decimal
 *
 * @param[in] val BCD value
 *
 * @return Decimal value
 */
static uint8_t bcd2dec(uint8_t val);

/**
 * @brief Convert decimal to BCD
 *
 * @param[in] val Decimal value
 *
 * @return BCD value
 */
static uint8_t dec2bcd(uint8_t val);

/**
 * @brief Check if a year is a leap year
 *
 * @param[in] year Year (full year, e.g., 2024)
 * @param[in] month Month (1-12)
 * @param[in] day Day of the month (1-31)
 *
 * @return true if leap year, false otherwise
 */
static inline int days_since_january_1st(int year, int month, int day);

/**
 * @brief Get a specific flag from a register
 *
 * @param[in] dev Device descriptor
 * @param[in] addr Register address
 * @param[in] mask Bit mask to apply
 * @param[out] flag Retrieved flag value
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t ds3231_get_flag(ds3231_t *dev, uint8_t addr, uint8_t mask, uint8_t *flag);

/**
 * @brief Set or clear specific bits in a register
 *
 * @param[in] dev Device descriptor
 * @param[in] addr Register address
 * @param[in] bits Bits to set or clear
 * @param[in] mode Operation mode (DS3231_SET, DS3231_CLEAR, DS3231_REPLACE)
 *
 * @return ESP_OK on success, error code otherwise
 */
static esp_err_t ds3231_set_flag(ds3231_t *dev, uint8_t addr, uint8_t bits, uint8_t mode);

/* Exported functions ---------------------------------------------------------*/

/**
 * @brief Initialize device descriptor
 */
esp_err_t ds3231_init_desc(ds3231_t *dev, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio)
{
    CHECK_ARG(dev);

    ESP_LOGI(TAG, "Initializing DS3231");

    dev->i2c_dev.port = port;
    dev->i2c_dev.addr = DS3231_ADDR;
    dev->i2c_dev.sda_io_num = sda_gpio;
    dev->i2c_dev.scl_io_num = scl_gpio;
    dev->i2c_dev.clk_speed = I2C_FREQ_HZ;

    esp_err_t res = i2c_dev_create_mutex(&dev->i2c_dev);
    if (res == ESP_OK)
    {
        ESP_LOGI(TAG, "DS3231 initialized on port %d (SDA: GPIO%d, SCL: GPIO%d)",
                 port, sda_gpio, scl_gpio);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to initialize DS3231: %s", esp_err_to_name(res));
    }

    return res;
}

/**
 * @brief Free device descriptor
 */
esp_err_t ds3231_free_desc(ds3231_t *dev)
{
    CHECK_ARG(dev);

    return i2c_dev_delete_mutex(&dev->i2c_dev);
}

/**
 * @brief Set the time on the RTC
 */
esp_err_t ds3231_set_time(ds3231_t *dev, struct tm *time)
{
    CHECK_ARG(dev && time);

    // Validate time ranges
    if (time->tm_sec < 0 || time->tm_sec > 59)
    {
        ESP_LOGE(TAG, "Invalid seconds: %d (must be 0-59)", time->tm_sec);
        return ESP_ERR_INVALID_ARG;
    }
    if (time->tm_min < 0 || time->tm_min > 59)
    {
        ESP_LOGE(TAG, "Invalid minutes: %d (must be 0-59)", time->tm_min);
        return ESP_ERR_INVALID_ARG;
    }
    if (time->tm_hour < 0 || time->tm_hour > 23)
    {
        ESP_LOGE(TAG, "Invalid hours: %d (must be 0-23)", time->tm_hour);
        return ESP_ERR_INVALID_ARG;
    }
    if (time->tm_mday < 1 || time->tm_mday > 31)
    {
        ESP_LOGE(TAG, "Invalid day: %d (must be 1-31)", time->tm_mday);
        return ESP_ERR_INVALID_ARG;
    }
    if (time->tm_mon < 0 || time->tm_mon > 11)
    {
        ESP_LOGE(TAG, "Invalid month: %d (must be 0-11)", time->tm_mon);
        return ESP_ERR_INVALID_ARG;
    }
    if (time->tm_year < 100)
    {
        ESP_LOGE(TAG, "Invalid year: %d (must be >= 100 for year 2000+)", time->tm_year);
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t data[7];

    /* time/date data */
    data[0] = dec2bcd(time->tm_sec);
    data[1] = dec2bcd(time->tm_min);
    data[2] = dec2bcd(time->tm_hour);
    /* The week data must be in the range 1 to 7, and to keep the start on the
     * same day as for tm_wday have it start at 1 on Sunday. */
    data[3] = dec2bcd(time->tm_wday + 1);
    data[4] = dec2bcd(time->tm_mday);
    data[5] = dec2bcd(time->tm_mon + 1);
    data[6] = dec2bcd(time->tm_year - 100);

    ESP_LOGI(TAG, "Setting time: %04d-%02d-%02d %02d:%02d:%02d",
             time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
             time->tm_hour, time->tm_min, time->tm_sec);

    CHECK(i2c_dev_write_reg(&dev->i2c_dev, DS3231_ADDR_TIME, data, 7));

    ESP_LOGD(TAG, "Time set successfully");
    return ESP_OK;
}

/**
 * @brief Set alarm time and options
 */
esp_err_t ds3231_set_alarm(ds3231_t *dev, ds3231_alarm_t alarms, struct tm *time1,
                           ds3231_alarm1_rate_t option1, struct tm *time2, ds3231_alarm2_rate_t option2)
{
    CHECK_ARG(dev);

    int i = 0;
    uint8_t data[7];

    /* alarm 1 data */
    if (alarms != DS3231_ALARM_2)
    {
        CHECK_ARG(time1);

        // Set alarm 1 seconds
        if (option1 >= DS3231_ALARM1_MATCH_SEC)
        {
            data[i++] = dec2bcd(time1->tm_sec);
        }
        else
        {
            data[i++] = DS3231_ALARM_NOTSET;
        }

        // Set alarm 1 minutes
        if (option1 >= DS3231_ALARM1_MATCH_SECMIN)
        {
            data[i++] = dec2bcd(time1->tm_min);
        }
        else
        {
            data[i++] = DS3231_ALARM_NOTSET;
        }

        // Set alarm 1 hours
        if (option1 >= DS3231_ALARM1_MATCH_SECMINHOUR)
        {
            data[i++] = dec2bcd(time1->tm_hour);
        }
        else
        {
            data[i++] = DS3231_ALARM_NOTSET;
        }

        // Set alarm 1 day/date
        if (option1 == DS3231_ALARM1_MATCH_SECMINHOURDAY)
        {
            data[i++] = dec2bcd(time1->tm_wday + 1) | DS3231_ALARM_WDAY;
        }
        else if (option1 == DS3231_ALARM1_MATCH_SECMINHOURDATE)
        {
            data[i++] = dec2bcd(time1->tm_mday);
        }
        else
        {
            data[i++] = DS3231_ALARM_NOTSET;
        }

        ESP_LOGD(TAG, "Setting alarm 1: option=%d", option1);
    }

    /* alarm 2 data */
    if (alarms != DS3231_ALARM_1)
    {
        CHECK_ARG(time2);

        // Set alarm 2 minutes
        if (option2 >= DS3231_ALARM2_MATCH_MIN)
        {
            data[i++] = dec2bcd(time2->tm_min);
        }
        else
        {
            data[i++] = DS3231_ALARM_NOTSET;
        }

        // Set alarm 2 hours
        if (option2 >= DS3231_ALARM2_MATCH_MINHOUR)
        {
            data[i++] = dec2bcd(time2->tm_hour);
        }
        else
        {
            data[i++] = DS3231_ALARM_NOTSET;
        }

        // Set alarm 2 day/date
        if (option2 == DS3231_ALARM2_MATCH_MINHOURDAY)
        {
            data[i++] = dec2bcd(time2->tm_wday + 1) | DS3231_ALARM_WDAY;
        }
        else if (option2 == DS3231_ALARM2_MATCH_MINHOURDATE)
        {
            data[i++] = dec2bcd(time2->tm_mday);
        }
        else
        {
            data[i++] = DS3231_ALARM_NOTSET;
        }

        ESP_LOGD(TAG, "Setting alarm 2: option=%d", option2);
    }

    uint8_t start_addr = (alarms == DS3231_ALARM_2) ? DS3231_ADDR_ALARM2 : DS3231_ADDR_ALARM1;

    esp_err_t res = i2c_dev_write_reg(&dev->i2c_dev, start_addr, data, i);

    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set alarm: %s", esp_err_to_name(res));
        return res;
    }

    ESP_LOGI(TAG, "Alarm configured successfully");
    return ESP_OK;
}

/**
 * @brief Get alarm flags
 */
esp_err_t ds3231_get_alarm_flags(ds3231_t *dev, ds3231_alarm_t *alarms)
{
    CHECK_ARG(dev && alarms);

    return ds3231_get_flag(dev, DS3231_ADDR_STATUS, DS3231_ALARM_BOTH, (uint8_t *)alarms);
}

/**
 * @brief Clear alarm flags
 */
esp_err_t ds3231_clear_alarm_flags(ds3231_t *dev, ds3231_alarm_t alarms)
{
    CHECK_ARG(dev);

    CHECK(ds3231_set_flag(dev, DS3231_ADDR_STATUS, alarms, DS3231_CLEAR));

    ESP_LOGD(TAG, "Alarm flags cleared (alarms: 0x%02x)", alarms);
    return ESP_OK;
}

/**
 * @brief Clear alarm flags
 */
esp_err_t ds3231_enable_alarm_ints(ds3231_t *dev, ds3231_alarm_t alarms)
{
    CHECK_ARG(dev);

    CHECK(ds3231_set_flag(dev, DS3231_ADDR_CONTROL, DS3231_CTRL_ALARM_INTS | alarms, DS3231_SET));

    ESP_LOGI(TAG, "Alarm interrupts enabled (alarms: 0x%02x)", alarms);
    return ESP_OK;
}

/**
 * @brief Disable alarm interrupts
 */
esp_err_t ds3231_disable_alarm_ints(ds3231_t *dev, ds3231_alarm_t alarms)
{
    CHECK_ARG(dev);

    CHECK(ds3231_set_flag(dev, DS3231_ADDR_CONTROL, alarms, DS3231_CLEAR));

    ESP_LOGI(TAG, "Alarm interrupts disabled (alarms: 0x%02x)", alarms);
    return ESP_OK;
}

/**
 * @brief Get the time from the RTC
 */
esp_err_t ds3231_get_time(ds3231_t *dev, struct tm *time)
{
    CHECK_ARG(dev && time);

    uint8_t data[7];

    /* read time */
    esp_err_t res = i2c_dev_read_reg(&dev->i2c_dev, DS3231_ADDR_TIME, data, 7);

    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read time: %s", esp_err_to_name(res));
        return res;
    }

    /* convert to unix time structure */
    time->tm_sec = bcd2dec(data[0]);
    time->tm_min = bcd2dec(data[1]);

    if (data[2] & DS3231_12HOUR_FLAG)
    {
        /* 12H mode */
        time->tm_hour = bcd2dec(data[2] & DS3231_12HOUR_MASK);
        /* AM/PM conversion */
        if (data[2] & DS3231_PM_FLAG)
        {
            /* PM: 12PM=12h, 1PM=13h, 11PM=23h */
            if (time->tm_hour != 12)
                time->tm_hour += 12;
        }
        else
        {
            /* AM: 12AM=0h, 1AM=1h, 11AM=11h */
            if (time->tm_hour == 12)
                time->tm_hour = 0;
        }
    }
    else
    {
        /* 24H mode */
        time->tm_hour = bcd2dec(data[2]);
    }

    time->tm_wday = bcd2dec(data[3]) - 1;
    time->tm_mday = bcd2dec(data[4]);
    time->tm_mon = bcd2dec(data[5] & DS3231_MONTH_MASK) - 1;
    time->tm_year = bcd2dec(data[6]) + 100;
    time->tm_isdst = 0;
    time->tm_yday = days_since_january_1st(time->tm_year, time->tm_mon, time->tm_mday);

    ESP_LOGD(TAG, "Read time: %04d-%02d-%02d %02d:%02d:%02d",
             time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
             time->tm_hour, time->tm_min, time->tm_sec);

    return ESP_OK;
}

/**
 * @brief Set Unix timestamp
 */
esp_err_t ds3231_set_timestamp(ds3231_t *dev, uint32_t timestamp)
{
    CHECK_ARG(dev);

    time_t ts = (time_t)timestamp;
    struct tm timeinfo;

    if (gmtime_r(&ts, &timeinfo) == NULL)
    {
        ESP_LOGE(TAG, "Failed to convert timestamp to timestamp");
        return ESP_FAIL;
    }

    // Log the converted time
    ESP_LOGD(TAG, "Setting timestamp: %lu (%04d-%02d-%02d %02d:%02d:%02d)",
             timestamp, timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,
             timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    esp_err_t res = ds3231_set_time(dev, &timeinfo);
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set time: %s", esp_err_to_name(res));
        return res;
    }

    return ESP_OK;
}

/**
 * @brief Get Unix timestamp
 */
esp_err_t ds3231_get_timestamp(ds3231_t *dev, uint32_t *timestamp)
{
    CHECK_ARG(dev && timestamp);

    struct tm time;
    esp_err_t res = ds3231_get_time(dev, &time);
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get time for timestamp: %s", esp_err_to_name(res));
        return res;
    }

    // Convert tm struct to Unix timestamp
    time_t ts = mktime(&time);
    if (ts == -1)
    {
        ESP_LOGE(TAG, "Failed to convert time to timestamp");
        return ESP_FAIL;
    }

    *timestamp = (uint32_t)ts;
    ESP_LOGD(TAG, "Timestamp: %lu (%04d-%02d-%02d %02d:%02d:%02d)",
             *timestamp, time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
             time.tm_hour, time.tm_min, time.tm_sec);

    return ESP_OK;
}

/* Private functions --------------------------------------------------------- */

/**
 * @brief Convert BCD to decimal
 */
static uint8_t bcd2dec(uint8_t val)
{
    return (val >> 4) * 10 + (val & 0x0f);
}

static uint8_t dec2bcd(uint8_t val)
{
    return ((val / 10) << 4) + (val % 10);
}

/**
 * @brief Calculate days since January 1st of the given year
 */
static inline int days_since_january_1st(int year, int month, int day)
{
    int days = day - 1;
    const int *ptr = days_per_month;

    // Handle leap year
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        ptr = days_per_month_leap_year;

    // Add days from previous months
    for (int i = 0; i < month; i++)
    {
        days += ptr[i];
    }

    return days;
}

/**
 * @brief Get a specific flag from a register
 */
static esp_err_t ds3231_get_flag(ds3231_t *dev, uint8_t addr, uint8_t mask, uint8_t *flag)
{
    uint8_t data;

    /* get register */
    esp_err_t res = i2c_dev_read_reg(&dev->i2c_dev, addr, &data, 1);
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read register 0x%02x: %s", addr, esp_err_to_name(res));
        return res;
    }

    /* return only requested flag */
    *flag = (data & mask);
    ESP_LOGD(TAG, "Read flag from addr 0x%02x: 0x%02x (mask 0x%02x)", addr, *flag, mask);
    return ESP_OK;
}

/**
 * @brief Set or clear specific bits in a register
 */
static esp_err_t ds3231_set_flag(ds3231_t *dev, uint8_t addr, uint8_t bits, uint8_t mode)
{
    uint8_t data;

    /* get status register */
    esp_err_t res = i2c_dev_read_reg(&dev->i2c_dev, addr, &data, 1);
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read register 0x%02x: %s", addr, esp_err_to_name(res));
        return res;
    }

    uint8_t old_data = data;

    /* modify the flag */
    if (mode == DS3231_REPLACE)
    {
        data = bits;
    }
    else if (mode == DS3231_SET)
    {
        data |= bits;
    }
    else
    {
        data &= ~bits;
    }

    ESP_LOGD(TAG, "Setting flag at addr 0x%02x: 0x%02x -> 0x%02x (mode %d)",
             addr, old_data, data, mode);

    res = i2c_dev_write_reg(&dev->i2c_dev, addr, &data, 1);
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write register 0x%02x: %s", addr, esp_err_to_name(res));
    }

    return res;
}