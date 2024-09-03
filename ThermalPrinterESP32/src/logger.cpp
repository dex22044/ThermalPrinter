#include "logger.h"

void Logger::logMsg(int level, const char* format, ...) {
    char msgFormatBuffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(msgFormatBuffer, sizeof(msgFormatBuffer), format, args);
    va_end(args);

    unsigned long curTime = millis();

    char logLetter = '-';
    if(level == 1) logLetter = 'I';
    if(level == 2) logLetter = 'W';
    if(level == 3) logLetter = 'E';
    if(level == 4) logLetter = 'F';

    Serial.printf("[%6lu.%03lu][%c%c] %s\r\n", curTime / 1000, curTime % 1000, logLetter, logLetter, msgFormatBuffer);
    Serial.flush();
}

void Logger::fatalAssert(bool x, const char* msg) {
    if(x) return;
    Logger::logMsg(4, "Fatal assert triggered (%s)!", msg);
    while(true) {
        if(INTERNAL_LED != -1) digitalWrite(INTERNAL_LED, 1);
        delay(100);
        if(INTERNAL_LED != -1) digitalWrite(INTERNAL_LED, 0);
        delay(100);
    }
}
