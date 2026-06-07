#include "stm32c0xx_hal.h"
#include <stdio.h>
#include <stdint.h>

// --- DÉCLARATION DES ÉTATS ET ÉVÉNEMENTS ---
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

// --- VARIABLES GLOBALES ---
int amount = 0;
int activePrice = 0;
int activeProduct = 0;

int stockCafe = 5;
int priceCafe = 200; // 2,00€

int stockThe = 3;
int priceThe = 150;  // 1,50€

int stockJus = 2;
int priceJus = 250;  // 2,50€


// --- DÉCLARATION DU MATÉRIEL (UART) ---
UART_HandleTypeDef huart2;

// Redirection du printf vers le terminal Wokwi (UART2)
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

// Configuration des broches et de la communication
void HARDWARE_INIT(void) {
    HAL_Init();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    // Configuration de la broche du bouton (PA0) en Entrée
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configuration de la communication série (PA2, PA3) pour le terminal
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&huart2);
}

// --- FONCTIONS STUB (Bouchons Matériels) ---
void DISPLAY_MENU(void) {
    printf("\n=== [ECRAN] MENU DU DISTRIBUTEUR ===\n");
    printf("1. Cafe (2.00 E) - Stock: %d\n", stockCafe);
    printf("2. The  (1.50 E) - Stock: %d\n", stockThe);
    printf("3. Jus  (2.50 E) - Stock: %d\n", stockJus);
    printf("=====================================\n");
}

void DISPENSE_CHANGE(void) {
    int changeNeeded = amount - activePrice;
    if (changeNeeded > 0) {
        printf("[MATERIEL] Rendu de monnaie : %d centimes distribues.\n", changeNeeded);
    } else {
        printf("[MATERIEL] Compte juste, aucun rendu necessaire.\n");
    }
}

void REFUND_TOTAL(void) {
    if (amount > 0) {
        printf("[MATERIEL] Annulation : Restitution de %d centimes.\n", amount);
        amount = 0;
    }
}

void GIVE_ITEM(void) {
    if (activeProduct == 1 && stockCafe > 0) stockCafe--;
    else if (activeProduct == 2 && stockThe > 0) stockThe--;
    else if (activeProduct == 3 && stockJus > 0) stockJus--;
    
    printf("[MATERIEL] MOTEUR ACTIF : Produit %d libere !\n", activeProduct);
}

void STOP_ALL_MOTORS(void) {
    printf("[MATERIEL] ARRET D'URGENCE : Coupure d'alimentation des moteurs.\n");
}

void START_TIMER_5S(void) { 
    printf("[TIMER] Lancement du compte a rebours de 5 secondes...\n"); 
}


