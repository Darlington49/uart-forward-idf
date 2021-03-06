

// TODO : Sd Card
// TODO : Web Server
// TODO : Modify Uart (baude rate, parity,...) Trought SERVER
// TODO : Accss Sd through web server
// TODO : File manipulation Sd through web server

#include "main.h"

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);
    // esp_log_level_set(tag, ESP_LOG_INFO);

    ESP_ERROR_CHECK(nvs_flash_init());

    // Uart Queue Creation;
    message_received_queue = xQueueCreate(10, sizeof(payload_ext_t));
    message_tcp_queue = xQueueCreate(10, sizeof(payload_ext_t));
    message_file_queue = xQueueCreate(10, sizeof(payload_ext_t));

    sdcard_init();

    // Uart init
    uart_init();

    // Uart Task creation
    xTaskCreate(uart_event_task, "uart_event_task", 4096, NULL, 12, NULL);
    wifi_init();

    // Mqtt init
    init_MQTT();

    ESP_ERROR_CHECK(wifi_connect_sta("AndroidAP9CB9", "00000000", 10000));
    xTaskCreate(tcp_server_task, "tcp_server", 4096, (void *)AF_INET, 5, NULL);

    ESP_ERROR_CHECK(start_file_server(MOUNT_POINT));
}
