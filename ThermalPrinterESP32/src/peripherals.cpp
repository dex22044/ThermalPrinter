#include "peripherals.h"

#define NOP asm volatile("nop;")

void Peripherals::initPeripherals() {
    if(INTERNAL_LED != -1) pinMode(INTERNAL_LED, OUTPUT);

    pinMode(MOT_DIR, OUTPUT);
    pinMode(MOT_STP, OUTPUT);
    pinMode(MOT_SLP, OUTPUT);
    pinMode(MOT_RST, OUTPUT);
    pinMode(MOT_EN, OUTPUT);



    digitalWrite(PRNT_STB1, 1);
    digitalWrite(PRNT_STB2, 1);
    digitalWrite(PRNT_STB3, 1);
    if(PRNT_STB4 != -1) digitalWrite(PRNT_STB4, 1);
    if(PRNT_STB5 != -1) digitalWrite(PRNT_STB5, 1);
    if(PRNT_STB6 != -1) digitalWrite(PRNT_STB6, 1);
    digitalWrite(PRNT_DAT, 1);
    digitalWrite(PRNT_LAT, 1);
    digitalWrite(PRNT_CLK, 0);

    pinMode(PRNT_DAT, OUTPUT);
    pinMode(PRNT_LAT, OUTPUT);
    pinMode(PRNT_CLK, OUTPUT);
    pinMode(PRNT_STB1, OUTPUT);
    pinMode(PRNT_STB2, OUTPUT);
    pinMode(PRNT_STB3, OUTPUT);
    if(PRNT_STB4 != -1) pinMode(PRNT_STB4, OUTPUT);
    if(PRNT_STB5 != -1) pinMode(PRNT_STB5, OUTPUT);
    if(PRNT_STB6 != -1) pinMode(PRNT_STB6, OUTPUT);

    digitalWrite(PRNT_STB1, 1);
    digitalWrite(PRNT_STB2, 1);
    digitalWrite(PRNT_STB3, 1);
    if(PRNT_STB4 != -1) digitalWrite(PRNT_STB4, 1);
    if(PRNT_STB5 != -1) digitalWrite(PRNT_STB5, 1);
    if(PRNT_STB6 != -1) digitalWrite(PRNT_STB6, 1);
    digitalWrite(PRNT_DAT, 1);
    digitalWrite(PRNT_LAT, 1);
    digitalWrite(PRNT_CLK, 0);

    pinMode(PRNT_SENS, INPUT);
}

void Peripherals::motorStep() {
    digitalWrite(MOT_STP, 1);
    NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
    digitalWrite(MOT_STP, 0);
    NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
}

void Peripherals::motorSetEnabled(int en) {
    digitalWrite(MOT_EN, en ? 0 : 1);
    digitalWrite(MOT_SLP, (!en) ? 0 : 1);
    delay(10);
}

void Peripherals::motorReset() {
    digitalWrite(MOT_RST, 0);
    delay(100);
    digitalWrite(MOT_RST, 1);
    delay(100);
}

void Peripherals::motorSetDirection(int dir) {
    digitalWrite(MOT_DIR, !dir);
}

void Peripherals::printerBurnNahuyLine(uint8_t lineData[54], uint16_t burnTime) {
    for(int i = 0; i < 54; i++) {
        uint8_t cval = lineData[i];
        for(int j = 0; j < 8; j++) {
            if((cval >> j) & 1) *(volatile uint32_t *)(GPIO_OUT_REG) |= (1u << PRNT_DAT);
            else *(volatile uint32_t *)(GPIO_OUT_REG) &= ~(1u << PRNT_DAT);
            *(volatile uint32_t *)(GPIO_OUT_REG) |= (1u << PRNT_CLK);
            *(volatile uint32_t *)(GPIO_OUT_REG) &= ~(1u << PRNT_CLK);
        }
    }

    digitalWrite(PRNT_LAT, 0);
    NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
    digitalWrite(PRNT_LAT, 1);
    NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;

    digitalWrite(PRNT_STB1, 0);
    if(PRNT_STB4 != -1) digitalWrite(PRNT_STB4, 0);
    delayMicroseconds(burnTime);
    digitalWrite(PRNT_STB1, 1);
    if(PRNT_STB4 != -1) digitalWrite(PRNT_STB4, 1);

    digitalWrite(PRNT_STB2, 0);
    if(PRNT_STB5 != -1) digitalWrite(PRNT_STB5, 0);
    delayMicroseconds(burnTime);
    digitalWrite(PRNT_STB2, 1);
    if(PRNT_STB5 != -1) digitalWrite(PRNT_STB5, 1);

    digitalWrite(PRNT_STB3, 0);
    if(PRNT_STB6 != -1) digitalWrite(PRNT_STB6, 0);
    delayMicroseconds(burnTime);
    digitalWrite(PRNT_STB3, 1);
    if(PRNT_STB6 != -1) digitalWrite(PRNT_STB6, 1);
}

void Peripherals::printerBurnNahuyLineGrayscale(uint8_t* lineData, uint16_t burnTimes[8], uint8_t bpp) {
    uint8_t outData[54];
    for(int b = 0; b < bpp; b++) {
        memset(outData, 0, sizeof(outData));
        for(int i = 0; i < 432; i++) {
            int cidx = i * bpp + b;
            if((lineData[cidx >> 3] >> (cidx & 7)) & 1) outData[i >> 3] |= 1 << (i & 7);
        }
        for(int i = 0; i < 54; i++) {
            uint8_t cval = outData[i];
            for(int j = 0; j < 8; j++) {
                if((cval >> j) & 1) *(volatile uint32_t *)(GPIO_OUT_REG) |= (1u << PRNT_DAT);
                else *(volatile uint32_t *)(GPIO_OUT_REG) &= ~(1u << PRNT_DAT);
                *(volatile uint32_t *)(GPIO_OUT_REG) |= (1u << PRNT_CLK);
                *(volatile uint32_t *)(GPIO_OUT_REG) &= ~(1u << PRNT_CLK);
            }
        }

        digitalWrite(PRNT_LAT, 0);
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        digitalWrite(PRNT_LAT, 1);
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;
        NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;

        digitalWrite(PRNT_STB1, 0);
        if(PRNT_STB4 != -1) digitalWrite(PRNT_STB4, 0);
        delayMicroseconds(burnTimes[b]);
        digitalWrite(PRNT_STB1, 1);
        if(PRNT_STB4 != -1) digitalWrite(PRNT_STB4, 1);
        delayMicroseconds(100);

        digitalWrite(PRNT_STB2, 0);
        if(PRNT_STB5 != -1) digitalWrite(PRNT_STB5, 0);
        delayMicroseconds(burnTimes[b]);
        digitalWrite(PRNT_STB2, 1);
        if(PRNT_STB5 != -1) digitalWrite(PRNT_STB5, 1);
        delayMicroseconds(100);

        digitalWrite(PRNT_STB3, 0);
        if(PRNT_STB6 != -1) digitalWrite(PRNT_STB6, 0);
        delayMicroseconds(burnTimes[b]);
        digitalWrite(PRNT_STB3, 1);
        if(PRNT_STB6 != -1) digitalWrite(PRNT_STB6, 1);
    }
}
