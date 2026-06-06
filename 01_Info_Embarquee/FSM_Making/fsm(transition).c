#include <stdio.h>
#include <stdint.h> // Permet d'utiliser des types entiers précis comme uint32_t


uint32_t GetSystemTick(void) {
    
    return 0; 
}

//(Message UART)
void SendUartTimeoutMessage(void) {
    // En vrai C embarqué, on utiliserait une fonction comme HAL_UART_Transmit()
    printf("UART MESSAGE : Timeout detecte, retour a l'accueil.\n");
}


// 0. On définit les types de l'état et de l'événement pour la machine
typedef enum{
    IDLE,
    SELECTION,
    PAYMENT,
    DISTRIBUTION,
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

MachineEvent read_sensor(void);

// 1. On définit le "Pointeur de fonction" 
typedef void (*ActionFunction)(void);

// 2. On crée le moule (la Structure) d'une transition
typedef struct {
    MachineState currentState;   // L'état actuel
    MachineEvent event;          // L'événement déclencheur
    MachineState nextState;      // Le prochain état
    ActionFunction actionFn;     // L'action à exécuter
} StateTransition;

// 3. On crée nos fonctions d'action (les "Comment ?")
void DisplayMenu(void) { printf("Action : Affichage du Menu\n"); }
void PaymentBoard(void) { printf("Action : Attente paiement\n"); }
void DispenseDrink(void) { printf("Action : Boisson servie\n"); }
void ReturnMoney(void) { printf("Action : Rendu monnaie\n"); }

// 4. Table de Transitions
StateTransition fsm_table[] = {
    // ETAT ACTUEL, EVENEMENT,    PROCHAIN ETAT,  ACTION
    {IDLE,          PRES_BTN,     SELECTION,      DisplayMenu},
    {SELECTION,     SELECT_ITEM,  PAYMENT,        PaymentBoard},
    {PAYMENT,       INSERT_COIN,  DISTRIBUTION,   DispenseDrink},
    {PAYMENT,       RETURN_CHANGE,IDLE,           ReturnMoney},
    {DISTRIBUTION,  DISPENSE_ITEM,IDLE,           0} 
};

// calcul automatique de la taille du tableau pour l'ordinateur
#define TABLE_SIZE (sizeof(fsm_table)/sizeof(fsm_table[0]))

void run_fsm() {
    MachineState currentState = IDLE;
    MachineEvent currentEvent = NO_EVENT;
    
    //  heure entré dans l'état SELECTION
    uint32_t time_entered_selection = 0; 

    while(1) {
        // A.  vérifie les capteurs
        currentEvent = read_sensor(); 

        // B. GESTION DU TEMPS (La nouveauté !)
        // Si on est dans l'état SELECTION et qu'aucun bouton n'a été touché...
        if (currentState == SELECTION && currentEvent == NO_EVENT) {
            
            // On calcule la différence entre "Maintenant" et "Le moment où on est entré"
            uint32_t time_elapsed = GetSystemTick() - time_entered_selection;
            
            // 30 secondes = 30 000 millisecondes
            if (time_elapsed >= 30000) {
                currentEvent = TIMEOUT30S; // On génère artificiellement l'événement !
                SendUartTimeoutMessage();  // On envoie le message UART demandé
            }
        }

    while(1) {

        currentEvent = read_sensor(); 

        
        for(int i = 0; i < TABLE_SIZE; i++) {
            if(fsm_table[i].currentState == currentState && fsm_table[i].event == currentEvent) {
                
                if(fsm_table[i].actionFn != 0) { 
                    fsm_table[i].actionFn(); // Exécute l'action
                }
                currentState = fsm_table[i].nextState; // Transition vers le prochain état
                printf("Transition to state %d\n", currentState);
                break; // Sort de la boucle après avoir trouvé la transition
            }
        }
    }
}

