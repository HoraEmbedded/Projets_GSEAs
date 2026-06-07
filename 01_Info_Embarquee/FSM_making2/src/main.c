/* ============================================================================= 
 * Distributeur Automatique - STM32 Blue Pill (F103C8) 
 * ============================================================================= */ 
#include "stm32f1xx_hal.h" 
#include <stdio.h> 
#include <stdint.h> 
#include <string.h>


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
static int g_amount = 0; 
static int g_activePrice = 0; 
static int g_activeProduct = 0; 
static int g_stockCafe = 5; 
static const int PRICE_CAFE = 200; 
static int g_stockThe = 3; 
static const int PRICE_THE = 150; 
static int g_stockJus = 2; 
static const int PRICE_JUS = 250; 
static UART_HandleTypeDef huart2; static I2C_HandleTypeDef hi2c1; 

/* --- Pilote LCD I2C --- */ #define LCD_I2C_ADDR (0x27 << 1) #define LCD_COLS 20 
static void lcd_send_nibble(uint8_t nibble, uint8_t rs) { 
    uint8_t frame[4]; 
    uint8_t base = (nibble & 0xF0) | (rs ? 0x01 : 0x00); 
    frame[0] = base | 0x0C; 
    frame[1] = base | 0x08; 
    frame[2] = base | 0x0C; 
    frame[3] = base | 0x08; 
    HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR, frame, 4, 100); 
} 
static void lcd_send_byte(uint8_t byte, uint8_t rs) { 
    uint8_t frame[4]; 
    frame[0] = (byte & 0xF0) | (rs ? 0x0D : 0x0C); 
    frame[1] = (byte & 0xF0) | (rs ? 0x09 : 0x08); 
    frame[2] = ((byte << 4) & 0xF0) | (rs ? 0x0D : 0x0C); 
    frame[3] = ((byte << 4) & 0xF0) | (rs ? 0x09 : 0x08); 
    HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR, frame, 4, 100); 
} 
static inline void lcd_cmd(uint8_t cmd) { lcd_send_byte(cmd, 0); } static inline void lcd_data(uint8_t ch) { lcd_send_byte(ch, 1); } 
static void lcd_init(void) { 
    HAL_Delay(50); 
    lcd_send_nibble(0x30, 0); 
    HAL_Delay(5); 
    lcd_send_nibble(0x30, 0); 
    HAL_Delay(1); 
    lcd_send_nibble(0x30, 0); 
    HAL_Delay(1); 
    lcd_send_nibble(0x20, 0); 
    HAL_Delay(1); 
    lcd_cmd(0x28); 
    HAL_Delay(1); 
    lcd_cmd(0x0C); 
    HAL_Delay(1); 
    lcd_cmd(0x06); 
    HAL_Delay(1); 
    lcd_cmd(0x01); 
    HAL_Delay(2); 
} 
static void lcd_clear(void) { 
    lcd_cmd(0x01); 
    HAL_Delay(2); 
} 
static void lcd_set_cursor(uint8_t row, uint8_t col) { 
    static const uint8_t row_offsets[4] = {0x00, 0x40, 0x14, 0x54}; 
    lcd_cmd(0x80 | (row_offsets[row] + col)); 
} 
static void lcd_print(const char *str) { 
    while (*str) lcd_data((uint8_t)*str++); 
} 
static void lcd_print_amount(int centimes) { 
    char buf[12]; 
    snprintf(buf, sizeof(buf), "%d.%02d EUR", centimes / 100, centimes % 100); 
    lcd_print(buf); 
} 
/* --- Redirection printf --- */ int _write(int file, char *ptr, int len) { 
    (void)file; 
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY); 
    return len; 
} 
/* --- Initialisation matérielle F103 (Blue Pill) --- */ static void HARDWARE_INIT(void) { 
    HAL_Init(); 
    SystemClock_Config(); 

    __HAL_RCC_GPIOA_CLK_ENABLE(); 
    __HAL_RCC_GPIOB_CLK_ENABLE(); 
    __HAL_RCC_USART2_CLK_ENABLE(); 
    __HAL_RCC_I2C1_CLK_ENABLE(); 
    __HAL_RCC_AFIO_CLK_ENABLE(); 

    GPIO_InitTypeDef gpio = {0}; 

    // Boutons PA0 (Menu) et PA4 (Annulation)
    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_4; 
    gpio.Mode = GPIO_MODE_INPUT; 
    gpio.Pull = GPIO_PULLUP; 
    HAL_GPIO_Init(GPIOA, &gpio); 

    // LEDs (PA5, PA6, PA7) 
    gpio.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; 
    gpio.Mode = GPIO_MODE_OUTPUT_PP; 
    gpio.Speed = GPIO_SPEED_FREQ_LOW; 
    HAL_GPIO_Init(GPIOA, &gpio); 

    // UART2 TX (PA2) 
    gpio.Pin = GPIO_PIN_2; 
    gpio.Mode = GPIO_MODE_AF_PP; 
    gpio.Speed = GPIO_SPEED_FREQ_HIGH; 
    HAL_GPIO_Init(GPIOA, &gpio); 

    // UART2 RX (PA3) 
    gpio.Pin = GPIO_PIN_3; 
    gpio.Mode = GPIO_MODE_INPUT; 
    gpio.Pull = GPIO_PULLUP; // CHANGEMENT : Pull-up activé pour stabiliser la liaison UART Wokwi
    HAL_GPIO_Init(GPIOA, &gpio); 

    huart2.Instance = USART2; 
    huart2.Init.BaudRate = 115200; 
    huart2.Init.WordLength = UART_WORDLENGTH_8B; 
    huart2.Init.StopBits = UART_STOPBITS_1; 
    huart2.Init.Parity = UART_PARITY_NONE; 
    huart2.Init.Mode = UART_MODE_TX_RX; 
    HAL_UART_Init(&huart2); 

    // I2C1 (PB6=SCL, PB7=SDA) 
    gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7; 
    gpio.Mode = GPIO_MODE_AF_OD; // Spécifique STM32F1 : AF Open-Drain est correct
    gpio.Speed = GPIO_SPEED_FREQ_HIGH; 
    HAL_GPIO_Init(GPIOB, &gpio); 

    hi2c1.Instance = I2C1; 
    hi2c1.Init.ClockSpeed = 100000; 
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2; 
    hi2c1.Init.OwnAddress1 = 0; 
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; 
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; 
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; 
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; 
    
    // CORRECTION : Forcer la réinitialisation logicielle du bloc I2C pour Wokwi
    __HAL_RCC_I2C1_FORCE_RESET();
    HAL_Delay(10);
    __HAL_RCC_I2C1_RELEASE_RESET();

    HAL_I2C_Init(&hi2c1); 
} 
/* --- Fonctions d'Affichage et FSM --- */ static void display_welcome(void) { 
    lcd_clear(); 
    lcd_set_cursor(1, 2); 
    lcd_print("Machine Prete !"); 
    lcd_set_cursor(2, 1); 
    lcd_print("Appuyez sur Menu"); 
} 
static void display_menu(void) { 
    char line[LCD_COLS + 1]; 
    lcd_clear(); 
    lcd_set_cursor(0, 0); 
    lcd_print("=== DISTRIBUTEUR ==="); 
    snprintf(line, sizeof(line), "1.Cafe 2.00E Stk:%d", g_stockCafe); 
    lcd_set_cursor(1, 0); 
    lcd_print(line); 
    snprintf(line, sizeof(line), "2.The 1.50E Stk:%d", g_stockThe); 
    lcd_set_cursor(2, 0); 
    lcd_print(line); 
    snprintf(line, sizeof(line), "3.Jus 2.50E Stk:%d", g_stockJus); 
    lcd_set_cursor(3, 0); 
    lcd_print(line); 
} 
static void display_payment_prompt(void) { 
    lcd_clear(); 
    lcd_set_cursor(0, 0); 
    lcd_print("--- PAIEMENT ---"); 
    lcd_set_cursor(1, 0); 
    lcd_print("Prix : "); 
    lcd_print_amount(g_activePrice); 
    lcd_set_cursor(2, 0); 
    lcd_print("Insere: "); 
    lcd_print_amount(g_amount); 
} 
static void dispense_change(void) { 
    int change = g_amount - g_activePrice; 
    lcd_clear(); 
    lcd_set_cursor(0, 0); 
    lcd_print("Distribution..."); 
    if (change > 0) { 
        lcd_set_cursor(1, 0); 
        lcd_print("Monnaie: "); 
        lcd_print_amount(change); 
    } 
} 
static void refund_total(void) { 
    if (g_amount > 0) { 
        lcd_clear(); 
        lcd_set_cursor(0, 0); 
        lcd_print("-- REMBOURSEMENT --"); 
        lcd_set_cursor(1, 0); 
        lcd_print("Rendu : "); 
        lcd_print_amount(g_amount); 
        g_amount = 0; 
    } 
} 
static void give_item(void) { 
    switch (g_activeProduct) { 
        case 1: if (g_stockCafe > 0) g_stockCafe--; break; 
        case 2: if (g_stockThe > 0) g_stockThe--; break; 
        case 3: if (g_stockJus > 0) g_stockJus--; break; 
    } 
} 
static int get_active_stock(void) { 
    if (g_activeProduct == 1) return g_stockCafe; 
    if (g_activeProduct == 2) return g_stockThe; 
    if (g_activeProduct == 3) return g_stockJus; 
    return -1; 
} 
static int get_active_price(void) { 
    if (g_activeProduct == 1) return PRICE_CAFE; 
    if (g_activeProduct == 2) return PRICE_THE; 
    if (g_activeProduct == 3) return PRICE_JUS; 
    return 0; 
} 
static void update_leds(VendingState state) { 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (state == STATE_DISTRIBUTION) ? GPIO_PIN_SET : GPIO_PIN_RESET); 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (state == STATE_PAYMENT) ? GPIO_PIN_SET : GPIO_PIN_RESET); 
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (state == STATE_ERROR) ? GPIO_PIN_SET : GPIO_PIN_RESET); 
} 
static VendingEvent poll_inputs(void) { 
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) { 
        HAL_Delay(250); // Léger ajustement anti-rebond
        return EVENT_BTN_MENU; 
    } 
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET) { 
        HAL_Delay(250); 
        return EVENT_CANCEL; 
    } 
    uint8_t rx = 0; 
    if (HAL_UART_Receive(&huart2, &rx, 1, 0) == HAL_OK) { 
        switch (rx) { 
            case 'm': return EVENT_BTN_MENU; 
            case '1': g_activeProduct = 1; return EVENT_SELECT_ITEM; 
            case '2': g_activeProduct = 2; return EVENT_SELECT_ITEM; 
            case '3': g_activeProduct = 3; return EVENT_SELECT_ITEM; 
            case 'c': return EVENT_INSERT_COIN; 
            case 'a': return EVENT_CANCEL; 
            case 'e': return EVENT_INVALID_ACTION; 
            case 't': return EVENT_TIMEOUT_30S; 
            case 'f': return EVENT_TIMEOUT_5S; 
        } 
    } 
    return EVENT_NONE; 
} 
int main(void) { 
    HARDWARE_INIT(); 
    lcd_init(); 
    
    VendingState currentState = STATE_IDLE; 
    VendingEvent currentEvent = EVENT_NONE; 
    
    HAL_Delay(500); 
    display_welcome(); 
    printf("--- Systeme BluePill F1 Demarre ---\n"); 

    while (1) { 
        if (currentEvent == EVENT_NONE) 
            currentEvent = poll_inputs(); 
            
        update_leds(currentState); 

        switch (currentState) { 
            case STATE_IDLE: 
                if (currentEvent == EVENT_BTN_MENU) { 
                    display_menu(); 
                    currentState = STATE_SELECTION; 
                } 
                currentEvent = EVENT_NONE; // Toujours nettoyer à la sortie
                break; 

            case STATE_SELECTION: 
                switch (currentEvent) { 
                    case EVENT_SELECT_ITEM: 
                        if (get_active_stock() == 0) { 
                            lcd_clear(); 
                            lcd_set_cursor(1, 3); 
                            lcd_print("STOCK VIDE !"); 

currentState = STATE_ERROR;
} else {
g_activePrice = get_active_price();
display_payment_prompt();
currentState = STATE_PAYMENT;
}
break;
case EVENT_TIMEOUT_30S:
lcd_clear();
lcd_set_cursor(1, 2);
lcd_print("Delai depasse");
HAL_Delay(1500);
display_welcome();
currentState = STATE_IDLE;
break;
case EVENT_CANCEL:
currentState = STATE_IDLE;
display_welcome();
break;
default:
break;
}
currentEvent = EVENT_NONE;
break;
case STATE_PAYMENT:
switch (currentEvent) {
case EVENT_INSERT_COIN:
g_amount += 50;
display_payment_prompt();
if (g_amount >= g_activePrice) {
give_item();
dispense_change();
currentState = STATE_DISTRIBUTION;
}
break;
case EVENT_INVALID_ACTION:
lcd_clear();
lcd_set_cursor(1, 2);
lcd_print("Piece refusee !");
currentState = STATE_ERROR;
break;
case EVENT_CANCEL:
refund_total();
HAL_Delay(1500);
currentState = STATE_IDLE;
display_welcome();
break;
default:
break;
}
currentEvent = EVENT_NONE;
break;
case STATE_DISTRIBUTION:
if (currentEvent == EVENT_TIMEOUT_5S) {
g_amount = 0;
g_activeProduct = 0;
g_activePrice = 0;
lcd_clear();
lcd_set_cursor(1, 1);
lcd_print("Bonne degustation!");
HAL_Delay(2000);
display_welcome();
currentState = STATE_IDLE;
}
currentEvent = EVENT_NONE;
break;
case STATE_ERROR:
if (currentEvent == EVENT_BTN_MENU || currentEvent == EVENT_CANCEL || currentEvent == EVENT_TIMEOUT_30S || currentEvent == EVENT_TIMEOUT_5S) {
lcd_clear();
display_welcome();
currentState = STATE_IDLE;
}
currentEvent = EVENT_NONE;
break;
}
}
}
/* --- Horloge Système F1 (64 MHz) --- */
void SystemClock_Config(void) {
RCC_OscInitTypeDef osc = {0};
RCC_ClkInitTypeDef clk = {0};
osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
osc.HSIState = RCC_HSI_ON;
osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
osc.PLL.PLLState = RCC_PLL_ON;
osc.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
osc.PLL.PLLMUL = RCC_PLL_MUL16;
HAL_RCC_OscConfig(&osc);
clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
clk.APB1CLKDivider = RCC_HCLK_DIV2;
clk.APB2CLKDivider = RCC_HCLK_DIV1;
HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2);
}