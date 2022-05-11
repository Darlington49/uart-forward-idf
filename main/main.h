#ifndef main_H_ /* Include guard */
#define main_H_
#include <stdio.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"

// #include "telnet.h"

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
#include "mqtt.h"
#include "sdcard.h"

#endif