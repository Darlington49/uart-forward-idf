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

#define PORT 8101
#define KEEPALIVE_IDLE 5
#define KEEPALIVE_INTERVAL 5
#define KEEPALIVE_COUNT 3

static const char *TAG_Telnet = "TAG_Telnet";
/** Uart Start**/
// #include "freertos/queue.h"
// #include "driver/uart.h"
// static const char *TAG = "uart_events";
// #define BUF_SIZE (1024)
// #define RD_BUF_SIZE (BUF_SIZE)
// typedef struct payload_ext_t
// {
//     char data[BUF_SIZE];
//     int count;
// } payload_ext_t;

// #define EX_UART_NUM UART_NUM_0
// #define PATTERN_CHR_NUM (3) /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

// static QueueHandle_t uart0_queue;
extern QueueHandle_t message_received_queue;

// static void uart_event_task(void *pvParameters)
// {
//     uart_event_t event;
//     size_t buffered_size;
//     uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE);
//     for (;;)
//     {
//         // Waiting for UART event.
//         if (xQueueReceive(uart0_queue, (void *)&event, (portTickType)portMAX_DELAY))
//         {
//             bzero(dtmp, RD_BUF_SIZE);
//             ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
//             switch (event.type)
//             {
//             // Event of UART receving data
//             /*We'd better handler data event fast, there would be much more data events than
//             other types of events. If we take too much time on data event, the queue might
//             be full.*/
//             case UART_DATA:
//                 ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
//                 uart_read_bytes(EX_UART_NUM, dtmp, event.size, portMAX_DELAY);
//                 // ESP_LOGI(TAG, "[DATA EVT]:");
//                 // uart_write_bytes(EX_UART_NUM, (const char *)dtmp, event.size);

//                 // Fill the queue with check mecanism
//                 payload_ext_t payload_ext;
//                 payload_ext.count = 5;
//                 strcpy(payload_ext.data, (const char *)dtmp);
//                 // check if the queue is not full if
//                 if (xQueueSend(message_received_queue, &payload_ext, 0) == pdTRUE)
//                 {
//                     ESP_LOGI(TAG, "message added to the queue");
//                 }
//                 else
//                 {
//                     ESP_LOGE(TAG, "message added to the queue maybe full");
//                 }
//                 break;
//             // Event of HW FIFO overflow detected
//             case UART_FIFO_OVF:
//                 ESP_LOGI(TAG, "hw fifo overflow");
//                 // If fifo overflow happened, you should consider adding flow control for your application.
//                 // The ISR has already reset the rx FIFO,
//                 // As an example, we directly flush the rx buffer here in order to read more data.
//                 uart_flush_input(EX_UART_NUM);
//                 xQueueReset(uart0_queue);
//                 break;
//             // Event of UART ring buffer full
//             case UART_BUFFER_FULL:
//                 ESP_LOGI(TAG, "ring buffer full");
//                 // If buffer full happened, you should consider encreasing your buffer size
//                 // As an example, we directly flush the rx buffer here in order to read more data.
//                 uart_flush_input(EX_UART_NUM);
//                 xQueueReset(uart0_queue);
//                 break;
//             // Event of UART RX break detected
//             case UART_BREAK:
//                 ESP_LOGI(TAG, "uart rx break");
//                 break;
//             // Event of UART parity check error
//             case UART_PARITY_ERR:
//                 ESP_LOGI(TAG, "uart parity error");
//                 break;
//             // Event of UART frame error
//             case UART_FRAME_ERR:
//                 ESP_LOGI(TAG, "uart frame error");
//                 break;
//             // UART_PATTERN_DET
//             case UART_PATTERN_DET:
//                 uart_get_buffered_data_len(EX_UART_NUM, &buffered_size);
//                 int pos = uart_pattern_pop_pos(EX_UART_NUM);
//                 ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
//                 if (pos == -1)
//                 {
//                     // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
//                     // record the position. We should set a larger queue size.
//                     // As an example, we directly flush the rx buffer here.
//                     uart_flush_input(EX_UART_NUM);
//                 }
//                 else
//                 {
//                     uart_read_bytes(EX_UART_NUM, dtmp, pos, 100 / portTICK_PERIOD_MS);
//                     uint8_t pat[PATTERN_CHR_NUM + 1];
//                     memset(pat, 0, sizeof(pat));
//                     uart_read_bytes(EX_UART_NUM, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
//                     ESP_LOGI(TAG, "read data: %s", dtmp);
//                     ESP_LOGI(TAG, "read pat : %s", pat);
//                 }
//                 break;
//             // Others
//             default:
//                 ESP_LOGI(TAG, "uart event type: %d", event.type);
//                 break;
//             }
//         }
//     }
//     free(dtmp);
//     dtmp = NULL;
//     vTaskDelete(NULL);
// }

