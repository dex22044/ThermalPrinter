#include <Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>

#include "logger.h"
#include "peripherals.h"
#include "wifi_proto.h"

TCPPrintServer srv;

uint32_t chipId = 0;
char chipIDHex[9] = "";
TaskHandle_t printTask;

uint8_t* currImageBuffer;
const int imageBufferRowLimit = 1024;
const int imageBufferSizeLimit = imageBufferRowLimit * 54;
int currentImageBufferSize = 0;
// const int rowsInUdpPacket = 4;
// uint8_t imageAvailBuffer[imageBufferRowLimit] = {0};

bool modeAP = false;
char wifiSSID[32] = "le printer";
char wifiPASS[32] = "ecbbb348-b98a-4379";

void printTaskHandler(void* userdata);
void processPacket();
void printImageFromBuffer();

void readChipID() {
    for(int i = 0; i < 17; i = i + 8) {
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    snprintf(chipIDHex, sizeof(chipIDHex), "%08X", chipId);
    snprintf(wifiSSID, sizeof(wifiSSID), "TP_%08X", chipId);
}

void setup() {
    Serial.begin(115200);
    Logger::logMsg(1, "Initializing printer");

    Peripherals::initPeripherals();
    Peripherals::motorSetEnabled(false);
    Peripherals::motorReset();
    Logger::logMsg(1, "Initialized peripherals");

    if(!modeAP) {
        Logger::logMsg(1, "Connecting to WiFi \"%s\"", wifiSSID);
        WiFi.begin(wifiSSID, wifiPASS);
        for(int i = 0; i < 20; i++) {
            if(WiFi.status() == WL_CONNECTED) break;
            Logger::logMsg(1, "...");
            delay(500);
        }
        if(WiFi.status() == WL_CONNECTED) {
            Logger::logMsg(1, "Connected");
            delay(1000);
            Logger::logMsg(1, "Initialized WiFI, IP: %s", WiFi.localIP().toString().c_str());
        } else {
            Logger::logMsg(1, "Failed to connect, falling back to AP");
            modeAP = true;
        }
    }
    if(modeAP) {
        Logger::logMsg(1, "WiFi AP SSID: %s", wifiSSID);
        Logger::fatalAssert(WiFi.softAP(wifiSSID, wifiPASS), "wifi ap init failed");
        delay(1000);
        Logger::logMsg(1, "Initialized WiFI, IP: %s", WiFi.softAPIP().toString().c_str());
    }

    srv.startServer();
    xTaskCreatePinnedToCore(printTaskHandler, "Print task", 4096, nullptr, 0, &printTask, 1 - xPortGetCoreID());
    currImageBuffer = (uint8_t*)malloc(imageBufferSizeLimit);
    Logger::fatalAssert(currImageBuffer != nullptr, "image buffer allocation failed");
    Logger::logMsg(1, "Done");
}

uint16_t minimalRowDelay = 300;
uint16_t motorStepDelay = 1700;
uint16_t rowBurnTime = 400;
uint8_t pktIn[64], pktOut[64];
uint8_t lineData[54];

void processPacket() {
    if(pktIn[0] == 0x01) { // Ping
        pktOut[0] = 0x71;
        pktOut[1] = pktIn[1];
        return;
    }

    if(pktIn[0] == 0x02) { // Set param
        if(pktIn[1] == 0x01) { // Minimal row delay
            minimalRowDelay = ((uint16_t)pktIn[2] << 8) | pktIn[3];
            pktOut[0] = 0x72;
            pktOut[1] = 0x01;
            return;
        }
        if(pktIn[1] == 0x02) { // Motor step delay
            motorStepDelay = ((uint16_t)pktIn[2] << 8) | pktIn[3];
            pktOut[0] = 0x72;
            pktOut[1] = 0x02;
            return;
        }
        if(pktIn[1] == 0x03) { // Row burn time
            rowBurnTime = ((uint16_t)pktIn[2] << 8) | pktIn[3];
            pktOut[0] = 0x72;
            pktOut[1] = 0x03;
            return;
        }
        return;
    }

    if(pktIn[0] == 0x03) { // Get param
        if(pktIn[1] == 0x01) { // Minimal row delay
            pktOut[0] = 0x73;
            pktOut[1] = 0x01;
            pktOut[2] = (minimalRowDelay >> 8);
            pktOut[3] = (minimalRowDelay & 0xFF);
            return;
        }
        if(pktIn[1] == 0x02) { // Motor step delay
            pktOut[0] = 0x73;
            pktOut[1] = 0x02;
            pktOut[2] = (motorStepDelay >> 8);
            pktOut[3] = (motorStepDelay & 0xFF);
            return;
        }
        if(pktIn[1] == 0x03) { // Row burn time
            pktOut[0] = 0x73;
            pktOut[1] = 0x03;
            pktOut[2] = (rowBurnTime >> 8);
            pktOut[3] = (rowBurnTime & 0xFF);
            return;
        }
        return;
    }

    if(pktIn[0] == 0x04) { // Print line
        uint8_t enableStep = pktIn[1];
        memcpy(lineData, &pktIn[2], 54);
        Peripherals::printerBurnNahuyLine(lineData, rowBurnTime);
        // Logger::logMsg(0, "Print line");
        if(enableStep) Peripherals::motorStep();
        delayMicroseconds(minimalRowDelay);
        pktOut[0] = 0x74;
        return;
    }

    if(pktIn[0] == 0x05) { // Motor step
        uint8_t direction = pktIn[1];
        uint16_t stepCount = ((uint16_t)pktIn[2] << 8) | pktIn[3];
        Logger::logMsg(0, "Skip %d lines in dir %d", (int)stepCount, (int)direction);
        Peripherals::motorSetDirection(direction);
        for(uint16_t i = 0; i < stepCount; i++) {
            Peripherals::motorStep();
            delayMicroseconds(motorStepDelay);
        }
        Peripherals::motorSetDirection(0);
        pktOut[0] = 0x75;
        return;
    }

    if(pktIn[0] == 0x06) { // Enable/disable motor
        uint8_t enable = pktIn[1];
        Peripherals::motorSetEnabled(enable);
        Logger::logMsg(0, "Motor enable: %d", (int)enable);
        pktOut[0] = 0x76;
        return;
    }

    if(pktIn[0] == 0x07) { // Autoprinting
        if(pktIn[1] == 0x01) { // Clear buffer
            // for(int i = 0; i < imageBufferRowLimit; i++) imageAvailBuffer[i] = 0;
            currentImageBufferSize = 0;
            pktOut[0] = 0x77;
            pktOut[1] = 0x01;
            Logger::logMsg(0, "Image buffer reset");
            return;
        }
        if(pktIn[1] == 0x02) { // Write line to buffer
            if(currentImageBufferSize < imageBufferRowLimit) {
                memcpy(&currImageBuffer[currentImageBufferSize * 54], &pktIn[2], 54);
                currentImageBufferSize++;
            }
            // while(currentImageBufferSize < imageBufferRowLimit && imageAvailBuffer[currentImageBufferSize]) currentImageBufferSize++;
            pktOut[0] = 0x77;
            pktOut[1] = 0x02;
            return;
        }
        if(pktIn[1] == 0x03) { // Print buffer
            printImageFromBuffer();
            pktOut[0] = 0x77;
            pktOut[1] = 0x03;
            return;
        }
    }
}

void printImageFromBuffer() {
    Peripherals::motorSetDirection(0);
    for(int row = 0; row < currentImageBufferSize; row++) {
        memcpy(lineData, &currImageBuffer[row * 54], 54);
        Peripherals::printerBurnNahuyLine(lineData, rowBurnTime);
        Peripherals::motorStep();
        delayMicroseconds(minimalRowDelay);
    }
}

void loop() {
    srv.pollClients();
    srv.pollPackets();
}

void printTaskHandler(void* userdata) {
    while(true) {
        if(srv.receivePacket(pktIn) == 0) {
            // Logger::logMsg(0, "Received packet");
            processPacket();
            if(srv.sendPacket(pktOut) != 0) {
                Logger::logMsg(3, "Send error");
            }
        }
    }
}
