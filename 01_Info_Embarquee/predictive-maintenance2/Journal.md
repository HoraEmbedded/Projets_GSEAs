
## Résumé de séance — vendredi 7 août 2026 (fin de S1)

**Fait aujourd'hui**
- Micro-étape 1.2 validée : Ubuntu 26.04 LTS, Git 2.53, compte Kaggle créé (`horaembedded`).
- Incident technique identifié : Python système en 3.14.4, **incompatible avec TensorFlow** (support officiel : 3.10 à 3.13). Bloquant pour EF04, exigence essentielle. Parade : environnement virtuel sur Python 3.13.
- Calendrier complété : soutenance le 18 octobre, point hebdomadaire le samedi, 9 jours de marge après la fin du planning.
- Micro-étape 1.5 exécutée : cahier des charges complété, réunion de cadrage tenue, compte rendu rédigé et diffusé.
- **D1 acquis.** Six décisions actées avec l'encadrant (D-01 à D-06).
- Reçu le classeur de suivi (Gantt, livrables, exigences).

**Décisions**
- D-02 : pas de plancher de précision, arbitrage par le coût total Scania.
- D-04 : le modèle final doit coûter au moins 50 % de moins que la règle naïve.
- D-05 : grille de critères et trois candidats présentés en S2 ; MetroPT-3 remplace AI4I 2020.
- D-06 : venv Python 3.11–3.13 pour la partie DL.

**Ouvert / à corriger**
- D-03 non tranchée : la règle naïve de référence n'est pas choisie, ce qui rend D-04 invérifiable.
- Cahier des charges passé en v2.0 avec de nouveaux identifiants sans procédure d'avenant — à me transmettre.
- Erreurs de date dans le compte rendu : le 8 août est un samedi ; le prochain point est le 15, pas le 14.

**À faire pour la prochaine séance**
1. Calculer le coût des deux règles naïves sur ~16 000 exemples de test dont ~375 pannes, et conclure sur D-03.
2. M'envoyer le cahier des charges v2.0.
3. Micro-étape 1.3 : dépôt Git et arborescence du projet.
4. Question jury en attente : comment garantir la reproductibilité (ENF03) ?

Noté pour le journal : des comptes rendus détaillés, pas des résumés. J'adapte à partir de celui-ci.

**Micro-étape 1.4 : validée.** Python 3.13.15 dans le venv, `which python` pointe bien vers `venv/bin/python`, `requirements.txt` généré, et `git status` ne montre aucun fichier du venv — ton `.gitignore` fait son travail. C'est propre.

Deux choses avant de tourner la page.

**Commite ton `requirements.txt`.** Il est modifié et non validé. Un fichier de dépendances qui traîne hors du dépôt ne sert à personne :

```bash
git add requirements.txt && git commit -m "chore: pin Python 3.13 environment dependencies" && git push
```

**Ce que contient ce fichier, et pourquoi c'est un choix.** `pip freeze` a listé une centaine de paquets — `anyio`, `argon2-cffi`, `arrow`… que tu n'as jamais demandés. Ce sont des **dépendances transitives** : des bibliothèques dont dépendent celles que tu as installées. Jupyter en tire à lui seul une quarantaine.

Deux écoles s'affrontent là-dessus. Ne lister que tes six paquets directs donne un fichier lisible, mais deux personnes qui l'installent à six mois d'écart n'obtiennent pas les mêmes versions des dépendances — donc pas forcément les mêmes résultats. Tout figer, comme tu viens de le faire, donne un fichier illisible mais **rigoureusement reproductible**. L'ENF03 exige la reproductibilité : ton choix est le bon. Garde-le, et sache l'expliquer si on te le demande. Ajoute simplement dans ton README la liste de tes six paquets directs, pour la lisibilité humaine.

Et n'oublie pas les `.gitkeep` de mon message précédent — sans eux, tes dossiers vides n'existent pas sur GitHub.

---

**Deux questions restent sans réponse** de ta part. Je les garde ouvertes, elles ne se périment pas :
1. La précision qu'implique réellement le critère D-04 (mon calcul du budget de 78 125).
2. Pourquoi fixe-t-on une graine aléatoire — qu'est-ce qui est aléatoire dans un entraînement ?

---

## Micro-étape 2.1 — Construire la grille de sélection du dataset

**Durée** : 1 h 30 · **Couvre** : O1, prépare **D2** · **Semaine 2, jour 1** · échéance : jalon J1, samedi 15 août

**Pourquoi.** Tu m'as reproché, au jour 2, de t'imposer un dataset. Tu avais raison, et voici ta revanche : c'est toi qui vas définir les critères, les pondérer, et trancher. Le livrable D2 s'appelle *« choix du dataset justifié »* — justifier, c'est démontrer une méthode, pas annoncer un résultat. En soutenance, la question *« pourquoi celui-là ? »* tombera à coup sûr.

**Comment.** Tu construis un tableau : les critères en lignes, les trois candidats en colonnes (Scania APS, MetroPT-3, Engine Health). Voici le squelette — à toi de le remplir et surtout de le compléter.

| Code | Critère | Ce qu'on mesure | Poids |
|---|---|---|---|
| C1 | Données réelles | industrielles, non simulées | ? |
| C2 | Pertinence au freinage | lien direct avec le composant visé | ? |
| C3 | Volume | nombre d'exemples disponibles | ? |
| C4 | Dimensionnalité | nombre de variables capteurs | ? |
| C5 | Qualité de l'étiquetage | étiquettes binaires fiables et documentées | ? |
| C6 | Métrique de coût fournie | le dataset apporte-t-il ses coûts d'erreur ? | ? |
| C7 | ... | à toi | ? |
| C8 | ... | à toi | ? |

