

# Mini-leçon n°3 — Fabriquer du continu réglable avec un interrupteur

*Objectif : comprendre le convertisseur buck. C'est le cœur matériel de ton projet.*

## Le problème à résoudre

Tu veux 13 V à partir de 18 V, sans dissiper la différence. Tu as vu qu'une résistance ne convient pas (25 W de chaleur). Il te faut un composant qui règle la tension **sans jamais dissiper** — donc un composant qui n'a que deux états :

- **fermé** : tension nulle à ses bornes → *P = U·I = 0 × I = 0*
- **ouvert** : courant nul → *P = U·I = U × 0 = 0*

Un **interrupteur idéal ne dissipe jamais rien**, dans aucun de ses deux états. C'est la seule échappatoire au dilemme. Le prix à payer : il est tout ou rien, donc en sortie tu obtiens du **haché**, pas du continu.

## Le rapport cyclique

On ouvre et ferme très vite, périodiquement. Sur une période T :

$$D = \frac{t_{\text{fermé}}}{T} \qquad \text{(duty cycle, entre 0 et 1)}$$

La sortie est un créneau qui vaut 18 V pendant D·T, puis 0 V pendant le reste. Sa **valeur moyenne** :

$$\overline{V} = D \times V_{in}$$

Avec D = 0,72 et Vin = 18 V → **12,96 V**. Ta tension batterie, obtenue sans dissiper un watt.

**C'est l'équation reine de ton projet.** Tout le MPPT consiste à faire varier D pour déplacer le point de fonctionnement du panneau.

## Le filtre : là où l'inductance entre en scène

Une valeur moyenne de 13 V, ce n'est pas 13 V continu — c'est un créneau brutal 0 V / 18 V. Il faut le lisser. Et tu connais déjà les deux outils :

- **L'inductance** s'oppose aux variations de **courant** → elle absorbe le hachage et laisse passer un courant quasi constant.
- **Le condensateur** s'oppose aux variations de **tension** → il maintient la sortie stable entre deux impulsions.

Un **filtre LC**, donc. Et tu peux maintenant relire la relation `v = L·di/dt` avec un œil neuf : quand l'interrupteur est fermé, l'inductance voit (18 − 13) = 5 V et son courant **monte en rampe** ; quand il s'ouvre, elle voit −13 V et son courant **descend en rampe**. Le courant oscille autour de sa moyenne au lieu de sauter. Cette oscillation s'appelle l'**ondulation de courant** (current ripple) :

$$\Delta I_L = \frac{(V_{in} - V_{out}) \times t_{\text{fermé}}}{L}$$

## La diode de roue libre — ta question 5, en application

Interrupteur fermé, courant de 7 A dans l'inductance. Tu ouvres. **Que veut faire ce courant ?**

Il veut continuer — c'est l'inertie. S'il ne trouve aucun chemin, `di/dt → ∞`, donc `v → ∞` : **la bobine d'allumage, et ton MOSFET détruit.** Exactement le mécanisme que tu as noté en rouge.

On lui offre donc un chemin : une **diode de roue libre** (freewheeling diode), qui conduit dès que l'interrupteur s'ouvre et laisse le courant boucler. Ce n'est pas une protection ajoutée par prudence : **c'est un organe fonctionnel indispensable.** Sans elle, pas de buck.

> **Note bien ce que tu viens de faire :** tu as appris un phénomène destructeur en mini-leçon 2, et il devient un composant obligatoire du schéma en mini-leçon 3. C'est ainsi que se construit l'électronique de puissance — chaque contrainte physique dicte un composant.

## Pourquoi ton projet exige un buck *synchrone*

Une diode dissipe : `P = V_f × I`. Une diode Schottky à 0,4 V parcourue par 7 A pendant 28 % du temps :

$$P = 0{,}4 \times 7 \times 0{,}28 \approx 0{,}8 \text{ W}$$

Sur 100 W, c'est **0,8 % de rendement perdu**, et ta spécification exige > 95 %. D'où la solution industrielle : remplacer la diode par un **second MOSFET** piloté en opposition. Un MOSFET à 10 mΩ dissipe `R·I² × 0,28 = 0,14 W`, soit **six fois moins**. C'est ça, un **buck synchrone** — le mot exact de ta fiche projet, que tu peux maintenant expliquer.

## Pièges classiques annoncés

1. **`Vout = D × Vin` n'est vraie qu'en régime établi, en conduction continue (CCM) et avec des composants idéaux.** À faible charge, le convertisseur bascule en conduction discontinue (DCM) et la relation change. Tu rencontreras ça au petit matin, quand le panneau produit peu.
2. **« Augmentons la fréquence, l'ondulation diminue »** — vrai, mais chaque commutation coûte de l'énergie (pertes de commutation). Fréquence ↑ → filtre plus petit **mais** pertes ↑. Arbitrage central de P3.
3. **Ne confonds pas ondulation de courant et ondulation de tension.** ΔI_L dépend de L ; ΔV_out dépend de C. Ce sont deux dimensionnements séparés.

---

## Ta prochaine action concrète

### A. Ton premier convertisseur buck sous LTspice *(2 h)*

Squelette — tu construis, je ne te donne pas le fichier.

**Composants** (`F2` pour placer, `F3` pour câbler, `G` pour la masse) :

