#ifndef sdcard_H_ /* Include guard */
#define sdcard_H_

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#define MOUNT_POINT "/sdcard"
// Pin mapping
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK 14
#define PIN_NUM_CS 13

#define SPI_DMA_CHAN 1

#define FORMAT_IF_MOUNT_FAILED 0

void sdcard_init();

void test();
#endif