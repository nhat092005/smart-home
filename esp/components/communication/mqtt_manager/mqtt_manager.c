/**
 * @file mqtt_manager.c
 *
 * @brief MQTT Manager Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "mqtt_manager.h"
#include "json_helper.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_crt_bundle.h"
#include <string.h>

/* Private defines -----------------------------------------------------------*/

// Buffer sizes
#define MQTT_TOPIC_MAX_LEN 128
#define MQTT_CMD_ID_MAX_LEN 8
#define MQTT_CMD_MAX_LEN 32

/* Exported variables --------------------------------------------------------*/

bool isMQTT = false; //!< Global MQTT connection state indicator

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "MQTT_MANAGER";

static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false;

// Event callbacks
static mqtt_event_callback_t connected_callback = NULL;
static mqtt_event_callback_t disconnected_callback = NULL;

// Command callback
static mqtt_command_callback_t command_callback = NULL;

// Dynamic topics built from Kconfig
static char topic_data[MQTT_TOPIC_MAX_LEN];    // QoS=0, Retain=No
static char topic_state[MQTT_TOPIC_MAX_LEN];   // QoS=1, Retain=Yes
static char topic_info[MQTT_TOPIC_MAX_LEN];    // QoS=1, Retain=Yes
static char topic_command[MQTT_TOPIC_MAX_LEN]; // QoS=1, Retain=No

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Build MQTT topic strings from Kconfig
 */
static void mqtt_manager_build_topics(void);

/**
 * @brief Handle incoming command JSON string
 *
 * @param[in] json_str JSON string received on command topic
 */
static void mqtt_manager_handle_command(const char *json_str);

/**
 * @brief MQTT event handler
 *
 * @param[in] handler_args Handler arguments (not used)
 * @param[in] base Event base
 * @param[in] event_id Event ID
 * @param[in] event_data Event data
 */
static void mqtt_manager_event_handler(void *handler_args, esp_event_base_t base,
                                       int32_t event_id, void *event_data);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize MQTT manager
 */
esp_err_t mqtt_manager_init(void)
{
    ESP_LOGI(TAG, "Initializing MQTT Manager");

    ESP_LOGI(TAG, "Device ID: %s", MQTT_DEVICE_ID);
    ESP_LOGI(TAG, "Broker: %s", MQTT_BROKER_URI);
    ESP_LOGI(TAG, "Broker Port: %d", MQTT_BROKER_PORT);

    mqtt_manager_build_topics();

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .hostname = MQTT_BROKER_URI,
                .transport = MQTT_TRANSPORT_OVER_SSL,
                .port = MQTT_BROKER_PORT,
            },
            .verification = {
                .crt_bundle_attach = esp_crt_bundle_attach,
            }},
        .credentials = {.client_id = MQTT_DEVICE_ID, .username = MQTT_USERNAME, .authentication = {
                                                                                    .password = MQTT_PASSWORD,
                                                                                }},
        .session = {
            .protocol_ver = MQTT_PROTOCOL_V_3_1_1,
            .keepalive = MQTT_KEEP_ALIVE_SEC,
            .disable_clean_session = 0,
        }};

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "MQTT client initialization failed");
        return ESP_FAIL;
    }

    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID,
                                   mqtt_manager_event_handler, NULL);

    ESP_LOGI(TAG, "MQTT Manager initialized successfully");
    return ESP_OK;
}

/**
 * @brief Start MQTT client
 */
esp_err_t mqtt_manager_start(void)
{
    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return ESP_FAIL;
    }

    esp_err_t ret = esp_mqtt_client_start(mqtt_client);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "MQTT client start failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "MQTT client started");
    return ESP_OK;
}

/**
 * @brief Stop MQTT client
 */
esp_err_t mqtt_manager_stop(void)
{
    if (mqtt_client == NULL)
    {
        return ESP_OK;
    }

    esp_mqtt_client_stop(mqtt_client);
    mqtt_connected = false;
    isMQTT = false; // Update global state

    ESP_LOGI(TAG, "MQTT client stopped");
    return ESP_OK;
}