Trois consignes de méthode :

- **Ajoute au moins deux critères de ton cru.** Pense à ce qui peut te bloquer concrètement : licence d'utilisation, accessibilité du téléchargement, existence de publications de référence pour te comparer, charge de calcul sur ta machine, présence de valeurs manquantes.
- **Pondère.** Tous les critères ne se valent pas. Un poids de 1 à 3, et tu dois pouvoir défendre chaque poids. C1 découle d'une exigence explicite de l'encadrant — quel poids ça mérite ?
- **Note de 0 à 5, et justifie chaque note en une ligne.** Une note sans justification est une opinion.

Ne remplis pas encore les colonnes des candidats : tu n'as pas les informations. Cette séance produit **la grille vide, pondérée et argumentée**. La collecte des informations sur les trois datasets sera la 2.2.

**Critère de réussite.** Un fichier `docs/grille_selection_dataset.md` dans ton dépôt, contenant au moins 8 critères, chacun avec un poids justifié en une phrase. Commité et poussé.

**Le piège à éviter,** et je te le dis maintenant parce que tous les étudiants y tombent : ne construis pas la grille pour qu'elle donne le résultat que tu as déjà en tête. Si tu inventes un critère « anonymisation des variables » avec un poids de 3 uniquement parce que Scania coche la case, tu fabriques une justification, pas une méthode. Un jury le voit immédiatement. Construis la grille comme si tu ne savais pas encore laquelle gagnera.

---

## Compte rendu de séance — vendredi 7 août 2026

**Cadre** · Fin de la semaine 1. Environ 3 h de travail effectif. Semaine 2 démarre demain, samedi 8 août, avec le point hebdomadaire.

**Travaux réalisés**

*Micro-étape 1.3 — Dépôt Git.* Création du dépôt `predictive-maintenance-aps` sous `~/`, initialisation et publication sur GitHub via `gh repo create` en mode public. Arborescence retenue : `data/raw`, `data/processed`, `notebooks`, `src`, `models`, `reports`, `docs`. Principe posé : `data/raw` est en lecture seule, tout traitement produit un nouveau fichier dans `processed`. Le `.gitignore` exclut `data/`, `models/`, `venv/`, `__pycache__/`, `.ipynb_checkpoints/`. Premier commit poussé, arbre de travail propre. Réserve ouverte : Git ne versionnant pas les dossiers vides, les fichiers `.gitkeep` restent à ajouter pour que la structure soit visible par un tiers.

*Micro-étape 1.4 — Environnement d'exécution.* Installation de Python 3.13 en parallèle du 3.14 système, sans y toucher. Création du venv sur 3.13, mise à jour de pip, installation du socle : numpy, pandas, scikit-learn, matplotlib, seaborn, jupyter. Gel des versions par `pip freeze`. Vérifications concluantes : `python --version` renvoie 3.13.15, `which python` pointe vers `venv/bin/python`, `git status` confirme que le venv est bien exclu du versionnage. TensorFlow et XGBoost volontairement non installés à ce stade.

*Décision D-03 — Règle naïve de référence.* Calcul mené sur le jeu de test (≈ 16 000 exemples, ≈ 375 pannes). Règle « toujours sain » : 375 faux négatifs × 500 = **187 500**. Règle « toujours défaillant » : 15 625 faux positifs × 10 = **156 250**. La seconde étant la moins coûteuse, c'est elle qui devient la référence à battre. Conclusion notable, et contre-intuitive : compte tenu du rapport de coût 50:1, envoyer tous les camions au contrôle coûte moins cher que d'en laisser tomber en panne.

**Observations à retenir**

Le jeu de test contient environ 2,34 % de positifs, alors que la section 5 annonce 1,7 % pour l'entraînement. Les deux jeux n'ont donc pas la même proportion de pannes. À garder en tête en semaine 4, au moment de comparer les scores de validation croisée aux scores de test.

Le critère D-04 (coût inférieur de 50 % à la règle naïve, soit un budget de 78 125) est moins exigeant qu'il n'y paraît : un modèle à 90 % de rappel consomme 19 000 en faux négatifs et conserve environ 5 900 fausses alertes autorisées. Calcul de la précision correspondante à faire.

Le choix d'un `requirements.txt` issu d'un `pip freeze` complet est assumé : il privilégie la reproductibilité stricte (ENF03) sur la lisibilité.

**Anomalie à corriger**

Le compte rendu de la réunion du 8 août mentionne un cahier des charges « v2.0 » et des identifiants (O-01, O-02, C-01 à C-05) qui n'existent pas dans le document de référence, resté en v1.0. Dates également erronées : le 8 août est un samedi, et le prochain point hebdomadaire tombe le 15, non le 14. Document à rectifier et à rediffuser à l'encadrant.

**État d'avancement**

Semaine 1 close, D1 acquis. ENF01, ENF02 et ENF03 engagées. Micro-étapes 1.1 à 1.4 validées. Aucun retard.

**En attente pour la prochaine séance**

Commit du `requirements.txt` et ajout des `.gitkeep`. Correction et rediffusion du compte rendu de réunion. Réponse aux deux questions ouvertes : précision impliquée par D-04, et rôle de la graine aléatoire. Micro-étape 2.1 : construction de la grille de sélection du dataset, échéance jalon J1 le samedi 15 août.

**Ce qui m'a bloquée :** 
**Ce que j'ai compris :** 