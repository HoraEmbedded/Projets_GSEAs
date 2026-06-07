#include <stdio.h>
#include <stdint.h>
#include <windows.h>


typedef enum {
    STATE_IDLE,
    STATE_SELECTION,
    STATE_PAYMENT,
    STATE_DISTRIBUTION,
    STATE_ERROR
} VendingState;

typedef enum {
    EVENT_NONE,
    EVENT_BTN_MENU,
    EVENT_SELECT_ITEM,
    EVENT_INSERT_COIN,
    EVENT_CANCEL,
    EVENT_INVALID_ACTION,
    EVENT_TIMEOUT_30S,
    EVENT_TIMEOUT_5S,
    EVENT_STOCK_EMPTY
} VendingEvent;

int amount = 0;             
int activePrice = 0;        
int activeProduct = 0;      

int stockCafe = 5;          
int priceCafe = 200;        

int stockThe = 3;           
int priceThe = 150;         

int stockJus = 2;           
int priceJus = 250;         

void DISPLAY_MENU(void) {
    printf("\n=== [ECRAN] MENU DU DISTRIBUTEUR ===\n");
    printf("1. Cafe (2.00 E) - Stock: %d\n", stockCafe);
    printf("2. Thé  (1.50 E) - Stock: %d\n", stockThe);
    printf("3. Jus  (2.50 E) - Stock: %d\n", stockJus);
    printf("=====================================\n");
}

void DISPENSE_CHANGE(void) {
    int changeNeeded = amount - activePrice;
    if (changeNeeded > 0) {
        printf("[MATÉRIEL] Rendu de monnaie : %d centimes distribués.\n", changeNeeded);
    } else {
        printf("[MATÉRIEL] Compte juste, aucun rendu nécessaire.\n");
    }
}

void REFUND_TOTAL(void) {
    if (amount > 0) {
        printf("[MATÉRIEL] Annulation : Restitution de %d centimes.\n", amount);
        amount = 0; 
    }
}

void GIVE_ITEM(void) {
    if (activeProduct == 1 && stockCafe > 0) stockCafe--;
    else if (activeProduct == 2 && stockThe > 0) stockThe--;
    else if (activeProduct == 3 && stockJus > 0) stockJus--;
    
    printf("[MATÉRIEL] MOTEUR ACTIF : Produit %d libéré !\n", activeProduct);
}

void STOP_ALL_MOTORS(void) {
    printf("[MATÉRIEL] ARRÊT D'URGENCE : Coupure d'alimentation des moteurs.\n");
}

void START_TIMER_30S(void) { printf("[TIMER] Lancement du compte à rebours de 30 secondes...\n"); }
void START_TIMER_5S(void)  { printf("[TIMER] Lancement du compte à rebours de 5 secondes...\n"); }

