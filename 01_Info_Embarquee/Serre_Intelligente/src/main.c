#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h> // Nécessaire pour la fonction sprintf()

// --- UART ---
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

void uart_init() {
    UBRR0H = (BRC >> 8); UBRR0L = BRC;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}
void uart_send_string(const char* str) {
    while (*str) uart_send_char(*str++);
}

// --- I2C & LCD ---
#define LCD_ADDR 0x27

void i2c_init() {
    PORTD |= (1 << PD0) | (1 << PD1);
    TWSR = 0x00; TWBR = 72; TWCR = (1 << TWEN); 
}
void i2c_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); 
}
void i2c_stop() { TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); }
void i2c_write(uint8_t data) {
    TWDR = data; TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}
void lcd_send_nibble(uint8_t half_byte, uint8_t mode) {
    uint8_t data = half_byte | mode | 0x08; 
    i2c_start(); i2c_write(LCD_ADDR << 1);
    i2c_write(data | 0x04); i2c_write(data & ~0x04); i2c_stop();
    _delay_us(100);
}
void lcd_send(uint8_t value, uint8_t mode) {
    lcd_send_nibble(value & 0xF0, mode);
    lcd_send_nibble((value << 4) & 0xF0, mode);
}
void lcd_init() {
    _delay_ms(50);
    lcd_send_nibble(0x30, 0); _delay_ms(5);
    lcd_send_nibble(0x30, 0); _delay_us(150);
    lcd_send_nibble(0x30, 0); lcd_send_nibble(0x20, 0); 
    lcd_send(0x28, 0); lcd_send(0x0C, 0); lcd_send(0x01, 0); _delay_ms(2);
}
void lcd_print(const char* str) {
    while (*str) lcd_send(*str++, 1);
}
void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = { 0x00, 0x40 };
    lcd_send(0x80 | (col + row_offsets[row]), 0);
}

// --- DHT22 (Broche 2 = PE4) ---
#define DHT_PIN PE4
uint8_t dht_data[5];

int dht_read() {
    uint8_t i, j;
    volatile uint16_t timeout; // Empêche l'optimisation du compilateur
    
    // 1. Start signal
    DDRE |= (1 << DHT_PIN);
    PORTE &= ~(1 << DHT_PIN);
    _delay_ms(20);
    PORTE |= (1 << DHT_PIN);
    _delay_us(30);
    
    // 2. Mode entrée
    DDRE &= ~(1 << DHT_PIN);
    
    // 3. Attente Ack
    timeout = 10000; while(PINE & (1 << DHT_PIN)) { if(--timeout==0) return -1; }
    timeout = 10000; while(!(PINE & (1 << DHT_PIN))) { if(--timeout==0) return -2; }
    timeout = 10000; while(PINE & (1 << DHT_PIN)) { if(--timeout==0) return -3; }
    
    // 4. Lecture bits
    for (i = 0; i < 5; i++) {
        dht_data[i] = 0;
        for (j = 0; j < 8; j++) {
            timeout = 10000; while(!(PINE & (1 << DHT_PIN))) { if(--timeout==0) return -4; }
            _delay_us(40); 
            if (PINE & (1 << DHT_PIN)) {
                dht_data[i] |= (1 << (7 - j));
                timeout = 10000; while(PINE & (1 << DHT_PIN)) { if(--timeout==0) return -5; }
            }
        }
    }
    
    // 5. Checksum
    if (dht_data[4] == ((dht_data[0] + dht_data[1] + dht_data[2] + dht_data[3]) & 0xFF)) return 0;
    return -6;
}

// --- Fonction Principale ---
int main(void) {
    uart_init(); 
    i2c_init(); 
    lcd_init();
    
    DDRH |= (1 << PH5) | (1 << PH6); // Broches LEDs en sortie
    
    char buffer_lcd[16];
    int temperature, humidite, etat_dht;

    lcd_print("Redemarrage...");
    _delay_ms(2000); // Temps de stabilisation nécessaire pour le capteur DHT22

    while (1) {
        etat_dht = dht_read();
        
        lcd_send(0x01, 0); // Nettoie l'écran LCD
        _delay_ms(2);
        
        if (etat_dht == 0) {
            // Lecture réussie : on calcule les vraies valeurs
            humidite = (dht_data[0] << 8) | dht_data[1];
            temperature = ((dht_data[2] & 0x7F) << 8) | dht_data[3];
            
            lcd_set_cursor(0, 0);
            sprintf(buffer_lcd, "Temp: %d.%d C", temperature/10, temperature%10);
            lcd_print(buffer_lcd);
            
            lcd_set_cursor(0, 1);
            sprintf(buffer_lcd, "Hum : %d.%d %%", humidite/10, humidite%10);
            lcd_print(buffer_lcd);
            
            uart_send_string("Lecture capteur : OK\r\n");
        } else {
            // Échec de lecture : on affiche l'erreur
            lcd_set_cursor(0, 0);
            sprintf(buffer_lcd, "Err DHT: %d", etat_dht);
            lcd_print(buffer_lcd);
            
            uart_send_string("Erreur DHT22\r\n");
        }
        
        // Clignotement "Heartbeat" (témoin d'activité)
        PORTH |= (1 << PH5) | (1 << PH6);
        _delay_ms(50);
        PORTH &= ~((1 << PH5) | (1 << PH6));
        
        // Le DHT22 nécessite un temps de repos entre chaque lecture
        _delay_ms(1950); 
    }
    return 0;
}