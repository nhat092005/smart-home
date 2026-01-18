/**
 * @file json_helper.c
 *
 * @brief JSON Helper Functions Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "json_helper.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include <string.h>
#include <math.h>
#include <time.h>

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "JSON_HELPER";

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Get string value from JSON object safely
 */
const char *json_helper_get_string(cJSON *object, const char *key, const char *default_val)
{
    if (object == NULL || key == NULL)
    {
        return default_val;
    }

    cJSON *item = cJSON_GetObjectItem(object, key);
    if (cJSON_IsString(item))
    {
        return item->valuestring;
    }

    return default_val;
}

/**
 * @brief Get number value from JSON object safely
 */
double json_helper_get_number(cJSON *object, const char *key, double default_val)
{
    if (object == NULL || key == NULL)
    {
        return default_val;
    }

    cJSON *item = cJSON_GetObjectItem(object, key);
    if (cJSON_IsNumber(item))
    {
        return item->valuedouble;
    }

    return default_val;
}

/**
 * @brief Get integer value from JSON object safely
 */
int json_helper_get_int(cJSON *object, const char *key, int default_val)
{
    if (object == NULL || key == NULL)
    {
        return default_val;
    }

    cJSON *item = cJSON_GetObjectItem(object, key);
    if (cJSON_IsNumber(item))
    {
        return item->valueint;
    }

    return default_val;
}

/**
 * @brief Get bool value from JSON object safely
 */
bool json_helper_get_bool(cJSON *object, const char *key, bool default_val)
{
    if (object == NULL || key == NULL)
    {
        return default_val;
    }

    cJSON *item = cJSON_GetObjectItem(object, key);
    if (cJSON_IsBool(item))
    {
        return cJSON_IsTrue(item);
    }

    return default_val;
}

/**
 * @brief Create sensor data JSON string
 */
char *json_helper_create_data(uint32_t timestamp, float temperature, float humidity, int light)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    cJSON_AddNumberToObject(root, "timestamp", timestamp);

    // Round temperature and humidity to 2 decimal places
    double temp_rounded = round(temperature * 100.0) / 100.0;
    double hum_rounded = round(humidity * 100.0) / 100.0;

    cJSON_AddNumberToObject(root, "temperature", temp_rounded);
    cJSON_AddNumberToObject(root, "humidity", hum_rounded);
    cJSON_AddNumberToObject(root, "light", light);

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);

    if (json_str == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
        return NULL;
    }

    return json_str;
}

/**
 * @brief Create device state JSON string
 */
char *json_helper_create_state(uint32_t timestamp, int mode, int interval, int fan, int light, int ac)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    cJSON_AddNumberToObject(root, "timestamp", timestamp);
    cJSON_AddNumberToObject(root, "mode", mode);
    cJSON_AddNumberToObject(root, "interval", interval);
    cJSON_AddNumberToObject(root, "fan", fan);
    cJSON_AddNumberToObject(root, "light", light);
    cJSON_AddNumberToObject(root, "ac", ac);

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);

    if (json_str == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
        return NULL;
    }

    return json_str;
}

/**
 * @brief Create device info JSON string
 */
char *json_helper_create_info(uint32_t timestamp, const char *device_id, const char *ssid,
                              const char *ip, const char *broker, const char *firmware)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    cJSON_AddNumberToObject(root, "timestamp", timestamp);

    if (device_id != NULL)
    {
        cJSON_AddStringToObject(root, "id", device_id);
    }

    if (ssid != NULL)
    {
        cJSON_AddStringToObject(root, "ssid", ssid);
    }

    if (ip != NULL)
    {
        cJSON_AddStringToObject(root, "ip", ip);
    }

    if (broker != NULL)
    {
        cJSON_AddStringToObject(root, "broker", broker);
    }

    if (firmware != NULL)
    {
        cJSON_AddStringToObject(root, "firmware", firmware);
    }

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);

    if (json_str == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
        return NULL;
    }

    return json_str;
}

/**
 * @brief Create command response JSON string
 */
char *json_helper_create_response(const char *cmd_id, const char *status)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON object");
    }

    if (cmd_id != NULL)
    {
        cJSON_AddStringToObject(root, "cmd_id", cmd_id);
    }

    if (status != NULL)
    {
        cJSON_AddStringToObject(root, "status", status);
    }

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);

    if (json_str == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
        return NULL;
    }

    return json_str;
}

/**
 * @brief Parse command from JSON string
 */
