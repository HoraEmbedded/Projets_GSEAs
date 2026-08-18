#include "display.h"
#include <u8g2.h>
#include "stm32f1xx_hal.h"

// On récupère le canal I2C configuré dans hardware.c
extern I2C_HandleTypeDef hi2c1;
static u8g2_t u8g2; // L'objet qui représente notre écran

// --- TRADUCTEUR 1 : Envoi des données I2C ---
uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    static uint8_t buffer[32];
    static uint8_t buf_idx;
    uint8_t *data;

    switch(msg) {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while(arg_int > 0) {
                buffer[buf_idx++] = *data++;
                arg_int--;
            }
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            // Envoi des pixels à l'écran via HAL
            HAL_I2C_Master_Transmit(&hi2c1, u8x8_GetI2CAddress(u8x8), buffer, buf_idx, HAL_MAX_DELAY);
            break;
        default: return 0;
    }
    return 1;
}

// --- TRADUCTEUR 2 : Gestion du temps ---
uint8_t u8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch(msg) {
        case U8X8_MSG_DELAY_MILLI:
            HAL_Delay(arg_int);
            break;
        default: return 0;
    }
    return 1;
}

// --- FONCTIONS PUBLIQUES ---

void Display_Init(void) {
    // Connexion de la bibliothèque à notre écran SSD1306
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_stm32_hw_i2c, u8x8_gpio_and_delay_stm32);
    u8g2_SetI2CAddress(&u8g2, 0x78); // L'adresse standard de l'OLED
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0); // Allumer l'écran
}

void Display_ShowIdleMenu(void) {
    u8g2_ClearBuffer(&u8g2); // Effacer l'écran précédent
    
    // On choisit une petite police de caractères
    u8g2_SetFont(&u8g2, u8g2_font_5x7_tr); 
    
    // On dessine le texte (X, Y)
    u8g2_DrawStr(&u8g2, 15, 8, "=== DISTRIBUTEUR ===");
    u8g2_DrawStr(&u8g2, 0, 20, "1: Cafe      1.00 E");
    u8g2_DrawStr(&u8g2, 0, 30, "2: Soda      1.50 E");
    u8g2_DrawStr(&u8g2, 0, 40, "3: The       1.20 E");
    u8g2_DrawStr(&u8g2, 0, 50, "4: Jus       2.00 E");
    u8g2_DrawStr(&u8g2, 15, 62, "  Faites un choix  ");
    
    u8g2_SendBuffer(&u8g2); // Envoyer l'image à l'écran
}