/**
 * @brief Check if MQTT is connected
 */
bool mqtt_manager_is_connected(void)
{
    return mqtt_connected;
}

/**
 * @brief Publish sensor data
 */
esp_err_t mqtt_manager_publish_data(uint32_t timestamp, float temperature, float humidity, int light)
{
    if (!mqtt_connected)
    {
        ESP_LOGW(TAG, "MQTT not connected, skipping data publish");
        return ESP_ERR_INVALID_STATE;
    }

    char *json = json_helper_create_data(timestamp, temperature, humidity, light);
    if (json == NULL)
    {
        ESP_LOGE(TAG, "Failed to create data JSON");
        return ESP_FAIL;
    }

    // Publish to data topic (QoS=0, no retain)
    int msg_id = esp_mqtt_client_publish(mqtt_client, topic_data, json, 0,
                                         MQTT_QOS_0, MQTT_RETAIN_OFF);

    if (msg_id < 0)
    {
        ESP_LOGE(TAG, "Failed to publish data");
    }

    free(json);
    return (msg_id >= 0) ? ESP_OK : ESP_FAIL;
}

/**
 * @brief Publish device state
 */
esp_err_t mqtt_manager_publish_state(uint32_t timestamp, int mode, int interval, int fan, int light, int ac)
{
    if (!mqtt_connected)
    {
        ESP_LOGW(TAG, "MQTT not connected, skipping state publish");
        return ESP_ERR_INVALID_STATE;
    }

    char *json = json_helper_create_state(timestamp, mode, interval, fan, light, ac);
    if (json == NULL)
    {
        ESP_LOGE(TAG, "Failed to create state JSON");
        return ESP_FAIL;
    }

    // Publish to state topic (QoS=1, retain)
    int msg_id = esp_mqtt_client_publish(mqtt_client, topic_state, json, 0,
                                         MQTT_QOS_1, MQTT_RETAIN_ON);

    if (msg_id < 0)
    {
        ESP_LOGE(TAG, "Failed to publish state");
    }

    free(json);
    return (msg_id >= 0) ? ESP_OK : ESP_FAIL;
}

/**
 * @brief Publish device info
 */
esp_err_t mqtt_manager_publish_info(uint32_t timestamp, const char *device_id, const char *ssid,
                                    const char *ip, const char *broker, const char *firmware)
{
    if (!mqtt_connected)
    {
        ESP_LOGW(TAG, "MQTT not connected, skipping info publish");
        return ESP_ERR_INVALID_STATE;
    }

    char *json = json_helper_create_info(timestamp, device_id, ssid, ip, broker, firmware);
    if (json == NULL)
    {
        ESP_LOGE(TAG, "Failed to create info JSON");
        return ESP_FAIL;
    }

    // Publish to info topic (QoS=1, retain)
    int msg_id = esp_mqtt_client_publish(mqtt_client, topic_info, json, 0,
                                         MQTT_QOS_1, MQTT_RETAIN_ON);

    if (msg_id < 0)
    {
        ESP_LOGE(TAG, "Failed to publish info");
    }

    free(json);
    return (msg_id >= 0) ? ESP_OK : ESP_FAIL;
}

/**
 * @brief Register command callback
 */
void mqtt_manager_register_command_callback(mqtt_command_callback_t callback)
{
    command_callback = callback;
    ESP_LOGI(TAG, "Command callback registered");
}

/**
 * @brief Register connected event callback
 */
void mqtt_manager_register_connected_callback(mqtt_event_callback_t callback)
{
    connected_callback = callback;
    ESP_LOGI(TAG, "Connected callback registered");
}

/**
 * @brief Register disconnected event callback
 */
