# Régulation de température d'un four industriel : identification FOPDT & réglage PID

> Automatique · MATLAB
> Identification d'un procédé thermique FOPDT et comparaison de trois méthodes de réglage PID pour tenir une consigne de 850 °C.

## Problème

Réguler la température d'un four de traitement thermique : procédé lent (constante de temps 50 s) avec retard pur (12 s) — la configuration qui met les PID en difficulté — et robustesse exigée face aux perturbations (ouverture de porte).

## Démarche

1. **Identification en boucle ouverte** (échelon de 10 % de puissance, méthode de la tangente / Broida) :

   G(s) = 1,5 · e^(−12s) / (50s + 1)

2. **Réglage PID par trois méthodes** : Ziegler-Nichols, Cohen-Coon, PID Tuner MATLAB.
3. **Comparaison** : dépassement, temps de montée, erreur statique, rejet de perturbation.

## Contenu du dépôt

```
├── process_identification.m   # Identification FOPDT (K, L, τ)
├── pid_tuning_zn.m            # Réglage Ziegler-Nichols
├── pid_tuning_cc.m            # Réglage Cohen-Coon
├── pid_tuning_app.m           # Réglage PID Tuner
├── project_comparison.m       # Comparaison des trois régulateurs
└── README.md
```

## Résultats

| Paramètre identifié | Valeur |
|---|---|
| Gain statique K | 1,5 °C/% |
| Retard pur L | 12 s |
| Constante de temps τ | 50 s |

Comparaison complète des trois réglages dans `project_comparison.m` (courbes superposées, critères chiffrés).

## Environnement

`MATLAB` · `Control System Toolbox`

## Auteur

**AZONHOUMON H. Horacia Gloriéta** ([@HoraEmbedded](https://github.com/HoraEmbedded)) · [Portfolio](https://hora-portfolio.vercel.app/)
