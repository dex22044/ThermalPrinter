#include "tp_device.h"

namespace TPDriver {

int TP_Device::open(char* device, int baudrate) {
    if(serial_fd >= 0) this->close();
    Logger::logInfo("Trying to open %s at %db/s", device, baudrate);
    serial_fd = ::open(device, O_RDWR);
    if(serial_fd < 0) {
        Logger::logError("Error opening %s: %d (%s)", device, errno, strerror(errno));
        return errno;
    }
    
    termios tty;
    if(tcgetattr(serial_fd, &tty) != 0) {
        Logger::logError("Error tcgetattr on %s: %d (%s)", device, errno, strerror(errno));
        return errno;
    }

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD;
    tty.c_cflag |= CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    cfsetispeed(&tty, baudrate);
    cfsetospeed(&tty, baudrate);

    if(tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
        Logger::logError("Error tcsetattr on %s: %d (%s)", device, errno, strerror(errno));
        return errno;
    }

    Logger::logInfo("Opened %s on %db/s", device, baudrate);
    return 0;
}

int TP_Device::openTCP(char* ip, uint16_t port) {
    if(serial_fd >= 0) this->close();
    Logger::logInfo("Trying to connect to %s:%d", ip, (int)port);
    serial_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serial_fd < 0) {
        Logger::logError("Error connecting to %s:%d (socket create failed)", ip, (int)port);
        return -1;
    }

    sockaddr_in saConn = {0};
    saConn.sin_family = AF_INET;
    saConn.sin_port = htons(port);
    saConn.sin_addr.s_addr = inet_addr(ip);
    int ret = 0;
    if((ret = connect(serial_fd, (sockaddr*)&saConn, sizeof(sockaddr_in))) != 0) {
        Logger::logError("Error connecting to %s:%d (connect failed, %d %s)", ip, (int)port, ret, strerror(ret));
        return -1;
    }

    int flags = fcntl(serial_fd, F_GETFL, 0);
    flags = flags | O_NONBLOCK;
    fcntl(serial_fd, F_SETFL, flags);
    int nodelay = 1;
    setsockopt(serial_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(int));

    Logger::logInfo("Connected to %s:%d", ip, (int)port);
    return 0;
}

void TP_Device::close() {
    Logger::logInfo("Closing port...");
    if(serial_fd < 0) {
        Logger::logError("Failed to close port (port is not open)");
        return;
    }
    ::close(serial_fd);
    serial_fd = -1;
    Logger::logInfo("Closed port");
}

void TP_Device::pollInput() {
    int rd = recv(serial_fd, inputBuffer, sizeof(inputBuffer), 0);
    for(int i = 0; i < rd; i++) inputQueue[queueWritePtr++] = inputBuffer[i];
}

uint16_t TP_Device::queueAvailable() {
    return queueWritePtr - queueReadPtr;
}

void TP_Device::sendPacket(uint8_t pkt[64]) {
    write(serial_fd, pkt, 64);
}

int TP_Device::receivePacket(uint8_t* pkt) {
    if(queueAvailable() < 64) return -1;
    for(int i = 0; i < 64; i++) pkt[i] = inputQueue[queueReadPtr++];
    return 0;
}

void TP_Device::waitForReply(int cmdId) {
    uint8_t pkt[64];
    int iters = 0;
    while(iters < 5000) {
        pollInput();
        iters++;
        if(receivePacket(pkt) == 0) {
            if(pkt[0] == cmdId) return;
        }
        usleep(2000);
    }
    Logger::logError("Took too long to receive reply cmd %02x", cmdId);
}

void TP_Device::ping() {
    uint8_t pkt[64];
    pkt[0] = 0x01;
    pkt[1] = 0x53;
    sendPacket(pkt);
    waitForReply(0x71);
}

void TP_Device::setParamUInt16(uint8_t param, uint16_t value) {
    uint8_t pkt[64];
    pkt[0] = 0x02;
    pkt[1] = param;
    pkt[2] = (value >> 8);
    pkt[3] = (value & 0xFF);
    sendPacket(pkt);
    waitForReply(0x72);
}

void TP_Device::setEnableMotor(bool enable) {
    uint8_t pkt[64];
    pkt[0] = 0x06;
    pkt[1] = (enable ? 0x01 : 0x00);
    sendPacket(pkt);
    waitForReply(0x76);
}

void TP_Device::skipLines(bool direction, uint16_t lines) {
    uint8_t pkt[64];
    pkt[0] = 0x05;
    pkt[1] = (direction ? 0x01 : 0x00);
    pkt[2] = lines >> 8;
    pkt[3] = lines & 0xFF;
    sendPacket(pkt);
    waitForReply(0x75);
}

void TP_Device::printImage(uint8_t* img, int height) {
    setEnableMotor(true);
    usleep(100000);
    int txLines = 0;
    int rxLines = 0;
    int cntWait = 0;
    uint8_t inputPkt[64], outputPkt[64];
    while(rxLines < height) {
        pollInput();
        while(receivePacket(inputPkt) == 0) {
            if(inputPkt[0] == 0x74) rxLines++;
        }
        if(txLines - rxLines > maximumBufferedLines) {
            cntWait++;
            if(cntWait > 10000) {
                Logger::logError("Failed to print image (didn't receive reply too long, rx:%d tx:%d)", rxLines, txLines);
                return;
            }
            usleep(500);
            continue;
        }
        // Logger::logInfo("Last wait: %d", cntWait);
        cntWait = 0;
        if(txLines == height) continue;
        outputPkt[0] = 0x04;
        outputPkt[1] = 0x01;
        memcpy(&outputPkt[2], &img[txLines * 54], 54);
        sendPacket(outputPkt);
        usleep(500);
        txLines++;
    }
    usleep(10000);
    setEnableMotor(false);
}

void TP_Device::uploadImage(uint8_t* img, int height, bool clearBuffer) {
    uint8_t inputPkt[64], outputPkt[64];
    if(clearBuffer) {
        outputPkt[0] = 0x07;
        outputPkt[1] = 0x01;
        sendPacket(outputPkt);
        waitForReply(0x77);
    }
    int txLines = 0;
    int rxLines = 0;
    int cntWait = 0;
    while(rxLines < height) {
        pollInput();
        while(receivePacket(inputPkt) == 0) {
            if(inputPkt[0] == 0x77) rxLines++;
        }
        if(txLines - rxLines > maximumBufferedLines) {
            cntWait++;
            if(cntWait > 10000) {
                Logger::logError("Failed to upload image (didn't receive reply too long, rx:%d tx:%d)", rxLines, txLines);
                return;
            }
            usleep(500);
            continue;
        }
        // Logger::logInfo("Last wait: %d", cntWait);
        cntWait = 0;
        if(txLines == height) continue;
        outputPkt[0] = 0x07;
        outputPkt[1] = 0x02;
        memcpy(&outputPkt[2], &img[txLines * 54], 54);
        sendPacket(outputPkt);
        // usleep(800);
        txLines++;
    }
}

void TP_Device::printBuffer() {
    uint8_t outputPkt[64];
    outputPkt[0] = 0x07;
    outputPkt[1] = 0x03;
    sendPacket(outputPkt);
    waitForReply(0x77);
}


};
