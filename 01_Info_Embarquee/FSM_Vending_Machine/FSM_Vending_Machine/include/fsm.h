#ifndef FSM_H
#define FSM_H

#include <stdint.h>

/**
 * @file fsm.h
 * @author Horacia
 * @brief Machine à États Finis (FSM) pour Distributeur de Boissons
 * * @details
 * Ce fichier définit les états et les événements de la machine.
 * La logique est implémentée via une Table de Transitions robuste.
 * * @par Diagramme d'états
 * \dot
 * digraph VendingMachineFSM {
 * node [shape=box, fontname="Helvetica", fontsize=10];
 * edge [fontname="Helvetica", fontsize=9];
 * * IDLE -> SELECTION [label=" Bouton Menu"];
 * SELECTION -> PAYMENT [label=" Produit Choisi"];
 * SELECTION -> IDLE [label=" Timeout (Inactivité)"];
 * * PAYMENT -> PAYMENT [label=" Pièce insérée (Solde < Prix)"];
 * PAYMENT -> DISTRIBUTION [label=" Pièce insérée (Solde >= Prix)"];
 * PAYMENT -> IDLE [label=" Timeout (Annulation)"];
 * * DISTRIBUTION -> IDLE [label=" Fin de distribution (Timeout)"];
 * ERROR -> IDLE [label=" Reset automatique (Timeout)"];
 * }
 * \enddot
 */

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


void FSM_Init(void);
void FSM_Run(Event_t event);
State_t FSM_GetCurrentState(void);

uint16_t FSM_GetBalance(void);

#endif // FSM_H