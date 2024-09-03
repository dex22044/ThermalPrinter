#include "wifi_proto.h"

void TCPPrintServer::startServer() {
    Logger::logMsg(1, "Starting TCP server");
    serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Logger::fatalAssert(serverSock >= 0, "serversock create failed");
    sockaddr_in bindAddr = {0};
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_addr.s_addr = INADDR_ANY;
    bindAddr.sin_port = htons(9000);
    Logger::fatalAssert(bind(serverSock, (sockaddr*)&bindAddr, sizeof(sockaddr_in)) == 0, "bind failed");
    Logger::fatalAssert(listen(serverSock, 100000) == 0, "listen failed");

    int flags = fcntl(serverSock, F_GETFL, 0);
    Logger::fatalAssert(flags != -1, "fcntl get failed");
    flags = flags | O_NONBLOCK;
    Logger::fatalAssert(fcntl(serverSock, F_SETFL, flags) == 0, "fcntl set nonblock failed");

    recvBuf = (uint8_t*)malloc(4096);
    sendBuf = (uint8_t*)malloc(4096);
    queueData = (uint8_t*)malloc(queueSizeA + 1);
    // txQueueData = (uint8_t*)malloc(queueSizeA + 1);

    Logger::logMsg(1, "TCP server started");
}

void TCPPrintServer::pollClients() {
    int rem = accept(serverSock, nullptr, nullptr);
    if(rem < 0) return;
    remoteSock = rem;

    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    int nodelay = 1;
    Logger::fatalAssert(setsockopt(remoteSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) == 0, "set rsock read timeout failed");
    Logger::fatalAssert(setsockopt(remoteSock, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(int)) == 0, "set rsock nodelay failed");
}

void TCPPrintServer::pollPackets() {
    if(remoteSock == -1) return;
    int rd = recv(remoteSock, recvBuf, sizeof(recvBuf), 0);
    for(int i = 0; i < rd; i++) queueData[(queueWritePtr++) & queueSizeA] = recvBuf[i];
    // int txLen = txQueueAvailable();
    // if(txLen == 0) return;
    // if(txLen > 4096) txLen = 4096;
    // for(int i = 0; i < txLen; i++) sendBuf[i] = txQueueData[(txQueueReadPtr++) & queueSizeA];
    // send(remoteSock, sendBuf, txLen, 0);
}

uint16_t TCPPrintServer::queueAvailable() {
    return queueWritePtr - queueReadPtr;
}

// uint16_t TCPPrintServer::txQueueAvailable() {
//     return txQueueWritePtr - txQueueReadPtr;
// }

int TCPPrintServer::receivePacket(uint8_t* output) {
    if(queueAvailable() < pktSize) return -1;
    for(int i = 0; i < pktSize; i++) output[i] = queueData[(queueReadPtr++) & queueSizeA];
    return 0;
}

int TCPPrintServer::sendPacket(uint8_t* pkt) {
    if(remoteSock == -1) return -1;
    if(send(remoteSock, pkt, pktSize, 0) < pktSize) return -1;
    // for(int i = 0; i < pktSize; i++) txQueueData[(txQueueWritePtr++) & queueSizeA] = pkt[i];
    return 0;
}
