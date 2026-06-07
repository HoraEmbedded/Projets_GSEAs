#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdint.h>

void SystemClock_Config(void);

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

UART_HandleTypeDef huart2;

int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

void HARDWARE_INIT(void) {
    HAL_Init();
    SystemClock_Config();
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE(); // Port C pour le bouton utilisateur PC13
    __HAL_RCC_USART2_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Configuration PC13 (Bouton Bleu de la carte Nucleo-F401RE)
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // Résistance de pull-up externe déjà présente sur la carte
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configuration UART2 (PA2 = TX, PA3 = RX)
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2; // AF7 pour l'USART2 sur STM32F4
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart2);
}

void DISPLAY_MENU(void) {
    printf("\n=== [ECRAN] MENU DU DISTRIBUTEUR ===\n");
    printf("1. Cafe (2.00 E) - Stock: %d\n", stockCafe);
    printf("2. The (1.50 E) - Stock: %d\n", stockThe);
    printf("3. Jus (2.50 E) - Stock: %d\n", stockJus);
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

int main(void) {
    HARDWARE_INIT();
    VendingState currentState = STATE_IDLE;
    VendingEvent currentEvent = EVENT_NONE;

    HAL_Delay(500);
    printf("--- Distributeur STM32F401 Pret ---\n");
    printf("-> Appuyez sur le bouton bleu PC13 ou tapez 'm' pour le Menu\n");

    while (1) {
        if (currentEvent == EVENT_NONE) {
            // Lecture du Bouton Utilisateur PC13 (Le bouton est à 0 lorsqu'il est pressé)
            if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
                currentEvent = EVENT_BTN_MENU;
                HAL_Delay(300); 
            }

            uint8_t rx_byte = 0;
            if (HAL_UART_Receive(&huart2, &rx_byte, 1, 0) == HAL_OK) {
                printf("\n[ENTREE] Touche: %c\n", rx_byte);
                switch (rx_byte) {
                    case 'm': currentEvent = EVENT_BTN_MENU; break;
                    case '1':
                    case '2':
                    case '3': 
                        currentEvent = EVENT_SELECT_ITEM; 
                        activeProduct = rx_byte - '0'; 
                        break;
                    case 'c': currentEvent = EVENT_INSERT_COIN; break;
                    case 'a': currentEvent = EVENT_CANCEL; break;
                    case 'e': currentEvent = EVENT_INVALID_ACTION; break;
                    case 't': currentEvent = EVENT_TIMEOUT_30S; break;
                    case 'f': currentEvent = EVENT_TIMEOUT_5S; break;
                }
            }
        }

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
                        if (activeProduct == 1 && stockCafe == 0) currentEvent = EVENT_STOCK_EMPTY;
                        else if (activeProduct == 2 && stockThe == 0) currentEvent = EVENT_STOCK_EMPTY;
                        else if (activeProduct == 3 && stockJus == 0) currentEvent = EVENT_STOCK_EMPTY;

                        if (currentEvent != EVENT_STOCK_EMPTY) {
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
                        amount += 50; 
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
                        currentState = STATE_ERROR;
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
                    case EVENT_TIMEOUT_5S:
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
                    case EVENT_TIMEOUT_5S:
                        printf("[SYSTEM] Reinitialisation...\n");
                        currentState = STATE_IDLE;
                        printf("[FSM] Transition -> IDLE\n");
currentEvent = EVENT_NONE;break;default:break;}break;}}}


// Configuration de l'horloge système du STM32F401 (HSI cadencé par défaut)
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
    
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}