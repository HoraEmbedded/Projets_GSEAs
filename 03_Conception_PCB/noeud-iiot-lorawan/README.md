# Nœud capteur IIoT ultra-basse consommation — LoRaWAN & energy harvesting

> Conception électronique & IIoT · **projet en cours**
> Nœud capteur autonome LoRaWAN visant < 15 µA de courant moyen et 10 ans d'autonomie sans maintenance batterie.

## Problème

Un capteur environnemental déployé en extérieur ne doit jamais exiger de changement de pile : l'enjeu est un **budget énergétique** tenu à la dizaine de µA près, du composant au protocole.

## Architecture

| Bloc | Composant | Justification |
|---|---|---|
| MCU | STM32U0 | Stop 2 + RTC : ~1 µA en veille |
| Radio | SX1262 | TX +14 dBm à 45 mA (réglages optimaux, DC-DC activé) |
| Capteur | SHT4x | Mesure T/HR en 7 ms à 0,32 mA |
| Protocole | LoRaWAN Class A | Le nœud ne maintient aucune connexion : il émet puis se rendort |

## Budget énergétique (cycle de 1 h, séance 2)

| État | Courant | Durée | Charge |
|---|---|---|---|
| Sleep (Stop 2 + RTC) | 0,985 µA | 3 598,7 s | 3,545 mA·s |
| Réveil | 1,35 mA | 50 ms | 0,068 mA·s |
| Mesure | 0,32 mA | 7 ms | 0,002 mA·s |
| TX LoRa | 45 mA | 1,155 s | 51,975 mA·s |
| RX (2 fenêtres) | 4,6 mA | 100 ms | 0,460 mA·s |

**I_moy = 15,57 µA** — hors gabarit de +0,57 µA : le TX pèse 92,7 % du budget. Optimisation en cours (SF/ADR, payload, période d'uplink).

## Contenu du dépôt

```
├── PCB_design/     # Conception de la carte (en cours)
├── journal.md      # Journal de conception : budget énergétique, choix, calculs
└── README.md
```

## État d'avancement

- [x] Cadrage, états de consommation, budget énergétique initial
- [ ] Optimisation du budget sous les 15 µA
- [ ] Dimensionnement de la récupération d'énergie
- [ ] Schéma et routage PCB

## Auteur

**AZONHOUMON H. Horacia Gloriéta** ([@HoraEmbedded](https://github.com/HoraEmbedded)) · [Portfolio](https://hora-portfolio.vercel.app/)
