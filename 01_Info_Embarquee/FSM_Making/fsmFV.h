/**
 * @file fsm.h
 * @brief Définition de la Machine à États Finis (FSM) pour le distributeur de boissons.
 * @date 2024
 */

#ifndef FSM_H
#define FSM_H

#include <stdint.h>

/**
 * @brief Liste des états possibles de la machine.
 */
typedef enum {
    IDLE,           /**< Machine au repos, en attente d'un client */
    SELECTION,      /**< Client en train de choisir sa boisson */
    PAYMENT,        /**< Attente de l'insertion de la monnaie */
    DISTRIBUTION,   /**< Service de la boisson en cours */
    ERROR           /**< État de sécurité suite à un dysfonctionnement */
} MachineState;

/**
 * @brief Liste des événements matériels ou logiciels pris en charge.
 */
typedef enum {
    NO_EVENT,
    PRES_BTN,
    SELECT_ITEM,
    INSERT_COIN,
    DISPENSE_ITEM,
    RETURN_CHANGE,
    TIMEOUT_30S,
    INVALID_ACTION
} MachineEvent;

/**
 * @brief Initialise la machine à états (Retour à IDLE et reset du temps).
 */
void Init_FSM(void);

/**
 * @brief Injecte un événement dans la FSM et déclenche la transition correspondante.
 * @param event L'événement à traiter.
 */
void Process_Event(MachineEvent event);

/**
 * @brief Permet de lire l'état actuel.
 * @return MachineState L'état courant de la FSM.
 */
MachineState Get_Current_State(void);

/**
 * @brief Fonction système appelée en boucle pour vérifier les timeouts.
 */
void Check_Timeouts(void);

#endif // FSM_H