// void uart_init()
// {
//     esp_log_level_set(TAG, ESP_LOG_INFO);

//     /* Configure parameters of an UART driver,
//      * communication pins and install the driver */
//     uart_config_t uart_config = {
//         .baud_rate = 115200,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//         .source_clk = UART_SCLK_APB,
//     };
//     // Install UART driver, and get the queue.
//     uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
//     uart_param_config(EX_UART_NUM, &uart_config);

//     // Set UART log level
//     esp_log_level_set(TAG, ESP_LOG_INFO);
//     // Set UART pins (using UART0 default pins ie no changes.)
//     uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

//     // Set uart pattern detect function.
//     uart_enable_pattern_det_baud_intr(EX_UART_NUM, '+', PATTERN_CHR_NUM, 9, 0, 0);
//     // Reset the pattern queue length to record at most 20 pattern positions.
//     uart_pattern_queue_reset(EX_UART_NUM, 20);
// }

/** Uart end**/
static char tag[] = "telnet";

// static void do_retransmit(const int sock)
// {
//     int len;
//     char rx_buffer[128];

//     do
//     {
//         // int written = send(sock, "1234", 4, 0);

//         // if (written < 0)
//         // {
//         //     ESP_LOGE(TAG_Telnet, "Error occurred during sending: errno %d (%s)", errno, strerror(errno));
//         // }

//         len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
//         if (len < 0)
//         {
//             ESP_LOGE(TAG_Telnet, "Error occurred during receiving: errno %d", errno);
//         }
//         else if (len == 0)
//         {
//             ESP_LOGW(TAG_Telnet, "Connection closed");
//         }
//         else
//         {
//             rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
//             ESP_LOGI(TAG_Telnet, "Received %d bytes: %s", len, rx_buffer);

//             // send() can return less bytes than supplied length.
//             // Walk-around for robust implementation.
//             int to_write = len;
//             while (to_write > 0)
//             {
//                 int written = send(sock, rx_buffer + (len - to_write), to_write, 0);
//                 if (written < 0)
//                 {
//                     ESP_LOGE(TAG_Telnet, "Error occurred during sending: errno %d", errno);
//                 }
//                 to_write -= written;
//             }
//         }
//     } while (len > 0);
// }

static void tcp_server_task(void *pvParameters)
{
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;
    payload_ext_t payload_ext;

    if (addr_family == AF_INET)
    {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;
    }

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0)
    {
        ESP_LOGE(TAG_Telnet, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG_Telnet, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0)
    {
        ESP_LOGE(TAG_Telnet, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG_Telnet, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG_Telnet, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0)
    {
        ESP_LOGE(TAG_Telnet, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1)
    {

        ESP_LOGI(TAG_Telnet, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            ESP_LOGE(TAG_Telnet, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string
        if (source_addr.ss_family == PF_INET)
        {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }

        ESP_LOGI(TAG_Telnet, "Socket accepted ip address: %s", addr_str);

        for (;;)
        {
            ESP_LOGI(TAG, "Check the queue");
            // Waiting for the queue UART event.
            if (xQueueReceive(message_received_queue, (void *)&payload_ext, 1000 / portTICK_PERIOD_MS))
            {
                ESP_LOGI(TAG, "Received Data from the queue");
                int written = send(sock, payload_ext.data, strlen(payload_ext.data), 0);

                if (written < 0)
                {
                    ESP_LOGE(TAG_Telnet, "Error occurred during sending: errno %d (%s)", errno, strerror(errno));
                    // elementr retreived has tobe added again in front or in back has ro be reviewd and taken in consideration
                    // shutdown(sock, 0);
                    // close(sock);
                    break;
                }
            }
            else
            {
                ESP_LOGE(TAG, "Queue may be void");
            }
            // vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        shutdown(sock, 0);
        close(sock);
    }

CLEAN_UP:
    close(listen_sock);
    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);
    esp_log_level_set(tag, ESP_LOG_INFO);
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
