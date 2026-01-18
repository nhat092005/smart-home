/**
 * @file json_helper.h
 *
 * @brief JSON Helper Functions API
 */

#ifndef JSON_HELPER_H
#define JSON_HELPER_H

/* Includes ------------------------------------------------------------------*/

#include "cJSON.h"
#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Get string value from JSON object safely
 *
 * @param[in] object JSON object
 * @param[in] key Key name
 * @param[in] default_val Default value if not found or not a string
 *
 * @return String value or default
 */
const char *json_helper_get_string(cJSON *object, const char *key, const char *default_val);

/**
 * @brief Get number value from JSON object safely
 *
 * @param[in] object JSON object
 * @param[in] key Key name
 * @param[in] default_val Default value if not found or not a number
 *
 * @return Number value or default
 */
double json_helper_get_number(cJSON *object, const char *key, double default_val);

/**
 * @brief Get integer value from JSON object safely
 *
 * @param[in] object JSON object
 * @param[in] key Key name
 * @param[in] default_val Default value if not found or not a number
 *
 * @return Integer value or default
 */
int json_helper_get_int(cJSON *object, const char *key, int default_val);

/**
 * @brief Get bool value from JSON object safely
 *
 * @param[in] object JSON object
 * @param[in] key Key name
 * @param[in] default_val Default value if not found or not a bool
 *
 * @return Bool value or default
 */
bool json_helper_get_bool(cJSON *object, const char *key, bool default_val);

/**
 * @brief Create sensor data JSON string for /data topic
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[in] temperature Temperature in Celsius
 * @param[in] humidity Humidity in percentage
 * @param[in] light Light level (lux)
 *
 * @return JSON string (caller must free) or NULL on error
 *
 * @note Format: {"timestamp": 1701388800, "temperature": 25.6, "humidity": 65.2, "light": 450}
 */
char *json_helper_create_data(uint32_t timestamp, float temperature, float humidity, int light);

/**
 * @brief Create device state JSON string for /state topic
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[in] mode Mode state (1=ON, 0=OFF)
 * @param[in] fan Fan state (1=ON, 0=OFF)
 * @param[in] light Light state (1=ON, 0=OFF)
 * @param[in] ac AC state (1=ON, 0=OFF)
 * @param[in] interval Data reporting interval in seconds
 *
 * @return JSON string (caller must free) or NULL on error
 *
 * @note Format: {"timestamp": 1701388800, "mode": 1, "interval": 5, "fan": 1, "light": 1, "ac": 1}
 */
char *json_helper_create_state(uint32_t timestamp, int mode, int interval, int fan, int light, int ac);

/**
 * @brief Create device info JSON string for /info topic
 *
 * @param[in] timestamp Unix timestamp in seconds
 * @param[in] device_id Device identifier
 * @param[in] ssid WiFi SSID
 * @param[in] ip IP address string
 * @param[in] broker MQTT broker URI
 * @param[in] firmware Firmware version string
 *
 * @return JSON string (caller must free) or NULL on error
 *
 * @note Format: {"timestamp": 1701388800, "id": "esp_01", "ssid": "MyHomeWiFi",
 *               "ip": "192.168.1.100", "broker": "mqtt://192.168.1.20:1883",
 *               "firmware": "1.0"}
 */
char *json_helper_create_info(uint32_t timestamp, const char *device_id, const char *ssid,
                              const char *ip, const char *broker, const char *firmware);

/**
 * @brief Create command response JSON string for /response topic
 *
 * @param[in] cmd_id Command ID
 * @param[in] status Status string
 *
 * @return JSON string (caller must free) or NULL on error
 *
 * @note Format: {"cmd_id": "1234", "status": "success"}
 */
char *json_helper_create_response(const char *cmd_id, const char *status);

/**
 * @brief Parse command from JSON string
 *
 * @param[in] json_str JSON string to parse
 * @param[out] cmd_id Output buffer for command ID (min 5 bytes)
 * @param[in] cmd_id_len Size of cmd_id buffer
 * @param[out] command Output buffer for command name (min 32 bytes)
 * @param[in] command_len Size of command buffer
 *
 * @return cJSON object (caller must cJSON_Delete) or NULL on error
 *
 * @note Call cJSON_GetObjectItem(result, "params") to get params object
 */
cJSON *json_helper_parse_command(const char *json_str, char *cmd_id, size_t cmd_id_len,
                                 char *command, size_t command_len);

/**
 * @brief Create WiFi scan result JSON array
 *
 * @param[in] ap_list Array of WiFi access point records
 * @param[in] ap_count Number of access points
 *
 * @return JSON string (caller must free) or NULL on error
 *
 * @note Format: {"ssid": "Network1", "rssi": -45, "auth": 3}, ...
 */
char *json_helper_create_wifi_scan_result(const void *ap_list, uint16_t ap_count);

/**
 * @brief Parse WiFi credentials from JSON
 *
 * @param[in] json_str JSON string to parse
 * @param[out] ssid_out Output buffer for SSID
 * @param[in] ssid_len Size of SSID buffer
 * @param[out] password_out Output buffer for password
 * @param[in] password_len Size of password buffer
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if invalid
 *
 * @note Extracts SSID and password from JSON object {"ssid": "...", "password": "..."}
 */
esp_err_t json_helper_parse_wifi_credentials(const char *json_str,
                                             char *ssid_out, size_t ssid_len,
                                             char *password_out, size_t password_len);

/**
 * @brief Create WiFi status JSON
 *
 * @param[in] connected WiFi connection status
 * @param[in] provisioned Provisioning status
 * @param[in] ip_address IP address string (can be NULL)
 * @param[in] rssi Signal strength (ignored if not connected)
 *
 * @return JSON string (caller must free) or NULL on error
 *
 * @note Format: {"connected": true, "provisioned": true, "ip": "192.168.1.100", "rssi": -45}
 */
char *json_helper_create_wifi_status(bool connected, bool provisioned,
                                     const char *ip_address, int8_t rssi);

/**
 * @brief Create simple response JSON
 *
 * @param[in] status Status string ("ok", "error", etc.)
 * @param[in] message Response message
 *
 * @return JSON string (caller must free) or NULL on error
 *
 * @note Format: {"status": "ok", "message": "Success"}
 */
char *json_helper_create_simple_response(const char *status, const char *message);

#endif /* JSON_HELPER_H */