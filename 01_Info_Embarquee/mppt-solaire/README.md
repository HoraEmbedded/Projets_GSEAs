# Régulateur de charge solaire MPPT : buck synchrone

> Électronique de puissance & embarqué · **projet en cours**
> Conception d'un régulateur de charge solaire à recherche du point de puissance maximale (MPPT), du convertisseur au firmware.

## Problème

Charger une batterie 12 V (13 – 14,4 V) depuis un panneau 100 W (~18 V au point optimal) sans dissiper la différence : une régulation linéaire perdrait ~25 W en chaleur. Il faut convertir sans dissiper, puis piloter le point de fonctionnement du panneau en temps réel.

## Approche (en cours)

1. **Convertisseur buck synchrone** : cellule de commutation MOSFET + filtre LC, rapport cyclique D pilotant `Vout = D × Vin` ; second MOSFET en remplacement de la diode de roue libre pour tenir l'objectif de rendement > 95 %.
2. **Dimensionnement et simulation LTspice** : ondulations de courant (ΔI_L) et de tension, arbitrage fréquence de découpage / pertes de commutation, prédiction systématique avant chaque simulation.
3. **Firmware MPPT** (à venir) : mesure V/I panneau, algorithme perturb & observe pilotant D, échantillonnage synchronisé sur le cycle de découpage.

## État d'avancement

- [x] Fondamentaux de la cellule de commutation (leçons + carnet)
- [x] Buck simulé sous LTspice, prédictions vérifiées
- [ ] Dimensionnement final (L, C, fréquence)
- [ ] Buck synchrone + drivers
- [ ] Algorithme MPPT embarqué

## Environnement

`LTspice` · `Électronique de puissance` · `C (firmware à venir)`

## Auteur

**AZONHOUMON H. Horacia Gloriéta** ([@HoraEmbedded](https://github.com/HoraEmbedded)) · [Portfolio](https://hora-portfolio.vercel.app/)
