# Commande floue temps réel d'un robot 2 DDL sur DSP TMS320C6713

> Commande temps réel · C ANSI sur cible · validation Hardware-in-the-Loop

Régulateur flou à 25 règles pilotant un modèle dynamique de bras robotique à
deux degrés de liberté, le tout exécuté sur DSP TMS320C6713. Le robot est
simulé sur la cible elle-même (pas de banc mécanique) : la boucle
consigne → erreur → inférence floue → couple → dynamique → intégration tourne
en temps réel sur le DSP, et les positions articulaires sortent en analogique
par le codec AIC23 pour être relevées à l'oscilloscope.

Aucun code généré automatiquement : l'ensemble est écrit en C ANSI, y compris le
moteur d'inférence flou et l'inversion de la matrice d'inertie.

## Ce que fait le programme

| Étage | Implémentation |
| --- | --- |
| Consigne | Sinusoïde 0,5 Hz, ±30° sur l'articulation 1, ±15° (cosinus) sur l'articulation 2 |
| Erreur | `e = ref - q`, dérivée par différence finie sur la période d'échantillonnage |
| Contrôleur | Inférence floue Mamdani, 2 entrées × 5 ensembles, 25 règles, défuzzification par barycentre |
| Actionneur | Couple saturé à ±50 N·m sur chaque articulation |
| Modèle | Dynamique Euler-Lagrange 2 DDL : inertie H(q), Coriolis/centrifuge V(q,q̇), gravité G(q) |
| Intégration | Euler explicite, pas de 125 µs |
| Sortie | Deux canaux 16 bits fusionnés en un mot 32 bits, écrits sur le codec AIC23 (8 kHz) |

Période de boucle : **125 µs** (8 kHz), imposée par la fréquence d'échantillonnage
du codec. La boucle est cadencée par l'attente sur `DSK6713_AIC23_write`.

## Contrôleur flou

Deux entrées normalisées puis saturées sur [-1, 1] : l'erreur et sa dérivée,
chacune décomposée en cinq ensembles (NB, NE, ZE, PO, PB) par fonctions
triangulaires et trapézoïdales écrites à la main (`trimf`, `trapmf`).

La base de règles est une matrice 5×5, soit **25 règles**, dont la sortie pointe
vers sept singletons répartis sur [-1, 1]. Agrégation par minimum, puis
défuzzification par barycentre pondéré.

Gains de normalisation, réglés par articulation :

| Articulation | K_E | K_DE | K_U |
| --- | --- | --- | --- |
| 1 (épaule) | 9,55 | 0,95 | 40,0 |
| 2 (coude) | 7,96 | 0,16 | 10,0 |

## Modèle dynamique

Bras plan à deux segments, paramètres dans `robot_model.c` : masses 1 kg,
longueurs 1 m, centres de masse à 0,4 m, inerties 0,05 kg·m².

L'accélération est obtenue par inversion analytique de la matrice d'inertie 2×2
(déterminant explicite, avec garde à 1e-9 contre la singularité), ce qui évite
toute routine d'algèbre linéaire sur la cible.

## Arborescence

```
dsp-commande-floue-c6713/
├── main.c                  Boucle temps réel, consigne, saturation, sortie codec
├── fuzzy_controller.c      Fonctions d'appartenance, base de règles, défuzzification
├── robot_model.c           Dynamique Euler-Lagrange 2 DDL
├── C6713.cmd               Fichier de commande du linker (mapping mémoire IRAM)
├── .ccsproject             Projet Code Composer Studio 7.4 · TMS320C6713 · little endian
├── .cproject / .project    Configuration de build Eclipse CDT
└── docs/
    ├── rapport-implementation-c-fr.pdf
    ├── dsp-lab.pdf
    └── tp2-dsp.pdf
```

## Reproduire

Prérequis : Code Composer Studio 7.4 ou ultérieur, carte DSK6713 et sa
bibliothèque support (`dsk6713.h`, `dsk6713_aic23.h`), sonde JTAG.

1. Importer le dossier comme projet existant dans CCS (*File → Import → CCS Projects*).
2. Vérifier que le chemin d'inclusion pointe vers le BSL de la DSK6713.
3. Compiler, puis charger sur la cible par JTAG.
4. Relier les sorties gauche et droite du codec à l'oscilloscope : chaque canal
   porte une position articulaire, à l'échelle 10 000 unités par radian.

## Mesures relevées

Suivi de consigne des deux articulations observé à l'oscilloscope, comparé au
comportement obtenu en simulation avant portage sur cible. Les relevés et
l'analyse figurent dans `docs/`.

## Limites connues

La dérivée de l'erreur est calculée à partir de la variation de la position
mesurée, `(q - q_prev) / TS`, et non de la variation de l'erreur elle-même.
Comme la consigne est mobile, les deux ne coïncident pas et le signe est opposé
à celui d'une dérivée d'erreur classique : les gains K_DE ont donc été réglés
pour ce montage précis. Un passage à `(e - e_prev) / TS` demanderait de reprendre
ce réglage.

Le pas d'intégration est celui de la boucle temps réel, sans sur-échantillonnage
du modèle : la dynamique est intégrée à 8 kHz, ce qui est confortable ici mais
constitue une contrainte à surveiller si le modèle se complexifie.
