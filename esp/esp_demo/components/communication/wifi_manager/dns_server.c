/**
 * @file dns_server.c
 *
 * @brief DNS Server Implementation
 */

/* Includes ------------------------------------------------------------------*/

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "wifi_config.h"
#include <string.h>

/* Private defines -----------------------------------------------------------*/

#define DNS_MAX_PACKET_SIZE 512

/* Private defines -----------------------------------------------------------*/

static const char *TAG = "DNS_SERVER";
static TaskHandle_t g_dns_task = NULL;
static int g_dns_socket = -1;

/* Private types ------------------------------------------------------------*/

/**
 * @brief DNS header structure
 */
typedef struct __attribute__((packed))
{
    uint16_t id;         //!< Identification
    uint16_t flags;      //!< Flags
    uint16_t questions;  //!< Number of questions
    uint16_t answers;    //!< Number of answers
    uint16_t authority;  //!< Number of authority records
    uint16_t additional; //!< Number of additional records
} dns_header_t;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief DNS server task
 */
static void dns_server_task(void *pvParameters);

/* Exported functions ---------------------------------------------------------*/

/**
 * @brief Start DNS server task
 */
esp_err_t dns_server_start(void)
{
    if (g_dns_task != NULL)
    {
        ESP_LOGW(TAG, "DNS server already running");
        return ESP_OK;
    }

    BaseType_t ret = xTaskCreate(dns_server_task, "dns_server", 4096, NULL, 5, &g_dns_task);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create DNS server task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "DNS server task created");
    return ESP_OK;
}

/**
 * @brief Stop DNS server task
 */
esp_err_t dns_server_stop(void)
{
    if (g_dns_task == NULL)
    {
        return ESP_OK;
    }

    // Close socket to break recvfrom() loop
    if (g_dns_socket >= 0)
    {
        close(g_dns_socket);
        g_dns_socket = -1;
    }

    // Wait for task to self-cleanup
    vTaskDelay(pdMS_TO_TICKS(100));

    // If still running, force delete
    if (g_dns_task != NULL)
    {
        vTaskDelete(g_dns_task);
        g_dns_task = NULL;
    }

    ESP_LOGI(TAG, "DNS server stopped");
    return ESP_OK;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief DNS server task
 */
static void dns_server_task(void *pvParameters)
{
    // Create UDP socket
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    socklen_t client_addr_len = sizeof(client_addr);
    uint8_t rx_buffer[DNS_MAX_PACKET_SIZE];
    uint8_t tx_buffer[DNS_MAX_PACKET_SIZE];

    g_dns_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (g_dns_socket < 0)
    {
        ESP_LOGE(TAG, "Failed to create socket");
        g_dns_task = NULL; // Clear task handle before exit
        vTaskDelete(NULL);
        return;
    }

    // Bind socket to DNS port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DNS_SERVER_PORT);

    if (bind(g_dns_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        ESP_LOGE(TAG, "Failed to bind socket");
        close(g_dns_socket);
        g_dns_socket = -1;
        g_dns_task = NULL; // Clear task handle before exit
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "DNS server started on port %d", DNS_SERVER_PORT);

    while (1)
    {
        int len = recvfrom(g_dns_socket, rx_buffer, sizeof(rx_buffer), 0,
                           (struct sockaddr *)&client_addr, &client_addr_len);

        // Check if socket closed (stop requested)
        if (len < 0)
        {
            break;
        }

        if (len < sizeof(dns_header_t))
        {
            continue;
        }

        dns_header_t *header = (dns_header_t *)rx_buffer;

        if (ntohs(header->questions) != 1)
        {
            continue;
        }

        memcpy(tx_buffer, rx_buffer, len);
        dns_header_t *response_header = (dns_header_t *)tx_buffer;

        response_header->flags = htons(0x8180);
        response_header->answers = htons(1);

        uint8_t *answer_ptr = tx_buffer + len;

        *answer_ptr++ = 0xC0;
        *answer_ptr++ = 0x0C;

        *answer_ptr++ = 0x00;
        *answer_ptr++ = 0x01;

        *answer_ptr++ = 0x00;
        *answer_ptr++ = 0x01;

        *answer_ptr++ = 0x00;
        *answer_ptr++ = 0x00;
        *answer_ptr++ = 0x00;
        *answer_ptr++ = 0x3C;

        *answer_ptr++ = 0x00;
        *answer_ptr++ = 0x04;

        *answer_ptr++ = 192;
        *answer_ptr++ = 168;
        *answer_ptr++ = 4;
        *answer_ptr++ = 1;

        int response_len = answer_ptr - tx_buffer;

        sendto(g_dns_socket, tx_buffer, response_len, 0,
               (struct sockaddr *)&client_addr, client_addr_len);
    }

    // Task cleanup
    if (g_dns_socket >= 0)
    {
        close(g_dns_socket);
        g_dns_socket = -1;
    }
    g_dns_task = NULL;
    ESP_LOGI(TAG, "DNS server task exiting");
    vTaskDelete(NULL);
}