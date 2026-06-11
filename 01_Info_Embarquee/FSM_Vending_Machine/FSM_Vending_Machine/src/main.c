#include "stm32f1xx_hal.h"
#include "hardware.h"
#include "fsm.h"

void SysTick_Handler(void) { HAL_IncTick(); }

int main(void) {
    HAL_Init();
    HW_Init();
    FSM_Init();

    // Arrays to handle edge detection for 3 buttons cleanly: [0]=Menu, [1]=Select, [2]=Coin
    bool prevBtn[3] = {false, false, false};
    bool currBtn[3] = {false, false, false};

    // Variables for the Non-Blocking Timer
    uint32_t stateEntryTime = HAL_GetTick(); 
    State_t previousFsmState = STATE_IDLE;

    while (1) {
        Event_t currentEvent = EVENT_NONE;
        State_t currentFsmState = FSM_GetCurrentState();

        // --- PHASE 1: TIMEOUT CHECK ---
        // If the state just changed, reset our stopwatch
        if (currentFsmState != previousFsmState) {
            stateEntryTime = HAL_GetTick();
            previousFsmState = currentFsmState;
        }

        // If we are in SELECTION state and 5 seconds have passed (testing value)
        if (currentFsmState == STATE_SELECTION) {
            if ((HAL_GetTick() - stateEntryTime) >= 5000) {
                currentEvent = EVENT_TIMEOUT;
            }
        }

        // --- PHASE 2: READ BUTTONS (If no timeout occurred) ---
        if (currentEvent == EVENT_NONE) {
            currBtn[0] = HW_IsMenuButtonPressed();
            currBtn[1] = HW_IsSelectButtonPressed();
            currBtn[2] = HW_IsCoinButtonPressed();

            if (currBtn[0] == true && prevBtn[0] == false) currentEvent = EVENT_BUTTON_MENU;
            else if (currBtn[1] == true && prevBtn[1] == false) currentEvent = EVENT_PRODUCT_SELECTED;
            else if (currBtn[2] == true && prevBtn[2] == false) currentEvent = EVENT_COIN_INSERTED;

            prevBtn[0] = currBtn[0];
            prevBtn[1] = currBtn[1];
            prevBtn[2] = currBtn[2];
        }

        // --- PHASE 3: PROCESS LOGIC ---
        if (currentEvent != EVENT_NONE) {
            FSM_Run(currentEvent);
        }

        // --- PHASE 4: WRITE OUTPUTS ---
        if (FSM_GetCurrentState() == STATE_IDLE) {
            HW_SetIdleLed(true);
        } else {
            HW_SetIdleLed(false);
        }

        HAL_Delay(50); // Debouncing
    }
}