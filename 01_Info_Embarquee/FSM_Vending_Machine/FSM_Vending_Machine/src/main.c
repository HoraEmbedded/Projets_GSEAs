#include "stm32f1xx_hal.h"
#include "hardware.h"
#include "fsm.h"

// Required by HAL for timing functions (HAL_Delay)
void SysTick_Handler(void) {
    HAL_IncTick();
}

int main(void) {
    // 1. System Initialization
    HAL_Init();
    HW_Init();
    FSM_Init();

    // Variables to track the button state and prevent spamming events
    bool previousButtonState = false;
    bool currentButtonState = false;

    // 2. The Super Loop
    while (1) {
        
        // --- Phase A: READ INPUTS ---
        currentButtonState = HW_IsMenuButtonPressed();
        Event_t currentEvent = EVENT_NONE;

        // Edge Detection: We only want to trigger the event exactly when 
        // the button transitions from "not pressed" to "pressed"
        if (currentButtonState == true && previousButtonState == false) {
            currentEvent = EVENT_BUTTON_MENU;
        }
        
        // Save the state for the next loop iteration
        previousButtonState = currentButtonState;

        // --- Phase B: PROCESS LOGIC (FSM) ---
        if (currentEvent != EVENT_NONE) {
            FSM_Run(currentEvent);
        }

        // --- Phase C: WRITE OUTPUTS ---
        State_t currentState = FSM_GetCurrentState();
        
        // The Green LED is only ON during the IDLE state
        if (currentState == STATE_IDLE) {
            HW_SetIdleLed(true);
        } else {
            HW_SetIdleLed(false);
        }

        // Small delay for basic "debouncing" (anti-rebond)
        HAL_Delay(50);
    }
}