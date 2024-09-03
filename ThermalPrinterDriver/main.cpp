#include <cstdio>
#include "logger.h"
#include "tp_device.h"
#include "image.h"

using namespace TPDriver;

int main(int argc, char** argv) {
    if(argc < 2) {
        Logger::logInfo("%s <image file>", argv[0]);
        return 0;
    }
    TP_Device* dev = new TP_Device();
    // dev->open("/dev/ttyACM0");
    dev->openTCP("192.168.1.211", 9000);
    dev->ping();
    dev->setParamUInt16(0x01, 0);
    dev->setParamUInt16(0x02, 1800);
    dev->setParamUInt16(0x03, 1500);
    TP_Image img;
    if(img.loadMonochromeFromFile(argv[1]) == 0) {
        dev->uploadImage(img.data, img.height, true);
        dev->setEnableMotor(true);
        dev->printBuffer();
        dev->skipLines(0, 200);
        dev->setEnableMotor(false);
        Logger::logInfo("Printed image");
    }
    dev->close();
}
