#include <avr/io.h>
#include <util/delay.h>

// Configuration de la vitesse de communication (Baud rate)
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

// Fonction d'initialisation de l'UART0
void uart_init() {
    // Définition du baud rate dans les registres (High et Low)
    UBRR0H = (BRC >> 8);
    UBRR0L = BRC;
    
    // Activation de la transmission (TX) et de la réception (RX)
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    
    // Configuration du format de la trame : 8 bits de données, 1 bit de stop
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Fonction pour envoyer un seul caractère
void uart_send_char(char c) {
    // Attendre que le buffer de transmission soit vide
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c; // Placer la donnée dans le registre
}

// Fonction pour envoyer une chaîne de caractères
void uart_send_string(const char* str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

int main(void) {
    // Initialisation
    uart_init();
    
    // Boucle infinie (équivalent de la fonction loop() d'Arduino)
    while (1) {
        uart_send_string("Systeme Serre Intelligente OK - Test UART\r\n");
        _delay_ms(1000); // Pause de 1 seconde
    }
    
    return 0;
}