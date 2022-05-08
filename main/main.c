#include <stdio.h>
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
#include "connect.h"

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
