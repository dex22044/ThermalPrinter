#pragma once

#include "main.h"

void blinkLed(int interval, int count);

void motorStep();
void motorSetDirection(int dir);
void motorSetEnabled(int en);
void motorReset();

void printerBurnNahuyLine(uint8_t lineData[54], uint16_t burnTime);
