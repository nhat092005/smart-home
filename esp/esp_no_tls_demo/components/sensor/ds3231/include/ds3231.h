/**
 * @file ds3231.h
 *
 * @brief DS3231 Real-Time Clock (RTC) Driver API
 */
#ifndef DS3231_H
#define DS3231_H

/* Includes ------------------------------------------------------------------*/

#include "i2cdev.h"
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>

/* Exported defines --------------------------------------------------------- */

#define DS3231_ADDR 0x68 //!< I2C address

/* Exported types ----------------------------------------------------------- */

/**
 * @brief Alarm selection
 */
typedef enum
{
    DS3231_ALARM_NONE = 0, //!< No alarms selected
    DS3231_ALARM_1,        //!< Alarm 1 only (can match seconds)
    DS3231_ALARM_2,        //!< Alarm 2 only (cannot match seconds)
    DS3231_ALARM_BOTH      //!< Both alarms
} ds3231_alarm_t;

/**
 * @brief Alarm 1 match rate (has seconds precision)
 */
typedef enum
{
    DS3231_ALARM1_EVERY_SECOND = 0,    //!< Alarm once per second
    DS3231_ALARM1_MATCH_SEC,           //!< Alarm when seconds match
    DS3231_ALARM1_MATCH_SECMIN,        //!< Alarm when minutes and seconds match
    DS3231_ALARM1_MATCH_SECMINHOUR,    //!< Alarm when hours, minutes, seconds match
    DS3231_ALARM1_MATCH_SECMINHOURDAY, //!< Alarm when day of week, hours, minutes, seconds match
    DS3231_ALARM1_MATCH_SECMINHOURDATE //!< Alarm when date, hours, minutes, seconds match
} ds3231_alarm1_rate_t;

/**
 * @brief Alarm 2 match rate (no seconds, minute precision)
 */
typedef enum
{
    DS3231_ALARM2_EVERY_MIN = 0,    //!< Alarm once per minute (at 00 seconds)
    DS3231_ALARM2_MATCH_MIN,        //!< Alarm when minutes match
    DS3231_ALARM2_MATCH_MINHOUR,    //!< Alarm when hours and minutes match
    DS3231_ALARM2_MATCH_MINHOURDAY, //!< Alarm when day of week, hours, minutes match
    DS3231_ALARM2_MATCH_MINHOURDATE //!< Alarm when date, hours, minutes match
} ds3231_alarm2_rate_t;

/**
 * @brief Square wave output frequency
 */
typedef enum
{
    DS3231_SQWAVE_1HZ = 0x00,    //!< 1 Hz square wave
    DS3231_SQWAVE_1024HZ = 0x08, //!< 1.024 kHz square wave
    DS3231_SQWAVE_4096HZ = 0x10, //!< 4.096 kHz square wave
    DS3231_SQWAVE_8192HZ = 0x18  //!< 8.192 kHz square wave
} ds3231_sqwave_freq_t;

/**
 * Device descriptor
 */
typedef struct
{
    i2c_dev_t i2c_dev; //!< I2C device descriptor
} ds3231_t;

/* Exported functions ------------------------------------------------------- */

/**
 * @brief Initialize device descriptor
 *
 * @param[in] dev I2C device descriptor to initialize
 * @param[in] port I2C port number (I2C_NUM_0 or I2C_NUM_1)
 * @param[in] sda_gpio GPIO pin for SDA line
 * @param[in] scl_gpio GPIO pin for SCL line
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_init_desc(ds3231_t *dev, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio);

/**
 * @brief Free device descriptor
 *
 * @param[in] dev I2C device descriptor
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_free_desc(ds3231_t *dev);

/**
 * @brief Set the time on the RTC
 *
 * @param[in] dev Device descriptor
 * @param[in] time Pointer to tm struct with time to set
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_set_time(ds3231_t *dev, struct tm *time);

/**
 * @brief Get the time from the RTC
 *
 * @param[in] dev Device descriptor
 * @param[out] time Pointer to tm struct to populate with RTC time
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_get_time(ds3231_t *dev, struct tm *time);

/**
 * @brief Set alarms
 *
 * @param[in] dev Device descriptor
 * @param[in] alarms Alarms to set (DS3231_ALARM_1, DS3231_ALARM_2, DS3231_ALARM_BOTH)
 * @param[in] time1 Time for alarm 1 (NULL if not setting alarm 1)
 * @param[in] option1 Alarm 1 rate option
 * @param[in] time2 Time for alarm 2 (NULL if not setting alarm 2)
 * @param[in] option2 Alarm 2 rate option
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_set_alarm(ds3231_t *dev, ds3231_alarm_t alarms, struct tm *time1,
                           ds3231_alarm1_rate_t option1, struct tm *time2, ds3231_alarm2_rate_t option2);

/**
 * @brief Check which alarm(s) have past
 *
 * @param[in] dev Device descriptor
 * @param[out] alarms Alarms
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_get_alarm_flags(ds3231_t *dev, ds3231_alarm_t *alarms);

/**
 * @brief Clear alarm past flag(s)
 *
 * @param[in] dev Device descriptor
 * @param[in] alarms Alarms
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_clear_alarm_flags(ds3231_t *dev, ds3231_alarm_t alarms);

/**
 * @brief enable alarm interrupts (and disables squarewave)
 *
 * @param[in] dev Device descriptor
 * @param[in] alarms Alarms
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_enable_alarm_ints(ds3231_t *dev, ds3231_alarm_t alarms);

/**
 * @brief Disable alarm interrupts
 *
 * Does not (re-)enable squarewave
 *
 * @param[in] dev Device descriptor
 * @param[in] alarms Alarms
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_disable_alarm_ints(ds3231_t *dev, ds3231_alarm_t alarms);

/**
 * @brief Set Unix timestamp on RTC
 *
 * @param[in] dev Device descriptor
 * @param[in] timestamp Unix timestamp in seconds
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_set_timestamp(ds3231_t *dev, uint32_t timestamp);

/**
 * @brief Get Unix timestamp from RTC
 *
 * @param[in] dev Device descriptor
 * @param[out] timestamp Unix timestamp in seconds
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ds3231_get_timestamp(ds3231_t *dev, uint32_t *timestamp);

#endif /* DS3231_H */
