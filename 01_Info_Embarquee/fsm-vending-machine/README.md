# Distributeur automatique : FSM sur microcontrôleur (simulation Wokwi)

> Informatique embarquée · ENSA Tanger, GSEA · C / PlatformIO
> Machine à états finis d'un distributeur de boissons, exécutée sur microcontrôleur et validée en simulation.

## Problème

Implémenter la logique de commande d'un distributeur (IDLE → SELECTION → PAYMENT → DISTRIBUTION → ERROR) sur une cible embarquée réelle, avec timeouts non bloquants — en complément du projet [fsm-making](../FSM_Making) qui compare les architectures FSM et les valide par tests unitaires.

## Contenu du dépôt

```
├── src/            # Sources C de la FSM
├── include/        # En-têtes
├── test/           # Tests
├── diagram.json    # Circuit de simulation Wokwi
├── wokwi.toml      # Configuration Wokwi
└── README.md
```

## Utilisation

```bash
pio run                 # compile le firmware
```
Puis importer `diagram.json` dans Wokwi et charger le firmware compilé.

## Environnement

`C` · `PlatformIO` · `Wokwi`

## Auteur

**AZONHOUMON H. Horacia Gloriéta** ([@HoraEmbedded](https://github.com/HoraEmbedded)) · [Portfolio](https://hora-portfolio.vercel.app/)
