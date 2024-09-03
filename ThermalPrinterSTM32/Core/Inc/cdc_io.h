#pragma once

#define CDC_IO_RECEIVE_QUEUE_SIZE 8192
#define CDC_IO_RECEIVE_QUEUE_SIZE_A (CDC_IO_RECEIVE_QUEUE_SIZE - 1)

#include <stdint.h>

// #include "usbd_cdc_if.h"
#include "stdint.h"

void cdc_init();
void cdc_dataReceiveCallback(uint8_t* data, uint32_t len);
unsigned int cdc_queueAvailable();
void cdc_queueRead(uint8_t* data, uint16_t len);
