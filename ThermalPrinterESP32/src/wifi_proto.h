#pragma once

#include <sys/socket.h>
#include <cstdint>

#include "logger.h"

class TCPPrintServer {
private:
    int serverSock = -1;
    int remoteSock = -1;
    const int pktSize = 64;
    uint8_t* recvBuf;
    uint8_t* sendBuf;
    uint8_t* queueData;
    // uint8_t* txQueueData;
    const uint16_t queueSizeA = 32767;
    uint16_t queueReadPtr = 0, queueWritePtr = 0;
    // uint16_t txQueueReadPtr = 0, txQueueWritePtr = 0;

public:
    void startServer();
    void pollClients();
    void pollPackets();
    uint16_t queueAvailable();
    uint16_t txQueueAvailable();
    int receivePacket(uint8_t* output);
    int sendPacket(uint8_t* pkt);
};
