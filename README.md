# Projets GSEA : Horacia Azonhoumon

Élève ingénieure en Génie des Systèmes Électronique & Automatique (ENSA Tanger).
Ce dépôt regroupe mes projets académiques : systèmes embarqués, électronique, automatisme industriel et IA appliquée.

**Portfolio :** [HoraPortFolio](https://hora-portfolio.vercel.app/) · **LinkedIn :** [in/horaembedded](https://linkedin.com/in/horaembedded)

---

## Projets

### 01 · Informatique embarquée

| Projet | Description | Technologies |
|---|---|---|
| [Serre intelligente](01_Info_Embarquee/greenhouse) | Régulation climatique bare-metal pilotée par interruptions (Timer1), hystérésis à double seuil, validée en software-in-the-loop | C, ATmega2560, PlatformIO, Wokwi |
| [Machine à états finis](01_Info_Embarquee/fsm-making) | Implémentations comparées d'une FSM (switch-case vs table de transitions) avec tests unitaires Unity et analyse de complexité | C, Unity |
| [Distributeur automatique (FSM)](01_Info_Embarquee/fsm-vending-machine) | Machine à états d'un distributeur, simulée sur microcontrôleur | C, PlatformIO, Wokwi |

### 02 · Automatique

| Projet | Description | Technologies |
|---|---|---|
| [Commande robotique](02_Automatique/commande-robotique) | Bras 2R : PID, cinématique FK/IK, trajectoires lissées (réponse ≈ 0,1 s, dépassement < 5 %) ; robot mobile en logique floue | MATLAB, Simulink, Simscape |
| [Tri convoyeur S7-1200](02_Automatique/tri-convoyeur-s7-1200) | Tri par hauteur en Ladder sur jumeau numérique Factory I/O via OPC-DA, modes Auto/Manuel/Arrêt d'urgence | TIA Portal, Factory I/O |
| [Four thermique FOPDT](02_Automatique/four-thermique-fopdt) | Identification d'un procédé FOPDT et comparaison de méthodes de réglage PID (Ziegler-Nichols, Cohen-Coon, PID Tuner) | MATLAB |
| [Robot vision & trajectoire](02_Automatique/robot-vision-trajectoire) | Suivi de trajectoire d'un robot mobile guidé par vision | Python, OpenCV, Simulink |

### 03 · Conception PCB & électronique

| Projet | Description | Technologies |
|---|---|---|
| [Driver relais NMOS](03_Conception_PCB/driver-relais-nmos) | Commande isolée de 6 relais industriels : dimensionnement, simulation, routage KiCad (Gerber), layout silicium 0,8 µm | KiCad, PSpice, Proteus, Microwind |
| [Nœud capteur IIoT LoRaWAN](03_Conception_PCB/noeud-iiot-lorawan) | Nœud capteur ultra-basse consommation avec récupération d'énergie — en cours | KiCad, LoRaWAN |

### 04 · VHDL / FPGA

En cours de formation - premiers projets à venir.

---

## Organisation

Chaque projet contient un `README.md` (problème → solution → résultats), les sources, et un dossier `docs/` (rapport, captures).

## Contact

azonhoumonhoracia@gmail.com · Tanger, Maroc
