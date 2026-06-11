#include "hardware.h"
#include "stm32f1xx_hal.h"

void HW_Init(void) {
    // 1. Enable clocks for Port A (LED) and Port B (Button)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 2. Configure Green LED on PA5
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. Configure All Buttons (Menu on PB0, Select on PB1, Coin on PB2)
    // PRO TIP: We configure all 3 pins at the same time to save memory and execution time
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

bool HW_IsMenuButtonPressed(void) {
    // Because of PULLUP, the pin goes to 0 (RESET) when connected to GND (pressed)
    return (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET);
}

void HW_SetIdleLed(bool state) {
    if (state) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    }
}

bool HW_IsSelectButtonPressed(void) {
    return (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET);
}

bool HW_IsCoinButtonPressed(void) {
    return (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET);
}