# TPs Commande Robotique : Manipulateur 2R & Robot Mobile

> Modélisation, commande et simulation de robots manipulateurs (2 DDL) et mobiles (unicycle) sous MATLAB/Simulink.
> **ENSA Tanger · GSEA2 · 2025-2026**, supervisé par Prof. Khadija EL HAMIDI

## Contenu

| TP | Sujet | Outils clés |
|----|-------|-------------|
| TP1 | Commande articulaire PID d'un robot 2R | Simscape Multibody, Simulink |
| TP2 | Cinématique directe (FK) / inverse (IK) & génération de trajectoires | Robotics System Toolbox |
| TP3 | Commande d'un robot mobile par logique floue | Fuzzy Logic Toolbox |

## TP1 : Commande PID d'un robot 2 DDL

Bras RR (L1 = 0.5 m, L2 = 0.4 m) modélisé sous **Simscape Multibody** (Solid, Revolute Joint, Rigid Transform), commandé articulation par articulation :

τᵢ = Kpᵢ·(qᵢ,ref - qᵢ) + Kiᵢ·∫(qᵢ,ref - qᵢ)dt + Kdᵢ·(q̇ᵢ,ref - q̇ᵢ)

**Gains retenus après réglage :**

| Joint | Kp | Ki | Kd |
|-------|-----|----|----|
| 1 | 100 | 25 | 25 |
| 2 | 100 | 15 | 40 |

**Résultats :** temps de réponse d'environ 0.1 s (échelon), erreur statique nulle, dépassement < 5 %. Sur trajectoire sinusoïdale (q1d = 0.5·sin(0.5t), q2d = 0.3·sin(0.5t + π/2)) : suivi stable, erreur d'environ 0.2°, oscillations négligeables.

**Limite observée :** le PID classique reste sensible aux non-linéarités et au couplage dynamique entre articulations.

## TP2 : Cinématique FK / IK & trajectoires

**FK** (bloc *Get Transform*) : validation simulation vs théorie sur 4 configurations, conformité exacte.

x = L1·cos(θ1) + L2·cos(θ1+θ2) ; y = L1·sin(θ1) + L2·sin(θ1+θ2)

**Espace de travail :** anneau de rayons R_min = |L1-L2| = 0.1 m à R_max = L1+L2 = 0.9 m.

**IK** (bloc *Inverse Kinematics*, weights [0 0 0 1 1 1]) : suivi validé sur rampe puis trajectoire circulaire, sortie conforme à la consigne.

**Génération de trajectoire** (*Polynomial Trajectory*) : trajectoire carrée ; l'ajout d'un *Rate Limiter* (contrainte vitesse/accélération) arrondit les sommets mais stabilise nettement les couples envoyés aux joints.

**Singularités :** det(J) = L1·L2·sin(q2) = 0 : bras tendu (q2 = 0°, singularité externe) ou replié (q2 = 180°, singularité interne).

## TP3 : Robot mobile & logique floue

Robot **unicycle** (R roues = 0.05 m, demi-empattement L = 0.07 m) :

ẋ = V·cos(θ) ; ẏ = V·sin(θ) ; θ̇ = ω, avec V = (vd+vg)/2 et ω = (vd-vg)/2L

**Contrôleur flou Mamdani** (défuzzification par centroïde : commande précise, directement exploitable par les moteurs) :
- Entrées : distance à la cible `Drc`, erreur angulaire `theta_rc`
- Sorties : vitesses des roues `vg`, `vd`
- 9 règles (3 niveaux de distance × 3 d'erreur angulaire)

**Résultats** (ralliement du point (2, 2)) : suivi précis, **erreur latérale < 0.05 m**, robustesse aux perturbations. Augmenter le nombre de règles améliore la précision ; ajuster les fonctions d'appartenance réduit les oscillations.

## Structure du dépôt

```
├── TP1_PID/
│   ├── Robot2R_PID.slx        # Modèle Simscape + PID
│   └── Robot2R_PID.m          # Simulation par script
├── TP2_Cinematique/
│   ├── Robot2R_FK.slx         # Cinématique directe
│   ├── Robot2R_IK.slx         # Cinématique inverse
│   └── Robot2R_IK_Traj.slx    # Trajectoires polynomiales
├── TP3_Fuzzy/
│   ├── robot_mobile.slx       # Modèle unicycle + contrôleur
│   └── controleur.fis         # Système d'inférence flou
├── docs/
│   └── Rapport_TPs_Robotique.pdf
└── README.md
```

## Prérequis

- MATLAB R2023a+ avec **Simulink**
- Toolboxes : Simscape Multibody, Robotics System Toolbox, Fuzzy Logic Toolbox

## Utilisation

```matlab
% TP1 : simulation par script
run('TP1_PID/Robot2R_PID.m')

% Ou ouvrir un modèle Simulink et lancer la simulation
open_system('TP2_Cinematique/Robot2R_IK.slx')
```

## Licence & auteur

**AZONHOUMON H. Horacia Gloriéta** ([@HoraEmbedded](https://github.com/HoraEmbedded)). Libre pour usage académique.
