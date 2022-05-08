#include <stdio.h>
#include "connect.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"

#include "telnet.h"

#include <string.h>
#include <sys/param.h>
#include "esp_wifi.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "serial.h"
#include "tcp.h"

#define PORT 8101
#define KEEPALIVE_IDLE 5
#define KEEPALIVE_INTERVAL 5
#define KEEPALIVE_COUNT 3

// extern QueueHandle_t message_received_queue;

// static void tcp_server_task(void *pvParameters)
// {
//     char addr_str[128];
//     int addr_family = (int)pvParameters;
//     int ip_protocol = 0;
//     int keepAlive = 1;
//     int keepIdle = KEEPALIVE_IDLE;
//     int keepInterval = KEEPALIVE_INTERVAL;
//     int keepCount = KEEPALIVE_COUNT;
//     struct sockaddr_storage dest_addr;
//     payload_ext_t payload_ext;

//     if (addr_family == AF_INET)
//     {
//         struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
//         dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
//         dest_addr_ip4->sin_family = AF_INET;
//         dest_addr_ip4->sin_port = htons(PORT);
//         ip_protocol = IPPROTO_IP;
//     }

//     int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
//     if (listen_sock < 0)
//     {
//         ESP_LOGE(TAG_Telnet, "Unable to create socket: errno %d", errno);
//         vTaskDelete(NULL);
//         return;
//     }
//     int opt = 1;
//     setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     ESP_LOGI(TAG_Telnet, "Socket created");

//     int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
//     if (err != 0)
//     {
//         ESP_LOGE(TAG_Telnet, "Socket unable to bind: errno %d", errno);
//         ESP_LOGE(TAG_Telnet, "IPPROTO: %d", addr_family);
//         goto CLEAN_UP;
//     }
//     ESP_LOGI(TAG_Telnet, "Socket bound, port %d", PORT);

//     err = listen(listen_sock, 1);
//     if (err != 0)
//     {
//         ESP_LOGE(TAG_Telnet, "Error occurred during listen: errno %d", errno);
//         goto CLEAN_UP;
//     }

//     while (1)
//     {

//         ESP_LOGI(TAG_Telnet, "Socket listening");

//         struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
//         socklen_t addr_len = sizeof(source_addr);
//         int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
//         if (sock < 0)
//         {
//             ESP_LOGE(TAG_Telnet, "Unable to accept connection: errno %d", errno);
//             break;
//         }

//         // Set tcp keepalive option
//         setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
//         setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
//         setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
//         setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
//         // Convert ip address to string
//         if (source_addr.ss_family == PF_INET)
//         {
//             inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
//         }

//         ESP_LOGI(TAG_Telnet, "Socket accepted ip address: %s", addr_str);

//         for (;;)
//         {
//             ESP_LOGI(TAG, "Check the queue");
//             // Waiting for the queue UART event.
//             if (xQueueReceive(message_received_queue, (void *)&payload_ext, 1000 / portTICK_PERIOD_MS))
//             {
//                 ESP_LOGI(TAG, "Received Data from the queue");
//                 int written = send(sock, payload_ext.data, strlen(payload_ext.data), 0);

//                 if (written < 0)
//                 {
//                     ESP_LOGE(TAG_Telnet, "Error occurred during sending: errno %d (%s)", errno, strerror(errno));
//                     // elementr retreived has tobe added again in front or in back has ro be reviewd and taken in consideration
//                     // shutdown(sock, 0);
//                     // close(sock);
//                     break;
//                 }
//             }
//             else
//             {
//                 ESP_LOGE(TAG, "Queue may be void");
//             }
//             // vTaskDelay(1000 / portTICK_PERIOD_MS);
//         }

//         shutdown(sock, 0);
//         close(sock);
//     }

// CLEAN_UP:
//     close(listen_sock);
//     vTaskDelete(NULL);
// }

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);
    // esp_log_level_set(tag, ESP_LOG_INFO);
    ESP_ERROR_CHECK(nvs_flash_init());
    // Uart Queue Creation;
    message_received_queue = xQueueCreate(10, sizeof(payload_ext_t));
    // Uart init
    uart_init();
    // Uart Task creation
    xTaskCreate(uart_event_task, "uart_event_task", 4096, NULL, 12, NULL);
    wifi_init();
    ESP_ERROR_CHECK(wifi_connect_sta("AndroidAP9CB9", "00000000", 10000));
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void *)AF_INET, 5, NULL);
}