// --- PROGRAMME PRINCIPAL ---
int main(void) {
    
    HARDWARE_INIT();

    VendingState currentState = STATE_IDLE;
    VendingEvent currentEvent = EVENT_NONE;
    
    // Pause de démarrage pour laisser Wokwi initialiser le terminal
    HAL_Delay(500); 

    printf("--- Distributeur STM32 Pret ---\n");
    printf("-> Appuyez sur le bouton virtuel PA0 ou tapez 'm' pour le Menu\n");

    while (1) {
        
        // 1. CAPTURE DES ÉVÉNEMENTS MATÉRIELS ET TERMINAL
        if (currentEvent == EVENT_NONE) {
            
            // A. Lecture du vrai Bouton physique sur Wokwi (Broche PA0)
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {
                currentEvent = EVENT_BTN_MENU;
                HAL_Delay(300); // Anti-rebond basique
            }

            // B. Lecture non-bloquante du clavier Wokwi (Terminal)
            uint8_t rx_byte = 0;
            // Si on reçoit 1 caractère (Timeout = 0ms pour ne pas bloquer)
            if (HAL_UART_Receive(&huart2, &rx_byte, 1, 0) == HAL_OK) {
                printf("\n[ENTREE] Touche: %c\n", rx_byte);
                switch (rx_byte) {
                    case 'm': currentEvent = EVENT_BTN_MENU; break;
                    case '1': 
                    case '2': 
                    case '3': 
                        currentEvent = EVENT_SELECT_ITEM;
                        activeProduct = rx_byte - '0'; // Astuce C pour convertir le texte '1' en chiffre 1
                        break;
                    case 'c': currentEvent = EVENT_INSERT_COIN; break;
                    case 'a': currentEvent = EVENT_CANCEL; break;
                    case 'e': currentEvent = EVENT_INVALID_ACTION; break;
                    case 't': currentEvent = EVENT_TIMEOUT_30S; break;
                    case 'f': currentEvent = EVENT_TIMEOUT_5S; break;
                }
            }
        }

        // 2. LA MACHINE À ÉTATS (FSM V1)
        switch (currentState) {
            
            case STATE_IDLE:
                if (currentEvent == EVENT_BTN_MENU) {
                    DISPLAY_MENU();
                    currentState = STATE_SELECTION;
                    printf("[FSM] Transition -> SELECTION\n");
                    printf("-> Touches (1: Cafe, 2: The, 3: Jus)\n");
                    currentEvent = EVENT_NONE;
                }
                break;

            case STATE_SELECTION:
                switch (currentEvent) {
                    case EVENT_SELECT_ITEM:
                        // Vérification des stocks
                        if (activeProduct == 1 && stockCafe == 0) currentEvent = EVENT_STOCK_EMPTY;
                        else if (activeProduct == 2 && stockThe == 0) currentEvent = EVENT_STOCK_EMPTY;
                        else if (activeProduct == 3 && stockJus == 0) currentEvent = EVENT_STOCK_EMPTY;
                        
                        if (currentEvent != EVENT_STOCK_EMPTY) {
                            // Assignation du prix
                            if (activeProduct == 1) activePrice = priceCafe;
                            else if (activeProduct == 2) activePrice = priceThe;
                            else if (activeProduct == 3) activePrice = priceJus;
                            
                            currentState = STATE_PAYMENT;
                            printf("[FSM] Transition -> PAYMENT (Prix : %d cts)\n", activePrice);
                            printf("-> Touches (c: Inserer 50cts, a: Annuler, e: Fausse piece, t: Timeout 30s)\n");
                            currentEvent = EVENT_NONE;
                        }
                        break;

                    case EVENT_STOCK_EMPTY:
                        printf("[ALERTE] Stock vide pour le produit %d !\n", activeProduct);
                        currentState = STATE_ERROR;
                        printf("-> Touche (f: Fin erreur 5s)\n");
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_TIMEOUT_30S:
                        currentState = STATE_IDLE;
                        printf("[INFO] Delai depasse. Retour a IDLE\n");
                        currentEvent = EVENT_NONE;
                        break;

                    default:
                        break;
                }
                break;

            case STATE_PAYMENT:
                switch (currentEvent) {
                    case EVENT_INSERT_COIN:
                        amount += 50; // On insère 50 cts à chaque appui sur 'c'
                        printf("[FSM] Montant : %d / %d\n", amount, activePrice);
                        
                        if (amount >= activePrice) {
                            DISPENSE_CHANGE();
                            GIVE_ITEM();
                            START_TIMER_5S();
                            currentState = STATE_DISTRIBUTION;
                            printf("[FSM] Transition -> DISTRIBUTION\n");
                            printf("-> Touche (f: Fin de distribution 5s)\n");
                        }
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_INVALID_ACTION:
                        printf("[ALERTE] Piece non reconnue ! Rejet.\n");
                        currentState = STATE_ERROR; // Correction: On va vers l'erreur
                        printf("-> Touche (f: Fin erreur 5s)\n");
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_CANCEL:
                        REFUND_TOTAL();
                        currentState = STATE_IDLE;
                        printf("[INFO] Paiement annule. Retour a IDLE.\n");
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_TIMEOUT_30S:
                        REFUND_TOTAL();
                        currentState = STATE_IDLE;
                        printf("[INFO] Delai depasse. Retour a IDLE.\n");
                        currentEvent = EVENT_NONE;
                        break;

                    default:
                        break;
                }
                break;

            case STATE_DISTRIBUTION:
                switch (currentEvent) {
                    case EVENT_TIMEOUT_5S: // On utilise la touche 'f' pour simuler la fin
                        amount = 0;
                        activeProduct = 0;
                        activePrice = 0;
                        printf("[INFO] Produit delivre !\n");
                        currentState = STATE_IDLE;
                        printf("[FSM] Transition -> IDLE\n");
                        currentEvent = EVENT_NONE;
                        break;

                    case EVENT_CANCEL:
                        printf("[ERREUR] Impossible, distribution en cours...\n");
                        currentEvent = EVENT_NONE;
                        break;

                    default:
                        break;
                }
                break;

            case STATE_ERROR:
                STOP_ALL_MOTORS();
                switch (currentEvent) {
                    case EVENT_TIMEOUT_5S: // On utilise la touche 'f' pour sortir de l'erreur
                        printf("[SYSTEM] Reinitialisation...\n");
                        currentState = STATE_IDLE;
                        printf("[FSM] Transition -> IDLE\n");
                        currentEvent = EVENT_NONE;
                        break;
                        
                    default:
                        break;
                }
                break;
        }
    }
}