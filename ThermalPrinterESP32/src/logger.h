#pragma once

#include <Arduino.h>
#include <cstdarg>
#include "peripherals.h"

class Logger {
public:
    static void logMsg(int level, const char* format, ...);
    static void fatalAssert(bool x, const char* msg = "no message");
};
