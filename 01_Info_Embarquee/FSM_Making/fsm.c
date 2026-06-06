#include    <stdio.h>
#include    <stdlib.h>

typedef enum{
    IDLE,
    SELECTION,
    DISTRIBUTION,
    PAYMENT,
    ERROR
} MachineState;

typedef enum{
    NO_EVENT,
    PRES_BTN,
    SELECT_ITEM,
    INSERT_COIN,
    DISPENSE_ITEM,
    RETURN_CHANGE,
    TIMEOUT30S,
    INVALID_ACTION
} MachineEvent;

void run_fsm(){

MachineState currentState = IDLE;
MachineEvent currentEvent = NO_EVENT;

while(1) {

    currentEvent = read_sensor(); // Hypothetical function to get the next event

    switch(currentState) {
        case IDLE:
            if(currentEvent == PRES_BTN) {
                currentState = SELECTION;
                printf("Transition to SELECTION\n");
            }
            break;

        case SELECTION:
            if(currentEvent == SELECT_ITEM) {
                currentState = DISTRIBUTION;
                printf("Transition to DISTRIBUTION\n");
            } else if(currentEvent == TIMEOUT30S) {
                currentState = IDLE;
                printf("Transition to IDLE due to timeout\n");
            }
            break;

        case DISTRIBUTION:
            if(currentEvent == DISPENSE_ITEM) {
                currentState = PAYMENT;
                printf("Transition to PAYMENT\n");
            } else if(currentEvent == INVALID_ACTION) {
                currentState = ERROR;
                printf("Transition to ERROR due to invalid action\n");
            }
            break;

        case PAYMENT:
            if(currentEvent == INSERT_COIN) {
                currentState = IDLE;
                printf("Transition to IDLE after payment\n");
            } else if(currentEvent == RETURN_CHANGE) {
                currentState = IDLE;
                printf("Transition to IDLE after returning change\n");
            } else if(currentEvent == TIMEOUT30S) {
                currentState = ERROR;
                printf("Transition to ERROR due to timeout\n");
            }
            break;

        case ERROR:
            // Handle error state, possibly reset or alert
            printf("In ERROR state, awaiting reset...\n");
            break;

        default:
            // Handle unexpected states
            printf("Unexpected state encountered!\n");
            break;
    }
}

}