int main(void) {

    
    SetConsoleOutputCP(65001);


    VendingState currentState = STATE_IDLE;
    VendingEvent currentEvent = EVENT_NONE;
    int coinInput = 0;

    printf("--- Distributeur Allume ---\n");

    while (1) {
        if (currentEvent == EVENT_NONE) {
            printf("\n=========================================\n");
            printf(" ÉTAT ACTUEL : %d | CRÉDIT : %d cts\n", currentState, amount);
            printf("=========================================\n");
            printf(" SIMULATEUR D'ÉVÉNEMENTS :\n");
            printf(" 1 : Appuyer sur le bouton MENU\n");
            printf(" 2 : Sélectionner et valider une boisson\n");
            printf(" 3 : Insérer une pièce (Montant libre)\n");
            printf(" 4 : Appuyer sur le bouton ANNULER\n");
            printf(" 5 : Insérer fausse pièce / Double appui\n");
            printf(" 6 : Simuler un Timeout de 30 secondes\n");
            printf(" 7 : Simuler un Timeout de 5 secondes\n");
            printf(" 8 : Simuler un capteur de Stock Vide\n");
            printf("-----------------------------------------\n");
            printf(" Votre choix : ");
            
            int choix = 0;
            if (scanf("%d", &choix) == 1) {
                switch (choix) {
                    case 1: currentEvent = EVENT_BTN_MENU; break;
                    case 2: currentEvent = EVENT_SELECT_ITEM; break;
                    case 3: currentEvent = EVENT_INSERT_COIN; break;
                    case 4: currentEvent = EVENT_CANCEL; break;
                    case 5: currentEvent = EVENT_INVALID_ACTION; break;
                    case 6: currentEvent = EVENT_TIMEOUT_30S; break;
                    case 7: currentEvent = EVENT_TIMEOUT_5S; break;
                    case 8: currentEvent = EVENT_STOCK_EMPTY; break;
                    default:
                        printf("[ERREUR] Choix inconnu.\n");
                        currentEvent = EVENT_NONE;
                        break;
                }
            } else {
                while(getchar() != '\n');
                printf("[ERREUR] Entrez un nombre entier.\n");
            }
            printf("\n");
        }

        switch (currentState) {
            case STATE_IDLE:
                if (currentEvent == EVENT_BTN_MENU) {
                    DISPLAY_MENU();
                    currentState = STATE_SELECTION;
                    printf("Transition vers SELECTION\n");
                    currentEvent = EVENT_NONE;
                }
                break;

            case STATE_SELECTION:
                switch (currentEvent) {
                    case EVENT_SELECT_ITEM:
                        printf("Choisissez votre boisson (1: Cafe, 2: Thé, 3: Jus) : ");
                        int choixBoisson = 0;
                        if (scanf("%d", &choixBoisson) == 1 && choixBoisson >= 1 && choixBoisson <= 3) {
                            activeProduct = choixBoisson;
                            
                            if (activeProduct == 1 && stockCafe == 0) currentEvent = EVENT_STOCK_EMPTY;
                            else if (activeProduct == 2 && stockThe == 0) currentEvent = EVENT_STOCK_EMPTY;
                            else if (activeProduct == 3 && stockJus == 0) currentEvent = EVENT_STOCK_EMPTY;
                            
                            if (currentEvent != EVENT_STOCK_EMPTY) {
                                if (activeProduct == 1) activePrice = priceCafe;
                                else if (activeProduct == 2) activePrice = priceThe;
                                else if (activeProduct == 3) activePrice = priceJus;
                                
                                currentState = STATE_PAYMENT;
                                printf("Transition vers PAYMENT (Prix : %d cts)\n", activePrice);
                                currentEvent = EVENT_NONE;
                            }
                        } else {
                            while(getchar() != '\n');
                            printf("[ERREUR] Choix de boisson invalide.\n");
                            currentEvent = EVENT_NONE;
                        }
                        break;

                    case EVENT_STOCK_EMPTY:
                        printf("Alerte Machine : Stock vide !\n");
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_TIMEOUT_30S:
                        currentState = STATE_IDLE;
                        printf("Délai dépassé\n");
                        currentEvent = EVENT_NONE;
                        break;

                    default:
                        break;
                }
                break;

            case STATE_PAYMENT:
                switch (currentEvent) {
                    case EVENT_INSERT_COIN:
                        printf("Entrez la valeur de la pièce (en centimes, ex: 50, 100, 200) : ");
                        if (scanf("%d", &coinInput) == 1 && coinInput > 0) {
                            amount += coinInput;
                            printf("Montant actuel : %d / Prix : %d\n", amount, activePrice);
                            
                            if (amount >= activePrice) {
                                DISPENSE_CHANGE();
                                GIVE_ITEM();
                                START_TIMER_5S();
                                currentState = STATE_DISTRIBUTION;
                                printf("Transition vers DISTRIBUTION\n");
                            } else {
                                printf("Montant insuffisant (%d cts manquants).\n", activePrice - amount);
                            }
                        } else {
                            while(getchar() != '\n');
                            printf("[ERREUR] Saisie de pièce incorrecte.\n");
                        }
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_INVALID_ACTION:
                        printf("[ALERTE] Pièce non reconnue ! Rejet de la pièce.\n");
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_CANCEL:
                        REFUND_TOTAL();
                        currentState = STATE_IDLE;
                        printf("Paiement annulé. Retour à IDLE.\n");
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_TIMEOUT_30S:
                        REFUND_TOTAL();
                        currentState = STATE_IDLE;
                        printf("Délai dépassé (30s). Retour à IDLE.\n");
                        currentEvent = EVENT_NONE;
                        break;

                    default:
                        break;
                }
                break;

            case STATE_DISTRIBUTION:
                switch (currentEvent) {
                    case EVENT_TIMEOUT_5S:
                        amount = 0;
                        activeProduct = 0;
                        activePrice = 0;
                        printf("Produit délivré. Bonne digestion !\n");
                        currentState = STATE_IDLE;
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_CANCEL:
                        printf("Action impossible, distribution en cours...\n");
                        currentEvent = EVENT_NONE;
                        break;

                    default:
                        break;
                }
                break;

            case STATE_ERROR:
                STOP_ALL_MOTORS();
                switch (currentEvent) {
                    case EVENT_TIMEOUT_5S:
                        printf("[SYSTEM] Tentative de réinitialisation automatique...\n");
                        currentState = STATE_IDLE;
                        currentEvent = EVENT_NONE;
                    break;
                    
                    case EVENT_INVALID_ACTION:
                    printf("[SYSTEM] Entrée invalide...\n");
                    currentState = STATE_IDLE;
                    currentEvent = EVENT_NONE;
                    break;
                }
            break;
        }
    }
}