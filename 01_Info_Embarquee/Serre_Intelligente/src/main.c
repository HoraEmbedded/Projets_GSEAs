#include <avr/io.h>
#include <util/delay.h>

// Configuration UART (Baud rate : 9600)
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

int main(void) {
    // 1. Initialisation de la communication UART
    uart_init();
    
    // 2. Configuration des broches pour les LEDs (Relais)
    // DDRH configure la direction. L'opérateur "|=" met les bits PH5 et PH6 à 1 (OUTPUT)
    DDRH |= (1 << PH5) | (1 << PH6); 

    while (1) {
        uart_send_string("Actionneurs ON\r\n");
        
        // Allumer les LEDs (Mise à 1 des bits dans PORTH)
        PORTH |= (1 << PH5) | (1 << PH6);
        _delay_ms(1000);
        
        uart_send_string("Actionneurs OFF\r\n");
        
        // Éteindre les LEDs (Mise à 0 via l'opérateur ET binaire sur l'inverse des bits)
        PORTH &= ~((1 << PH5) | (1 << PH6));
        _delay_ms(1000);
    }
    
    return 0;
}