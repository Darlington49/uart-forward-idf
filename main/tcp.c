#include "tcp.h"
const static char *TAG_Telnet = "TAG_Telnet";

void do_retransmit(const int sock)
{
    int len;
    char rx_buffer[128];

    do
    {
        // int written = send(sock, "1234", 4, 0);

        // if (written < 0)
        // {
        //     ESP_LOGE(TAG_Telnet, "Error occurred during sending: errno %d (%s)", errno, strerror(errno));
        // }

        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0)
        {
            ESP_LOGE(TAG_Telnet, "Error occurred during receiving: errno %d", errno);
        }
        else if (len == 0)
        {
            ESP_LOGW(TAG_Telnet, "Connection closed");
        }
        else
        {
            rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
            ESP_LOGI(TAG_Telnet, "Received %d bytes: %s", len, rx_buffer);

            // send() can return less bytes than supplied length.
            // Walk-around for robust implementation.
            int to_write = len;
            while (to_write > 0)
            {
                int written = send(sock, rx_buffer + (len - to_write), to_write, 0);
                if (written < 0)
                {
                    ESP_LOGE(TAG_Telnet, "Error occurred during sending: errno %d", errno);
                }
                to_write -= written;
            }
        }
    } while (len > 0);
}

void tcp_server_task(void *pvParameters)
{
    esp_log_level_set(TAG_Telnet, ESP_LOG_INFO);
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
            ESP_LOGI(TAG_Telnet, "Check the queue");
            // Waiting for the queue UART event.
            if (xQueueReceive(message_tcp_queue, (void *)&payload_ext, 1000 / portTICK_PERIOD_MS))
            {
                ESP_LOGI(TAG_Telnet, "Received Data from tcp queue");
                int written = send(sock, payload_ext.data, strlen(payload_ext.data), 0);

                if (written < 0)
                {
                    ESP_LOGE(TAG_Telnet, "Error occurred during sending: errno %d (%s)", errno, strerror(errno));
                    // elementr retreived has tobe added again in front or in back has ro be reviewd and taken in consideration
                    // two elements are lost
                    // https://www.freertos.org/xQueueSendToFront.html
                    break;
                }
            }
            else
            {
                ESP_LOGE(TAG_Telnet, "tcp Queue may be void");
            }

            // Waiting for the queue UART event.
            if (xQueueReceive(message_tcp_queue, (void *)&payload_ext, 1000 / portTICK_PERIOD_MS))
            {
                ESP_LOGI(TAG_Telnet, "Received Data from file queue");
                appendFile("/hello658.txt", payload_ext.data);
                // have to retuen if succesfully appended

                // if (written < 0)
                // {
                //     ESP_LOGE(TAG_Telnet, "Error occurred during sending: errno %d (%s)", errno, strerror(errno));
                //     // elementr retreived has tobe added again in front or in back has ro be reviewd and taken in consideration
                //     // two elements are lost
                //     // https://www.freertos.org/xQueueSendToFront.html
                //     break;
                // }
            }
            else
            {
                ESP_LOGE(TAG_Telnet, "file Queue may be void");
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