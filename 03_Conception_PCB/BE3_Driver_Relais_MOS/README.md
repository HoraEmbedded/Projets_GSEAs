#  Interface de Commande de Puissance (Driver MOS)

## Description du projet
Ce projet a été réalisé dans le cadre du Bureau d'Étude N°3 d'Électronique de Puissance (ENSAT). Il consiste à analyser, dimensionner et concevoir une interface de commande basée sur un transistor MOS en mode commutation[cite: 19]. 
L'objectif est de piloter de manière fiable six relais électromagnétiques (RT42405) depuis un microcontrôleur 3.3V (courant max 20 mA)[cite: 20].

## Technologies et Outils
* **Conception PCB :** KiCad
* **Simulation & Layout Intégré :** Pspice, Microwind 
* **Composants majeurs :** * Transistor NMOS Logic-Level (IRLML2502) en boîtier SOT-23 [cite: 57, 66]
  * Isolation par Optocoupleur (PC817) [cite: 123]
  * Diode de Roue Libre Schottky (1N5819HW) [cite: 78]

##  Structure du dépôt
* `/Docs` : Rapport complet de l'étude (Calculs théoriques, dimensionnement thermique, industrialisation)[cite: 11, 103, 129].
* `/Hardware` : Fichiers sources KiCad (Schématique et Routage).
* `/Simulation` : Fichiers de validation.

## Caractéristiques techniques
* [cite_start]**Tension de commande :** 3.3 V [cite: 30]
* [cite_start]**Tension de puissance :** 5 V (Bobine relais) [cite: 64, 84]
* [cite_start]**Courant nominal :** ~80 mA par canal [cite: 64]
* [cite_start]**Protection :** Isolation galvanique totale de l'étage logique [cite: 124]