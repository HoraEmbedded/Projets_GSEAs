
**Cadrage.** Projet : nœud LoRaWAN autonome sur energy harvesting, cible < 15 µA moy., 10 ans d'autonomie. Enjeu business : supprimer la maintenance batterie (OPEX). Cœur technique identifié : le **budget énergétique**. À faire : auto-évaluation de niveau (envoyée au mentor).


> **Mini-leçon LoRaWAN.** Famille **LPWAN** (basse conso, longue portée, faible débit) — comble le trou que Wi-Fi/BLE/4G ne couvrent pas. **Triangle radio** : portée × débit × conso, on n'en a que 2 → LoRaWAN choisit portée + conso. Distinction clé : **LoRa** = couche physique (modulation CSS, la radio) ; **LoRaWAN** = protocole réseau au-dessus. Architecture en **étoile** : nœud → passerelle → network server → app ; chiffrement AES bout-en-bout, le nœud **ne maintient aucune connexion** (crie et se rendort → énorme gain d'énergie). Liens : **Class A** = uplink puis 2 fenêtres RX ; **SF** = curseur portée/débit (SF12 → payload max 51 o) ; **duty cycle ≤ 1 %** imposé par l'ETSI car fréquence partagée.

Séance 1 — Mission & états. Période uplink = 1 h, justifiée par la dynamique lente du sol (la fréquence suit la physique du phénomène, pas le feeling). Trame : ~12 octets et non 64 → leçon LoRaWAN : payload max = 51 o à SF10-12, et payload → airtime → énergie (chaque octet se paie 10 ans). États de conso = Sleep · Réveil · Mesure · TX · RX (oubli initial : Mesure ; nouveau : fenêtres RX obligatoires en Class A). Puissance = vitesse / Énergie = distance. Outil découvert : airtime calculator TTN. Prochain : construire le tableau du budget énergétique (Séance 2).

# Journal de Projet - Séance 2 : Bilan Énergétique Initial

## 1. Méthode et Formule Maîtresse

Le courant moyen ($I_{moy}$) est calculé sur un cycle complet d'une heure ($T_{cycle} = 3600\text{ s}$). La monnaie commune utilisée pour consolider les données sans erreur d'unité est le **mA·s** (milliampère-seconde).

$$\text{Charge } Q = I \times t$$

$$I_{moy} = \frac{\sum Q_i}{T_{cycle}} = \frac{Q_{total}\text{ [mA·s]}}{3600\text{ [s]}} \times 1000\text{ } [\mu A]$$

## 2. Tableau du Budget Énergétique (Cycle de 1 heure)

| État | Courant $I$ | Durée $t$ (par cycle) | Charge $Q = I \times t$ | Source & Conditions Datasheet |
| --- | --- | --- | --- | --- |
| **Sleep** | 0,000985 mA | 3598,688 s | 3,545 mA·s | STM32U0 (Stop 2 + RTC, 3.0V, 25°C) |
| **Réveil** | 1,350000 mA | 0,050 s | 0,068 mA·s | STM32U0 (Run 16MHz, 3.0V, 25°C) |
| **Mesure** | 0,320000 mA | 0,007 s | 0,002 mA·s | SHT4x (Mesure, High repeatability) |
| **TX** | 45,000000 mA | 1,155 s | 51,975 mA·s | SX1262 (TX +14 dBm optimal, 868MHz) |
| **RX (2 fen.)** | 4,600000 mA | 0,100 s | 0,460 mA·s | SX1262 (RX LoRa 125kHz, DC-DC) |
| **Total** | — | **3600,000 s** | **56,050 mA·s** | — |

## 3. Calcul du Courant Moyen Final

$$I_{moy} = \frac{56,050\text{ mA·s}}{3600\text{ s}} \times 1000 = \mathbf{15,57\ \mu A}$$

* **Cible du projet :** < 15 µA
* **Statut :** **Hors-gabarit** (+0,57 µA)

## 4. Notes d'Analyse & Constats Clés

1. **Domination absolue du TX :** La phase d'émission (TX) représente à elle seule **92,7 %** de la consommation totale d'énergie du cycle ($51,975\text{ mA·s}$ sur $56,050\text{ mA·s}$). C'est le point critique sur lequel faire peser les efforts d'optimisation.
2. **Rigueur du temps de Sleep :** Le temps passé en mode Sleep est précisément de $3600 - (0,05 + 0,007 + 1,155 + 0,1) = 3598,688\text{ s}$. Cette soustraction rigoureuse valide l'intégrité de la base de temps du cycle complet.
3. **Sensibilité de l'architecture matérielle :** * Le choix des réglages optimaux (*optimal settings*) sur le SX1262 maintient le courant de TX à 45 mA au lieu des 90 mA par défaut.
* L'activation du convertisseur continu-continu (DC-DC) de la puce radio contient le courant de RX à 4,6 mA (au lieu de 8,8 mA en mode LDO).


4. **Plan d'action pour la Séance 3 :** La cible des 15 µA étant dépassée, il est nécessaire d'actionner des leviers logiciels ou protocolaires : réduction du Spreading Factor (SF) via le mécanisme d'ADR si la qualité du lien le permet, optimisation fine de la taille de la charge utile (payload), ou allongement de la période de transmission (par exemple, passage à un uplink toutes les 2 heures).