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

    // 3. Configure Menu Button on PB0
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // Internal resistor to 3.3V
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