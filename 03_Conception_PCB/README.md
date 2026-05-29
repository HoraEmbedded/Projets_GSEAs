#  Interface de Commande de Puissance (Driver MOS)

![Vue 3D de la carte de puissance](./Pictures/vue_3D.png)

## Description du projet
Ce projet a été réalisé dans le cadre du Bureau d'Étude N°3 d'Électronique de Puissance (ENSAT). Il consiste à analyser, dimensionner et concevoir une interface de commande basée sur un transistor MOS en mode commutation. 
L'objectif est de piloter de manière fiable six relais électromagnétiques (RT42405) depuis un microcontrôleur 3.3V (courant max 20 mA).

## Technologies et Outils
* **Conception PCB :** KiCad
* **Simulation & Layout Intégré :** Pspice, Microwind 
* **Composants majeurs :** * Transistor NMOS Logic-Level (IRLML2502) en boîtier SOT-23
  * Isolation par Optocoupleur (PC817) 
  * Diode de Roue Libre Schottky (1N5819HW)

##  Structure du dépôt
* `/Docs` : Rapport complet de l'étude (Calculs théoriques, dimensionnement thermique, industrialisation)
* `/Hardware` : Fichiers sources KiCad (Schématique et Routage).

## Caractéristiques techniques
* **Tension de commande :** 3.3 V 
* **Tension de puissance :** 5 V (Bobine relais)
* **Courant nominal :** ~80 mA par canal 
* **Protection :** Isolation galvanique totale de l'étage logique