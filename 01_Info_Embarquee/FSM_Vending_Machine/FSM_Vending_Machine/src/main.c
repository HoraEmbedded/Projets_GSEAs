#include "stm32f1xx_hal.h"

// Cette fonction est obligatoire pour faire fonctionner les "timers" de base comme HAL_Delay()
void SysTick_Handler(void) {
    HAL_IncTick();
}

int main(void) {
    // 1. Initialiser la bibliothèque HAL
    HAL_Init();

    // 2. Allumer le courant (l'horloge) pour le port A (car notre LED est sur PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 3. Configurer la broche PA5 comme une Sortie (Output)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // Push-Pull (pour envoyer du courant)
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 4. La Boucle Infinie (Le programme tourne ici pour toujours)
    while (1) {
        // Inverser l'état de la broche PA5 (Allumé -> Éteint, ou Éteint -> Allumé)
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); 
        
        // Attendre 500 millisecondes
        HAL_Delay(500); 
    }
}