/**
 * @file webserver.h
 *
 * @brief WiFi Manager Web Server API
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

/* Includes ------------------------------------------------------------------*/

#include <esp_err.h>

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Start the web server
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t webserver_start(void);

/**
 * @brief Stop the web server
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t webserver_stop(void);

#endif /* WEBSERVER_H */
