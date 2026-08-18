# Suivi de trajectoire d'un robot mobile par vision artificielle

> Automatique & vision · ENSA Tanger, GSEA · **projet en cours**
> Système complet de suivi de chemin pour robot mobile différentiel, guidé par traitement d'image — entièrement simulé sous MATLAB/Simulink et Python.

## Problème

Faire suivre à un robot mobile différentiel un chemin tracé au sol, détecté par vision artificielle, sans capteur de position dédié : la caméra est le seul capteur de guidage.

## Approche (en cours)

1. **Vision** : génération d'images de test et extraction du chemin par traitement d'image (Python / OpenCV).
2. **Trajectoire** : conversion du chemin détecté en consigne de trajectoire exploitable par le contrôleur.
3. **Commande** : modèle Simulink du robot différentiel et boucle de suivi de trajectoire.

## Contenu du dépôt

```
├── create_test_image.py     # Génération d'images de chemin pour les tests
├── images/                  # Images de test et résultats
└── README.md
```

## Environnement

`Python` · `OpenCV` · `MATLAB / Simulink`

## État d'avancement

- [x] Génération d'images de test
- [ ] Extraction du chemin par vision
- [ ] Modèle Simulink du robot différentiel
- [ ] Boucle de suivi complète et résultats mesurés

## Auteur

**AZONHOUMON H. Horacia Gloriéta** ([@HoraEmbedded](https://github.com/HoraEmbedded)) · [Portfolio](https://hora-portfolio.vercel.app/)
