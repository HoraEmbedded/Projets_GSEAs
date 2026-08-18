#include "fsm.h"
#include <stddef.h> // Nécessaire pour la constante NULL

static State_t currentState;
static uint16_t currentBalance = 0;
static uint16_t productPrice = 150; // Prix fixé à 150 centimes (1.50€)

// ==============================================================================
// 1. LES FONCTIONS D'ACTION (Logique Métier)
// Chaque fonction exécute une tâche et retourne le NOUVEL ÉTAT dans lequel aller
// ==============================================================================

static State_t Action_ShowMenu(void) {
    return STATE_SELECTION;
}

static State_t Action_SelectProduct(void) {
    return STATE_PAYMENT;
}

static State_t Action_ProcessCoin(void) {
    currentBalance += 50; // Chaque appui sur le bouton rouge ajoute 50 centimes
    
    if (currentBalance >= productPrice) {
        currentBalance = 0; // Achat validé, on réinitialise la caisse (la monnaie serait rendue ici)
        return STATE_DISTRIBUTION; // Assez d'argent -> On distribue !
    }
    return STATE_PAYMENT; // Pas assez d'argent -> On reste dans l'état de paiement
}

static State_t Action_DispenseFinished(void) {
    return STATE_IDLE; // La distribution est finie (5 secondes écoulées)
}

static State_t Action_CancelAndRefund(void) {
    currentBalance = 0; // Remboursement de l'argent inséré
    return STATE_IDLE;
}

// ==============================================================================
// 2. LA TABLE DE TRANSITIONS
// ==============================================================================

// Structure d'une ligne de la table
typedef struct {
    State_t state;
    Event_t event;
    State_t (*action)(void); // Pointeur vers la fonction d'action à exécuter
} Transition_t;

// La base de données des règles de notre machine
static const Transition_t fsmTable[] = {
    {STATE_IDLE,         EVENT_BUTTON_MENU,      Action_ShowMenu},
    {STATE_SELECTION,    EVENT_PRODUCT_SELECTED, Action_SelectProduct},
    {STATE_SELECTION,    EVENT_TIMEOUT,          Action_CancelAndRefund},
    {STATE_PAYMENT,      EVENT_COIN_INSERTED,    Action_ProcessCoin},
    {STATE_PAYMENT,      EVENT_TIMEOUT,          Action_CancelAndRefund},
    {STATE_DISTRIBUTION, EVENT_TIMEOUT,          Action_DispenseFinished},
    {STATE_ERROR,        EVENT_TIMEOUT,          Action_CancelAndRefund}
};

// Calcul automatique du nombre de lignes dans le tableau
#define FSM_TABLE_SIZE (sizeof(fsmTable) / sizeof(Transition_t))

// ==============================================================================
// 3. LE MOTEUR DE LA FSM
// ==============================================================================

void FSM_Init(void) {
    currentState = STATE_IDLE;
    currentBalance = 0;
}

void FSM_Run(Event_t event) {
    // On parcourt la table pour trouver la règle qui correspond à notre situation
    for (uint8_t i = 0; i < FSM_TABLE_SIZE; i++) {
        if (fsmTable[i].state == currentState && fsmTable[i].event == event) {
            
            // Si une action est définie, on l'exécute, et on récupère le nouvel état
            if (fsmTable[i].action != NULL) {
                currentState = fsmTable[i].action();
            }
            break; // On a trouvé et traité la transition, on arrête de chercher
        }
    }
}

// ==============================================================================
// 4. LES GETTERS (Accès aux données privées)
// ==============================================================================
State_t FSM_GetCurrentState(void) { return currentState; }
uint16_t FSM_GetBalance(void)     { return currentBalance; }