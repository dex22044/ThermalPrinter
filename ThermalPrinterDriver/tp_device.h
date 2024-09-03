#pragma once

#include <cstdio>
#include <cstdint>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include "logger.h"

namespace TPDriver {

class TP_Device {
private:
    int serial_fd = -1;

    uint16_t queueReadPtr = 0, queueWritePtr = 0;
    uint8_t inputQueue[65536];
    uint8_t inputBuffer[1024];


public:
    int maximumBufferedLines = 448;

    int open(char* device, int baudrate = 500000);
    int openTCP(char* ip, uint16_t port);
    void close();

    void pollInput();
    uint16_t queueAvailable();
    void sendPacket(uint8_t pkt[64]);
    int receivePacket(uint8_t* pkt);
    void waitForReply(int cmdId);

    void ping();
    void setParamUInt16(uint8_t param, uint16_t value);
    void setEnableMotor(bool enable);
    void skipLines(bool direction, uint16_t lines);

    void printImage(uint8_t* img, int height);
    void uploadImage(uint8_t* img, int height, bool clearBuffer = true);
    void printBuffer();
};

};
