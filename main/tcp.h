#ifndef tcp_H_ /* Include guard */
#define tcp_H_
#include <stdio.h>
#include "connect.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"



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

extern QueueHandle_t message_received_queue;

void tcp_server_task(void *pvParameters);
void do_retransmit(const int sock);
#endif // serial_H_