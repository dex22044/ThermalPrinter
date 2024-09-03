#pragma once

#include <cstdint>
#include "logger.h"

namespace TPDriver {

class TP_Image {
public:
    int height;
    uint8_t* data = nullptr;

    void allocate();
    int loadMonochromeFromFile(char* path);
};

};
