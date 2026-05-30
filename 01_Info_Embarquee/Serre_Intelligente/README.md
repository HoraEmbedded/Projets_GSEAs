
Voici le code complet pour ton fichier `README.md`. Il est rédigé selon les standards de l'industrie (clair, structuré, purement technique) pour mettre en valeur la rigueur de ton architecture logicielle.

Tu peux copier l'intégralité de ce bloc et le coller dans ton dépôt GitHub.

```markdown
# Système Embarqué de Gestion de Serre Intelligente

## Description du Projet
Ce projet consiste en la conception et l'implémentation d'un système de contrôle automatisé pour une serre agricole. Le micrologiciel est développé intégralement en C pur (approche *bare-metal*) pour un microcontrôleur ATmega2560, sans utilisation de bibliothèques de haut niveau (comme celles de l'écosystème Arduino standard) ni de système d'exploitation temps réel (RTOS). 

L'objectif est d'assurer une régulation autonome de la température ambiante et de l'humidité du sol via un contrôle par hystérésis, tout en garantissant un fonctionnement non-bloquant grâce à une architecture logicielle pilotée par les interruptions (*Interrupt-Driven*).

## Fonctionnalités Principales
* **Acquisition de données capteurs :** Lecture de la température et de l'humidité de l'air (protocole 1-Wire) et de l'humidité du sol (conversion analogique-numérique).
* **Régulation par hystérésis :** Contrôle de deux actionneurs (ventilateur et pompe) pour éviter les oscillations d'état.
* **Architecture Non-Bloquante :** Utilisation du Timer 1 en mode CTC pour cadencer le système toutes les 2 secondes sans figer le processeur principal.
* **Interface Homme-Machine (IHM) :** Affichage en temps réel des métriques sur un écran LCD 16x2 via un bus I2C logiciel.
* **Télémétrie :** Transmission des logs de fonctionnement et des mesures via l'interface série UART vers un terminal externe.

## Architecture Matérielle
* **Microcontrôleur :** Arduino Mega 2560 (ATmega2560 à 16 MHz).
* **Capteurs :** DHT22 (Air) et Potentiomètre 10 kΩ (simulation de la sonde d'humidité du sol).
* **Actionneurs :** Modules relais 5V pour le pilotage de la puissance (Ventilation et Pompe).
* **Affichage :** Écran LCD 16x2 avec module contrôleur I2C (PCF8574).

### Plan de Câblage (Pinout)
| Composant | Rôle | Connexion ATmega2560 | Registre / Port |
| :--- | :--- | :--- | :--- |
| **DHT22** | Données (1-Wire) | Pin 2 | PE4 |
| **Sonde Sol** | Signal Analogique | A0 | PF0 (ADC0) |
| **Relais 1** | Commande Ventilateur | Pin 9 | PH6 |
| **Relais 2** | Commande Pompe | Pin 8 | PH5 |
| **Module I2C** | SDA (Données) | Pin 20 | PD0 |
| **Module I2C** | SCL (Horloge) | Pin 21 | PD1 |

## Architecture Logicielle (Pilotes Bare-Metal)
Le projet est structuré autour de pilotes développés spécifiquement par manipulation directe des registres :
1. **UART :** Configuration des registres `UBRR0` et `UCSR0` pour une communication asynchrone à 9600 bauds.
2. **I2C (TWI) :** Implémentation de la machine d'état I2C matérielle à 100 kHz via le registre `TWCR`.
3. **ADC :** Configuration de l'échantillonnage avec un facteur de division de 128 via `ADCSRA`.
4. **Timer 1 :** Configuration en mode CTC (Clear Timer on Compare) générant une interruption via l'ISR `TIMER1_COMPA_vect`.

## Installation et Compilation

### Prérequis
* Un environnement de développement compatible avec le framework AVR (ex: **PlatformIO** sous VS Code ou AVR-GCC en ligne de commande).

### Instructions (PlatformIO)
1. Cloner le dépôt :
   ```bash
   git clone <url-du-depot>
   cd <nom-du-dossier>

```

2. Construire le projet (compilation du firmware) :
```bash
pio run

```


3. Téléverser le firmware sur l'ATmega2560 :
```bash
pio run --target upload

```


4. Ouvrir le moniteur série (9600 bauds) pour observer la télémétrie :
```bash
pio device monitor -b 9600

```



## Auteur

*Projet réalisé dans le cadre d'un cursus en ingénierie des systèmes embarqués.*

```

```
