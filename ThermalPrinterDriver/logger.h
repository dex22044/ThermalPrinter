#pragma once

#include <cstdio>
#include <time.h>
#include <string.h>
#include <stdarg.h>

namespace TPDriver {

class Logger {
public:
    static void logInfo(const char* fmt, ...);
    static void logWarning(const char* fmt, ...);
    static void logError(const char* fmt, ...);
};

};
