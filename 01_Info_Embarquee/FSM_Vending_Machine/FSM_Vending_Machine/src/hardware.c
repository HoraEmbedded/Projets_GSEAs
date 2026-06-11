#include "hardware.h"
#include "stm32f1xx_hal.h"

// 1. DÉCLARATION GLOBALE (Crucial pour que display.c puisse utiliser l'écran)
I2C_HandleTypeDef hi2c1;

void HW_Init(void) {
    // 2. Activer les horloges (une seule fois au début)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    // On déclare notre fiche de configuration une seule fois
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // --- CONFIGURATION DE L'ÉCRAN OLED (I2C) ---
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);

    // --- CONFIGURATION DES LEDS ---
    // Optimisation : On configure PA1 (Bleue) et PA5 (Verte) en même temps
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_5; 
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // --- PRÉPARATION DU CLAVIER (Keypad) ---
    // Les LIGNES (Rows) envoient du courant : Mode OUTPUT
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Les COLONNES (Cols) lisent le courant : Mode INPUT avec résistance (PULLUP)
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// Fonction pour piloter la LED Verte (IDLE)
void HW_SetIdleLed(bool state) {
    if (state) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    else       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

// Fonction pour piloter la LED Bleue (DISTRIBUTION)
void HW_SetDistLed(bool state) {
    if (state) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    else       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}