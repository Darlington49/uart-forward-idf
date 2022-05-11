#ifndef serial_H_ /* Include guard */
#define serial_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "sdcard.h"

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
typedef struct payload_ext_t
{
    char data[BUF_SIZE];
    int count;
} payload_ext_t;

#define EX_UART_NUM UART_NUM_0
#define PATTERN_CHR_NUM (3) /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

QueueHandle_t uart0_queue;
QueueHandle_t message_received_queue;

void uart_event_task(void *pvParameters);
void uart_init();
#endif // serial_H_