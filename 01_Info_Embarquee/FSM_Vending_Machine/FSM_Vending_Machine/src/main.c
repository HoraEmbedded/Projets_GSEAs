#include "stm32f1xx_hal.h"


void SysTick_Handler(void) {
    HAL_IncTick();
}

int main(void) {
    
    HAL_Init();

    
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIOInit (GPIOA, &GPIO_InitStruct);

    While(1) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

        HAL_Delay(500);
    }
}