| Élément | Recherche `F2` | Valeur |
|---|---|---|
| Source d'entrée | `voltage` | 18 |
| Interrupteur commandé | `sw` | (voir modèle ci-dessous) |
| Source de commande | `voltage` | `PULSE(0 5 0 1n 1n 7.2u 10u)` |
| Diode de roue libre | `diode` | par défaut |
| Inductance | `ind` | 100µ |
| Condensateur | `cap` | 100µ |
| Charge | `res` | 1.7 |

**Topologie :** V1(+) → interrupteur → nœud commun ; de ce nœud commun part l'inductance vers la sortie ; la diode relie la masse à ce même nœud commun (**cathode côté nœud commun** — la bande de la diode vers le haut) ; le condensateur et la charge sont en parallèle entre la sortie et la masse.

**Directives** (touche `S`) :
```
.tran 2m
.model MYSW SW(Ron=10m Roff=1Meg Vt=2.5)
```
Puis clic droit sur l'interrupteur → dans le champ valeur, écris `MYSW`.

**La commande** (`V2`, le PULSE) se place entre la borne de commande de l'interrupteur et la masse. Un interrupteur `sw` a quatre bornes : deux de puissance, deux de commande.

**Ce que tu observes :** la tension de sortie, le courant dans l'inductance, la tension sur le nœud commun. Zoome sur les 100 dernières µs pour voir l'ondulation.

### B. Prédire avant de mesurer *(1 h, carnet)*

**La règle du projet, à partir de maintenant : tu écris ta prédiction dans le carnet AVANT de lancer la simulation.** Une simulation qui confirme un calcul est une preuve. Une simulation qu'on regarde sans rien attendre n'est qu'un joli dessin.

1. Vout attendue avec D = 0,72 ?
2. Ondulation de courant ΔI_L attendue ? *(Vin − Vout = 5 V, t_fermé = 7,2 µs, L = 100 µH)*
3. Ondulation de tension de sortie, avec `ΔV = ΔI / (8·f·C)` ?
4. Quel D faut-il pour charger la batterie à 14,4 V ? Quel t_fermé programmer dans le PULSE ?
5. Si la fréquence tombe à 20 kHz (T = 50 µs, D inchangé), que devient ΔI_L ? **En quoi est-ce un problème pour ton algorithme MPPT ?**

Puis compare chaque prédiction à la simulation. **Note les écarts et cherche leur cause avant de m'appeler.**

<details>
Corrigé numérique B — à consulter APRÈS : 1) 12,96 V (mesure attendue plus basse, ~12,4 V) · 2) 360 mA · 3) 4,5 mV · 4) D = 0,80, soit t_fermé = 8 µs · 5) 1,8 A, soit 5× plus
</details>

*Indice pour l'écart de la question 1 : regarde la tension aux bornes de la diode quand elle conduit. Ce que tu perds là, c'est précisément l'argument du buck synchrone.*

### C. Le C, en tâche de fond *(1 h — condition 4)*

Tu ne m'en as pas parlé. Rappel : **VS Code + MinGW-w64 installés, premier `hello world` compilé en ligne de commande.** Une heure, pas plus. Si tu bloques sur l'installation, dis-le-moi — c'est la seule étape du parcours C où l'aide vaut mieux que l'acharnement.

---

## Questions de contrôle

1. Un interrupteur idéal ne dissipe rien dans aucun de ses deux états. Alors **pourquoi un MOSFET réel chauffe-t-il malgré tout ?** *(Deux causes distinctes — l'une pendant les états, l'autre entre les états.)*
2. Reprends ton tableau linéaire vs buck de la mini-leçon 2. Tu peux maintenant l'expliquer complètement : **d'où vient physiquement le courant supplémentaire en sortie du buck ?** Le convertisseur crée-t-il du courant ?
3. Ton algorithme MPPT mesure la tension et le courant du panneau pour décider s'il augmente ou diminue D. **Que se passe-t-il s'il échantillonne au mauvais moment dans le cycle de découpage ?** *(C'est le premier vrai couplage matériel/logiciel du projet, et il reviendra en P4.)*

## Note pour le journal de compétences

> **S0-4 — La cellule de commutation et le convertisseur buck.**
> Correction acquise : l'inductance n'est pas un réservoir mais un organe de **transfert** — 1,25 mJ par cycle × 100 kHz = 125 W transportés. Principe fondateur de l'électronique de puissance.
> Un interrupteur idéal ne dissipe rien (U = 0 fermé, I = 0 ouvert) : c'est la seule façon de régler une tension sans chaleur. Rapport cyclique D = t_on/T, relation reine **Vout = D · Vin** (valable en régime établi, CCM, composants idéaux). Filtre LC : L lisse le courant, C lisse la tension. Ondulation ΔI_L = (Vin−Vout)·t_on/L. Diode de roue libre = organe fonctionnel obligatoire, réponse directe à la surtension de rupture d'inductance ; remplacée par un second MOSFET (buck **synchrone**) pour gagner ~0,7 % de rendement.
> **Méthode :** test de vraisemblance systématique avant d'écrire un résultat (repère : 1 panneau 100 W ≈ 200 dh/an) · raisonner en pourcentage avant d'arrondir · un argument sans chiffre est une opinion · 1 Wh = 3600 J · **prédire avant de simuler**.
> **🚦 Go/no-go n°1 validé** — circuit RL simulé et expliqué, τ = L/R vérifiée par lecture graphique et par calcul.

Beau travail. Continue à ce rythme et les 43 semaines deviendront un plafond, pas une prévision.