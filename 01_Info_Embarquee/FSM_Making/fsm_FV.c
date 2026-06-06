/**
 * @file fsm.c
 * @brief Implémentation du moteur de la FSM par Table de Transitions.
 */

#include <stdio.h>
#include <stddef.h>
#include "fsmFV.h"


// --- Variables privées du module ---
static MachineState currentState = IDLE;
static uint32_t time_entered_selection = 0;

// --- Fonctions de simulation (Mocks) ---
uint32_t GetSystemTick(void) { return 0; /* Simulé */ }
void SendUartMessage(const char* msg) { printf("UART: %s\n", msg); }

// --- Actions de la FSM ---
void DisplayMenu(void) { printf("Action : Affichage du Menu\n"); }
void PaymentBoard(void) { printf("Action : Attente paiement\n"); }
void DispenseDrink(void) { printf("Action : Boisson servie\n"); }
void ReturnMoney(void) { printf("Action : Rendu monnaie\n"); }
void GoToError(void) { SendUartMessage("MISE EN SECURITE !"); }

// --- Structure et Table de Transitions ---
typedef void (*ActionFunction)(void);

typedef struct {
    MachineState currentState;
    MachineEvent event;
    MachineState nextState;
    ActionFunction actionFn;
} StateTransition;

static const StateTransition fsm_table[] = {
    {IDLE,         PRES_BTN,      SELECTION,    DisplayMenu},
    {SELECTION,    SELECT_ITEM,   PAYMENT,      PaymentBoard},
    {SELECTION,    TIMEOUT_30S,   IDLE,         NULL},
    {PAYMENT,      INSERT_COIN,   DISTRIBUTION, DispenseDrink},
    {PAYMENT,      RETURN_CHANGE, IDLE,         ReturnMoney},
    {PAYMENT,      TIMEOUT_30S,   ERROR,        GoToError},
    {DISTRIBUTION, DISPENSE_ITEM, IDLE,         NULL},
    {DISTRIBUTION, INVALID_ACTION,ERROR,        GoToError}
};
#define TABLE_SIZE (sizeof(fsm_table)/sizeof(fsm_table[0]))

// --- Fonctions Publiques ---

void Init_FSM(void) {
    currentState = IDLE;
    time_entered_selection = 0;
}

MachineState Get_Current_State(void) {
    return currentState;
}

void Process_Event(MachineEvent event) {
    for(int i = 0; i < TABLE_SIZE; i++) {
        if(fsm_table[i].currentState == currentState && fsm_table[i].event == event) {
            
            if(fsm_table[i].actionFn != NULL) {
                fsm_table[i].actionFn();
            }
            
            currentState = fsm_table[i].nextState;
            
            // Armement du chronomètre si on entre en sélection
            if (currentState == SELECTION) {
                time_entered_selection = GetSystemTick();
            }
            break; 
        }
    }
}

void Check_Timeouts(void) {
    if (currentState == SELECTION || currentState == PAYMENT) {
        uint32_t time_elapsed = GetSystemTick() - time_entered_selection;
        if (time_elapsed >= 30000) {
            SendUartMessage("Timeout detecte.");
            Process_Event(TIMEOUT_30S); 
        }
    }
}