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
    dev->setParamUInt16(0x02, 1700);
    dev->setParamUInt16(0x03, 1600);
    dev->setParamUInt16(0x04, 2);
    dev->setEnableMotor(true);
    dev->skipLines(1, 120);
    dev->setEnableMotor(false);
    TP_Image img;
    if(img.loadMonochromeFromFile(argv[1], 2) == 0) {
        dev->uploadImage(img.data, img.height * 2, true);
        Logger::logInfo("Uploaded image");
        dev->setEnableMotor(true);
        dev->printBuffer();
        dev->skipLines(0, 200);
        dev->setEnableMotor(false);
        Logger::logInfo("Printed image");
    }
    dev->close();
}
