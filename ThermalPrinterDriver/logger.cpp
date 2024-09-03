#include "logger.h"

namespace TPDriver {

void Logger::logInfo(const char* fmt, ...) {
    char msgFormatBuffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgFormatBuffer, sizeof(msgFormatBuffer), fmt, args);
    va_end(args);

    time_t timer;
    char timeBuffer[32];
    struct tm* tm_info;

    timer = time(nullptr);
    tm_info = localtime(&timer);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("\033[1;32m[%s][II] %s\033[0m\n", timeBuffer, msgFormatBuffer);
    fflush(stdout);
}

void Logger::logWarning(const char* fmt, ...) {
    char msgFormatBuffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgFormatBuffer, sizeof(msgFormatBuffer), fmt, args);
    va_end(args);

    time_t timer;
    char timeBuffer[32];
    struct tm* tm_info;

    timer = time(nullptr);
    tm_info = localtime(&timer);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("\033[1;33m[%s][WW] %s\033[0m\n", timeBuffer, msgFormatBuffer);
    fflush(stdout);
}

void Logger::logError(const char* fmt, ...) {
    char msgFormatBuffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgFormatBuffer, sizeof(msgFormatBuffer), fmt, args);
    va_end(args);

    time_t timer;
    char timeBuffer[32];
    struct tm* tm_info;

    timer = time(nullptr);
    tm_info = localtime(&timer);
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("\033[1;31m[%s][EE] %s\033[0m\n", timeBuffer, msgFormatBuffer);
    fflush(stdout);
}

};
