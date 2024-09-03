#include "peripherals.h"

#define NOP asm volatile("nop;")

void blinkLed(int interval, int count) {
    while(count > 0) {
        count--;
        HAL_GPIO_WritePin(INTERNAL_LED_GPIO_Port, INTERNAL_LED_Pin, GPIO_PIN_SET);
        HAL_Delay(interval);
        HAL_GPIO_WritePin(INTERNAL_LED_GPIO_Port, INTERNAL_LED_Pin, GPIO_PIN_RESET);
        HAL_Delay(interval);
    }
}

void motorStep() {
    HAL_GPIO_WritePin(MOT_STP_GPIO_Port, MOT_STP_Pin, GPIO_PIN_SET);
    NOP;NOP;NOP;NOP;
    HAL_GPIO_WritePin(MOT_STP_GPIO_Port, MOT_STP_Pin, GPIO_PIN_RESET);
    NOP;NOP;NOP;NOP;
}

void motorSetEnabled(int en) {
    HAL_GPIO_WritePin(MOT_EN_GPIO_Port, MOT_EN_Pin, en ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOT_SLP_GPIO_Port, MOT_SLP_Pin, (!en) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_Delay(10);
}

void motorReset() {
    HAL_GPIO_WritePin(MOT_RST_GPIO_Port, MOT_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(MOT_RST_GPIO_Port, MOT_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
}

void motorSetDirection(int dir) {
    HAL_GPIO_WritePin(MOT_DIR_GPIO_Port, MOT_DIR_Pin, (GPIO_PinState)(!dir));
}

void printerBurnNahuyLine(uint8_t lineData[54], uint16_t burnTime) {
    HAL_GPIO_WritePin(PRNT_STB1_GPIO_Port, PRNT_STB1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PRNT_STB2_GPIO_Port, PRNT_STB2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PRNT_STB3_GPIO_Port, PRNT_STB3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PRNT_DAT_GPIO_Port, PRNT_DAT_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PRNT_LAT_GPIO_Port, PRNT_LAT_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PRNT_CLK_GPIO_Port, PRNT_CLK_Pin, GPIO_PIN_RESET);

    uint8_t cval = 0;
    for(int i = 0; i < 432; i++) {
        if((i & 7) == 0) cval = lineData[i >> 3];
        HAL_GPIO_WritePin(PRNT_DAT_GPIO_Port, PRNT_DAT_Pin, (cval & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(PRNT_CLK_GPIO_Port, PRNT_CLK_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(PRNT_CLK_GPIO_Port, PRNT_CLK_Pin, GPIO_PIN_RESET);
        cval >>= 1;
    }

    HAL_GPIO_WritePin(PRNT_LAT_GPIO_Port, PRNT_LAT_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PRNT_LAT_GPIO_Port, PRNT_LAT_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(PRNT_STB1_GPIO_Port, PRNT_STB1_Pin, GPIO_PIN_RESET);
    delay_us(burnTime);
    HAL_GPIO_WritePin(PRNT_STB1_GPIO_Port, PRNT_STB1_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(PRNT_STB2_GPIO_Port, PRNT_STB2_Pin, GPIO_PIN_RESET);
    delay_us(burnTime);
    HAL_GPIO_WritePin(PRNT_STB2_GPIO_Port, PRNT_STB2_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(PRNT_STB3_GPIO_Port, PRNT_STB3_Pin, GPIO_PIN_RESET);
    delay_us(burnTime);
    HAL_GPIO_WritePin(PRNT_STB3_GPIO_Port, PRNT_STB3_Pin, GPIO_PIN_SET);
}