void mqtt_manager_register_disconnected_callback(mqtt_event_callback_t callback)
{
    disconnected_callback = callback;
    ESP_LOGI(TAG, "Disconnected callback registered");
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Build MQTT topic strings from Kconfig
 */
static void mqtt_manager_build_topics(void)
{
    const char *base = MQTT_BASE_TOPIC;
    const char *device_id = MQTT_DEVICE_ID;

    int ret;

    ret = snprintf(topic_data, sizeof(topic_data), MQTT_TOPIC_DATA_FMT, base, device_id);
    if (ret >= sizeof(topic_data))
    {
        ESP_LOGW(TAG, "Data topic truncated");
    }

    ret = snprintf(topic_state, sizeof(topic_state), MQTT_TOPIC_STATE_FMT, base, device_id);
    if (ret >= sizeof(topic_state))
    {
        ESP_LOGW(TAG, "State topic truncated");
    }

    ret = snprintf(topic_info, sizeof(topic_info), MQTT_TOPIC_INFO_FMT, base, device_id);
    if (ret >= sizeof(topic_info))
    {
        ESP_LOGW(TAG, "Info topic truncated");
    }

    ret = snprintf(topic_command, sizeof(topic_command), MQTT_TOPIC_COMMAND_FMT, base, device_id);
    if (ret >= sizeof(topic_command))
    {
        ESP_LOGW(TAG, "Command topic truncated");
    }

    ESP_LOGI(TAG, "DataData:  %s (QoS=0, Retain=No)", topic_data);
    ESP_LOGI(TAG, "State: %s (QoS=1, Retain=Yes)", topic_state);
    ESP_LOGI(TAG, "Info: %s (QoS=1, Retain=Yes)", topic_info);
    ESP_LOGI(TAG, "Command: %s (QoS=1, Retain=No)", topic_command);
}

/**
 * @brief Handle incoming command JSON string
 */
static void mqtt_manager_handle_command(const char *json_str)
{
    if (command_callback == NULL)
    {
        ESP_LOGW(TAG, "No command callback registered");
        return;
    }

    char cmd_id[MQTT_CMD_ID_MAX_LEN];
    char command[MQTT_CMD_MAX_LEN];

    cJSON *root = json_helper_parse_command(json_str, cmd_id, sizeof(cmd_id),
                                            command, sizeof(command));

    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to parse command JSON");
        return;
    }

    ESP_LOGI(TAG, "Command received - ID: %s, Command: %s", cmd_id, command);

    // Get params object (can be NULL)
    cJSON *params = cJSON_GetObjectItem(root, "params");

    // Invoke callback
    command_callback(cmd_id, command, params);

    cJSON_Delete(root);
}

/**
 * @brief MQTT event handler
 */
static void mqtt_manager_event_handler(void *handler_args, esp_event_base_t base,
                                       int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT Connected to broker");

        isMQTT = true; // Update global state
        mqtt_connected = true;

        // Subscribe to command topic
        esp_mqtt_client_subscribe(mqtt_client, topic_command, MQTT_QOS_1);
        ESP_LOGI(TAG, "Subscribed to: %s (QoS=1)", topic_command);

        // Notify application
        if (connected_callback)
        {
            connected_callback();
        }
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT Disconnected");

        isMQTT = false; // Update global state
        mqtt_connected = false;

        // Notify application
        if (disconnected_callback)
        {
            disconnected_callback();
        }
        break;

    case MQTT_EVENT_DATA:
        // Process command messages
        if (event->topic_len == strlen(topic_command) &&
            strncmp(event->topic, topic_command, event->topic_len) == 0)
        {
            char *json_str = strndup(event->data, event->data_len);
            if (json_str)
            {
                mqtt_manager_handle_command(json_str);
                free(json_str);
            }
            else
            {
                ESP_LOGE(TAG, "Failed to allocate memory for command");
            }
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT Error");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            ESP_LOGE(TAG, "TLS error: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGE(TAG, "TLS stack: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGE(TAG, "Socket errno: %d (%s)", event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        }
        else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED)
        {
            ESP_LOGE(TAG, "Connection refused: 0x%x", event->error_handle->connect_return_code);
        }
        break;

    default:
        break;
    }
}