#include "cdc_io.h"

static struct CDC_IO_Shared {
    unsigned int cdc_receiveQueueReadPtr;
    unsigned int cdc_receiveQueueWritePtr;
    uint8_t cdc_receiveQueueData[CDC_IO_RECEIVE_QUEUE_SIZE];
} CDC_IO_Shared;

void cdc_init() {
    CDC_IO_Shared.cdc_receiveQueueReadPtr = 0;
    CDC_IO_Shared.cdc_receiveQueueWritePtr = 0;
}

void cdc_dataReceiveCallback(uint8_t* data, uint32_t len) {
    for(int i = 0; i < len; i++) CDC_IO_Shared.cdc_receiveQueueData[(CDC_IO_Shared.cdc_receiveQueueWritePtr++) & CDC_IO_RECEIVE_QUEUE_SIZE_A] = data[i];
}

unsigned int cdc_queueAvailable() {
    return CDC_IO_Shared.cdc_receiveQueueWritePtr - CDC_IO_Shared.cdc_receiveQueueReadPtr;
}

void cdc_queueRead(uint8_t* data, uint16_t len) {
    for(int i = 0; i < len; i++) data[i] = CDC_IO_Shared.cdc_receiveQueueData[(CDC_IO_Shared.cdc_receiveQueueReadPtr++) & CDC_IO_RECEIVE_QUEUE_SIZE_A];
}
