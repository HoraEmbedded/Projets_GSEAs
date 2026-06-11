#include "stm32f1xx_hal.h"
#include "hardware.h"
#include "fsm.h"
#include "display.h" // <-- C'est ça qui manquait pour l'écran !

void SysTick_Handler(void) { HAL_IncTick(); }

int main(void) {
    // 1. Initialisation du système
    HAL_Init();
    HW_Init();
    FSM_Init();

    // 2. Allumage de l'écran OLED
    Display_Init();
    Display_ShowIdleMenu();

    // 3. Boucle principale (temporairement vide pour tester l'écran)
    while (1) {
        // On fait juste clignoter la LED verte pour vérifier que la carte n'est pas plantée
        HW_SetIdleLed(true);
        HAL_Delay(500);
        HW_SetIdleLed(false);
        HAL_Delay(500);
    }
}