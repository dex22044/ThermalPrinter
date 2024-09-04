#pragma once

#include <cstdint>
#include "logger.h"

namespace TPDriver {

class TP_Image {
public:
    int height, bpp;
    uint8_t* data = nullptr;

    void allocate();
    int loadMonochromeFromFile(char* path, int bpp);
};

};