cJSON *json_helper_parse_command(const char *json_str, char *cmd_id, size_t cmd_id_len,
                                 char *command, size_t command_len)
{
    if (json_str == NULL || cmd_id == NULL || command == NULL ||
        cmd_id_len == 0 || command_len == 0)
    {
        ESP_LOGE(TAG, "Invalid parameters");
        return NULL;
    }

    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        ESP_LOGE(TAG, "JSON parse error: %s", error_ptr ? error_ptr : "unknown");
        return NULL;
    }

    // Extract "id" field
    cJSON *id_item = cJSON_GetObjectItem(root, "id");
    if (!cJSON_IsString(id_item))
    {
        ESP_LOGE(TAG, "Command ID field not found or not a string");
        cJSON_Delete(root);
        return NULL;
    }

    strncpy(cmd_id, id_item->valuestring, cmd_id_len - 1);
    cmd_id[cmd_id_len - 1] = '\0';

    // Extract "command" field
    cJSON *cmd_item = cJSON_GetObjectItem(root, "command");
    if (!cJSON_IsString(cmd_item))
    {
        ESP_LOGE(TAG, "Command field not found or not a string");
        cJSON_Delete(root);
        return NULL;
    }

    strncpy(command, cmd_item->valuestring, command_len - 1);
    command[command_len - 1] = '\0';

    // Return root object so caller can access "params"
    return root;
}

/**
 * @brief Create WiFi scan result JSON array
 */
char *json_helper_create_wifi_scan_result(const void *ap_list, uint16_t ap_count)
{
    if (ap_list == NULL)
    {
        ESP_LOGE(TAG, "Invalid parameter: ap_list is NULL");
        return NULL;
    }

    const wifi_ap_record_t *ap_records = (const wifi_ap_record_t *)ap_list;

    cJSON *root = cJSON_CreateArray();
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON array");
        return NULL;
    }

    for (int i = 0; i < ap_count; i++)
    {
        cJSON *item = cJSON_CreateObject();
        if (item == NULL)
        {
            ESP_LOGE(TAG, "Failed to create JSON object for AP %d", i);
            cJSON_Delete(root);
            return NULL;
        }

        // Ensure SSID is null-terminated (max 32 bytes + null)
        char ssid_str[33];
        memcpy(ssid_str, ap_records[i].ssid, 32);
        ssid_str[32] = '\0';

        cJSON_AddStringToObject(item, "ssid", ssid_str);
        cJSON_AddNumberToObject(item, "rssi", ap_records[i].rssi);
        cJSON_AddNumberToObject(item, "auth", ap_records[i].authmode);
        cJSON_AddItemToArray(root, item);
    }

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);

    if (json_str == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
        return NULL;
    }

    return json_str;
}

/**
 * @brief Parse WiFi credentials from JSON string
 */
esp_err_t json_helper_parse_wifi_credentials(const char *json_str,
                                             char *ssid_out, size_t ssid_len,
                                             char *password_out, size_t password_len)
{
    if (json_str == NULL || ssid_out == NULL || password_out == NULL)
    {
        ESP_LOGE(TAG, "Invalid parameters");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        ESP_LOGE(TAG, "JSON parse error: %s", error_ptr ? error_ptr : "unknown");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *ssid_json = cJSON_GetObjectItem(root, "ssid");
    if (!cJSON_IsString(ssid_json))
    {
        ESP_LOGE(TAG, "SSID field not found or not a string");
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    const char *ssid = ssid_json->valuestring;

    // Validate SSID length (WiFi SSID max 32 bytes, need null terminator)
    if (strlen(ssid) == 0 || strlen(ssid) > 31)
    {
        ESP_LOGE(TAG, "Invalid SSID length: %d", strlen(ssid));
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    // Copy SSID
    strncpy(ssid_out, ssid, ssid_len - 1);
    ssid_out[ssid_len - 1] = '\0';

    // Get password (optional)
    cJSON *pass_json = cJSON_GetObjectItem(root, "password");
    if (cJSON_IsString(pass_json))
    {
        const char *password = pass_json->valuestring;

        // Validate password length (WiFi password max 63 characters)
        if (strlen(password) > 63)
        {
            ESP_LOGE(TAG, "Password too long: %d (max 63)", strlen(password));
            cJSON_Delete(root);
            return ESP_ERR_INVALID_ARG;
        }

        strncpy(password_out, password, password_len - 1);
        password_out[password_len - 1] = '\0';
    }
    else
    {
        password_out[0] = '\0';
    }

    cJSON_Delete(root);
    return ESP_OK;
}

/**
 * @brief Create WiFi status JSON string
 */
char *json_helper_create_wifi_status(bool connected, bool provisioned,
                                     const char *ip_address, int8_t rssi)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    cJSON_AddBoolToObject(root, "connected", connected);
    cJSON_AddBoolToObject(root, "provisioned", provisioned);

    if (connected)
    {
        if (ip_address != NULL && strlen(ip_address) > 0)
        {
            cJSON_AddStringToObject(root, "ip", ip_address);
        }
        cJSON_AddNumberToObject(root, "rssi", rssi);
    }

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);

    if (json_str == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
        return NULL;
    }

    return json_str;
}

/**
 * @brief Create simple JSON response with status and message
 */
char *json_helper_create_simple_response(const char *status, const char *message)
{
    if (status == NULL || message == NULL)
    {
        ESP_LOGE(TAG, "Invalid parameters: status or message is NULL");
        return NULL;
    }

    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return NULL;
    }

    cJSON_AddStringToObject(root, "status", status);
    cJSON_AddStringToObject(root, "message", message);

    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);

    if (json_str == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON");
        return NULL;
    }

    return json_str;
}
