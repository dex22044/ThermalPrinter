#pragma once

#include <Arduino.h>

#define INTERNAL_LED -1

#define MOT_DIR 27
#define MOT_STP 26
#define MOT_SLP 25
#define MOT_RST 33
#define MOT_EN 32

#define PRNT_DAT 4
#define PRNT_LAT 19
#define PRNT_CLK 21
#define PRNT_STB1 18
#define PRNT_STB2 16
#define PRNT_STB3 17
#define PRNT_STB4 -1
#define PRNT_STB5 -1
#define PRNT_STB6 -1
#define PRNT_SENS 36

class Peripherals {
public:
    static void initPeripherals();
    static void motorStep();
    static void motorSetDirection(int dir);
    static void motorSetEnabled(int en);
    static void motorReset();

    static void printerBurnNahuyLine(uint8_t lineData[54], uint16_t burnTime);
};
