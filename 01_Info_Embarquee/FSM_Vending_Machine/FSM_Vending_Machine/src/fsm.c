#include "fsm.h"

// 1. Encapsulation : The current state is private to this file
// The 'static' keyword prevents other files from modifying it directly
static State_t currentState;

// 2. Initialization function
void FSM_Init(void) {
    currentState = STATE_IDLE; // The machine always starts in IDLE
}

// 3. The main logic engine : Calculates the next state based on the event
void FSM_Run(Event_t event) {
    switch (currentState) {
        
        case STATE_IDLE:
            if (event == EVENT_BUTTON_MENU) {
                currentState = STATE_SELECTION;
                // Action: Display menu (to be implemented later)
            }
            break;

        case STATE_SELECTION:
            if (event == EVENT_PRODUCT_SELECTED) {
                currentState = STATE_PAYMENT;
            } 
            else if (event == EVENT_TIMEOUT) {
                currentState = STATE_IDLE;
            }
            break;

        case STATE_PAYMENT:
            if (event == EVENT_COIN_INSERTED) {
                // Here we will eventually check if the amount is sufficient
                currentState = STATE_DISTRIBUTION;
            } 
            else if (event == EVENT_TIMEOUT) {
                currentState = STATE_IDLE;
                // Action: Refund money
            }
            break;

        case STATE_DISTRIBUTION:
            if (event == EVENT_TIMEOUT) { // After 5 seconds
                currentState = STATE_IDLE;
            }
            break;

        case STATE_ERROR:
            if (event == EVENT_TIMEOUT) { // After 5 seconds
                currentState = STATE_IDLE; // Reset system
            }
            break;

        default:
            // Failsafe: if the state is unknown, go to ERROR
            currentState = STATE_ERROR;
            break;
    }
}

// 4. Getter function : Allows main.c to read the state safely
State_t FSM_GetCurrentState(void) {
    return currentState;
}