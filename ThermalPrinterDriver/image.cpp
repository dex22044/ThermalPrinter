#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace TPDriver {

void TP_Image::allocate() {
    if(data != nullptr) free(data);
    data = (uint8_t*)calloc(height, 54 * bpp);
}

int TP_Image::loadMonochromeFromFile(char* path, int bpp) {
    Logger::logInfo("Loading grayscale (bpp=%d) image %s", bpp, path);
    int width, height, channels;
    uint8_t* imgData = stbi_load(path, &width, &height, &channels, 1);
    if(imgData == nullptr) {
        Logger::logError("Failed to load grayscale image (file not found)");
        return -1;
    }
    if(channels != 1) {
        Logger::logError("Failed to load grayscale image (not 1 channel)");
        return -1;
    }
    if(width != 432) {
        Logger::logError("Failed to load grayscale image (width is not 432)");
        return -1;
    }

    this->height = height;
    this->bpp = bpp;
    this->allocate();

    for(int row = 0; row < height; row++) {
        for(int col = 0; col < 432; col++) {
            int dataIdx = row * 432 + col;
            int val = ~imgData[dataIdx];
            val >>= 8 - bpp;
            dataIdx *= bpp;
            for(int b = 0; b < bpp; b++) {
                if((val >> b) & 1) this->data[dataIdx >> 3] ^= (1 << (dataIdx & 7));
                dataIdx++;
            }
        }
    }
    Logger::logInfo("Loaded grayscale image %s", path);
    return 0;
}

};
