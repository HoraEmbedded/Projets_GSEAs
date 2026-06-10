#ifndef FSM_H
#define FSM_H

// Definition of the states for our Vending Machine
typedef enum {
    STATE_IDLE,
    STATE_SELECTION,
    STATE_PAYMENT,
    STATE_DISTRIBUTION,
    STATE_ERROR
} State_t;

// Definition of the events that trigger transitions
typedef enum {
    EVENT_NONE,
    EVENT_BUTTON_MENU,
    EVENT_PRODUCT_SELECTED,
    EVENT_COIN_INSERTED,
    EVENT_TIMEOUT,
    EVENT_ERROR_OCCURRED
} Event_t;

// Function prototypes for our FSM logic
void FSM_Init(void);
void FSM_Run(Event_t event);

#endif // FSM_H