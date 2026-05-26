#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// --- UART ---
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

void uart_init() {
    UBRR0H = (BRC >> 8);
    UBRR0L = BRC;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_send_string(const char* str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

// --- I2C (TWI) ---
void i2c_init() {
    // Activation des résistances de pull-up internes sur SDA (PD1) et SCL (PD0)
    PORTD |= (1 << PD0) | (1 << PD1);
    
    TWSR = 0x00; 
    TWBR = 72;   
    TWCR = (1 << TWEN); 
}

void i2c_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); 
}

void i2c_stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

// --- LCD 1602 I2C ---
#define LCD_ADDR 0x27

// NOUVEAU : Envoi d'un demi-octet (vital pour la séquence d'initialisation)
void lcd_send_nibble(uint8_t half_byte, uint8_t mode) {
    uint8_t data = half_byte | mode | 0x08; // 0x08 = Rétroéclairage ON
    i2c_start();
    i2c_write(LCD_ADDR << 1);
    i2c_write(data | 0x04); // Impulsion EN (Enable) = 1
    i2c_write(data & ~0x04); // Impulsion EN = 0
    i2c_stop();
    _delay_us(100);
}

void lcd_send(uint8_t value, uint8_t mode) {
    // Découpage en deux demi-octets (Poids fort, puis poids faible)
    lcd_send_nibble(value & 0xF0, mode);
    lcd_send_nibble((value << 4) & 0xF0, mode);
}

void lcd_init() {
    _delay_ms(50); // Attente de la montée en tension
    
    // Séquence stricte d'initialisation du HD44780 en mode 4-bits
    lcd_send_nibble(0x30, 0); _delay_ms(5);
    lcd_send_nibble(0x30, 0); _delay_us(150);
    lcd_send_nibble(0x30, 0);
    lcd_send_nibble(0x20, 0); // Passage officiel en mode 4 bits
    
    // À partir d'ici, on peut envoyer des octets complets
    lcd_send(0x28, 0); // 2 lignes, matrice 5x8
    lcd_send(0x0C, 0); // Écran ON, curseur OFF
    lcd_send(0x01, 0); // Nettoyage de l'écran
    _delay_ms(2);
}

void lcd_print(const char* str) {
    while (*str) {
        lcd_send(*str++, 1);
    }
}

// --- Fonction Principale ---
int main(void) {
    uart_init();
    i2c_init();
    lcd_init();
    
    // Configuration des LEDs (Broches 8 et 9)
    DDRH |= (1 << PH5) | (1 << PH6); 
    
    // Test d'affichage
    lcd_print("Serre Wokwi OK");
    uart_send_string("Initialisation des sous-systemes terminee.\r\n");

    while (1) {
        PORTH |= (1 << PH5) | (1 << PH6); // LEDs ON
        _delay_ms(1000);
        
        PORTH &= ~((1 << PH5) | (1 << PH6)); // LEDs OFF
        _delay_ms(1000);
    }
    
    return 0;
}