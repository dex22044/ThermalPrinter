#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace TPDriver {

void TP_Image::allocate() {
    if(data != nullptr) free(data);
    data = (uint8_t*)calloc(height, 54);
}

int TP_Image::loadMonochromeFromFile(char* path) {
    Logger::logInfo("Loading monochrome image %s", path);
    int width, height, channels;
    uint8_t* imgData = stbi_load(path, &width, &height, &channels, 1);
    if(imgData == nullptr) {
        Logger::logError("Failed to load monochrome image (file not found)");
        return -1;
    }
    if(channels != 1) {
        Logger::logError("Failed to load monochrome image (not 1 channel)");
        return -1;
    }
    if(width != 432) {
        Logger::logError("Failed to load monochrome image (width is not 432)");
        return -1;
    }

    this->height = height;
    this->allocate();

    for(int row = 0; row < height; row++) {
        for(int col = 0; col < 432; col++) {
            if(imgData[row * 432 + col] < 128) this->data[row * 54 + (col >> 3)] ^= (1 << (col & 7));
        }
    }
    Logger::logInfo("Loaded monochrome image %s", path);
    return 0;
}

};
