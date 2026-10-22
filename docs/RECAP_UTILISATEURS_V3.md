# Deye Monitor - récapitulatif utilisateur

> Édition 4.6.3 conservée pour les fonctions communes. En 4.7.0, le choix du modèle et du port GEN a changé : consulter le [guide actuel](MISE_A_JOUR_ECRAN_GITHUB.md) et le [guide 4.7.0](GUIDE_UNIFIE_4.7.0.md).
## Firmware unifié 4.6.3

**Édition du 20 octobre 2026**

Un seul firmware pour l'écran GUITION ESP32-S3 480 × 480 du projet. Choisissez votre **modèle Deye** et, si nécessaire, votre **borne de recharge** dans les réglages. Les noms de fichiers V3 sont conservés pour les liens existants.

### Les évolutions à retenir

- **Wi-Fi : reconnexion automatique en 4.6.3.** Après une perte, l'écran laisse cinq secondes au pilote, puis relance une connexion avec les identifiants enregistrés toutes les 30 secondes, sans limite de durée. Au démarrage et après un scan, la première tentative dispose de 30 secondes. DHCP/IP fixe et interface restent disponibles.
- **Mise à jour depuis l'écran.** Ouvrez **Configuration > MISE À JOUR > VÉRIFIER SUR GITHUB > INSTALLER**, puis confirmez. Wi-Fi avec Internet et heure correcte nécessaires. Les réglages sont conservés sur une installation compatible.
- **Scintillement pendant l'installation : c'est normal.** Ne coupez pas l'alimentation jusqu'au redémarrage. Le QR **DOC / GITHUB** ouvre le guide sur le téléphone.
- **Trois types de logger : LSW, LSE et DL1000B.** Ports proposés : 8899, 8899 et 502, modifiables comme l'unité Modbus. Le numéro DL1000B peut contenir des lettres.
- **Recherche du logger depuis l'écran.** Saisissez son vrai numéro, touchez **RECHERCHER**, vérifiez le résultat puis sauvegardez. Si le numéro est non vérifié, choisissez explicitement l'équipement dans **RESULTATS**.
- **Heure réglable sans Internet.** Dans **RÉSEAU / HEURE**, utilisez le réglage manuel. Après une coupure de courant, réglez de nouveau l'heure si NTP reste indisponible.

### Si la box change l'adresse du logger

Avec **LSW / Solarman V5 uniquement**, enregistrez le vrai numéro de série du logger. Après au moins trois lectures échouées et 60 secondes sans réponse V5 valide, Wi-Fi connecté, l'écran recherche le même module. S'il trouve une autre IP, il sauvegarde uniquement cette adresse puis redémarre automatiquement.

Sans résultat ou avec la même IP, aucun redémarrage : une nouvelle recherche peut partir cinq minutes après la fin de la précédente. Un numéro vide, nul ou factice `123456789` désactive cette fonction. Pour **LSE/DL1000B**, contrôlez manuellement l'IP et le port. La reconnexion Wi-Fi 4.6.3 s'applique aux trois types de logger.

[Guide complet](GUIDE_UTILISATEUR_V3.md) · [PDF du récapitulatif](pdf/RECAP_UTILISATEURS_V3.pdf) · [Téléchargements](https://github.com/Chasticot/DEYE_LVGL_GUITION/releases/latest).

<!-- pagebreak -->

## Mesures et fonctions communes

### Choisir le bon modèle

Dans **DEYE / SOLARMAN > MODÈLE DEYE**, choisissez la référence complète, enregistrez puis laissez redémarrer. Le catalogue comprend **12 profils sélectionnables** ; SG06LP1 reste visible mais indisponible. Les adresses, conversions et options sont adaptées au modèle, avec jusqu'à quatre lignes PV. Les réglages V3 restent séparés par modèle ; le Wi-Fi et le logger sont communs.

Les profils LP1 utilisent GEN Daily **R62**, les LP3/HP3 **R536**, avec un coefficient de départ de **0,1 kWh**. AI-W5.1 ESS P1 conserve ses registres monophasés confirmés en 4.3.3 et ne propose pas GEN. Comparez les mesures au LCD ; les retours utilisateurs ne valident pas toutes les révisions et toutes les fonctions.

### Production solaire et GEN

- **Afficher ou masquer PV1 à PV4** règle les lignes visibles et le total instantané, sans couper les panneaux. Le compteur PV journalier reste global.
- **GEN MO + Cumuler GEN MO + PV** ajoute GEN comme une source PV virtuelle au total instantané, au Web, à l'historique et aux kWh du jour. Sa puissance est prise en valeur positive après calibration. Vérifiez que PV Daily n'inclut pas déjà GEN avant de cumuler.
- **GEN Daily configuré** : lecture du compteur. **Adresse 0** : estimation journalière signalée par `~`, sans extrapoler les trous de communication et avec sauvegarde périodique. L'heure doit être correcte.
- **Afficher les kWh GEN Daily** ajoute le compteur dans le bandeau SMARTLOAD/GEN, indépendamment du cumul. Désactivée par défaut. Un registre absent ou une mesure invalide affiche `-- kWh` ; le bandeau n'utilise pas l'estimation à sa place.

### Relais, affichage et tarifs

Le relais **GPIO40** de l'écran à un relais utilise une règle registre × coefficient, un comparateur, un seuil et une temporisation. Il démarre OFF et force OFF après cinq minutes sans nouvelle mesure. Son état ON/OFF apparaît seulement si sa règle est activée. Il est distinct du SmartLoad de l'onduleur.

Thèmes clair/sombre, luminosité jour/nuit et veille horaire sont conservés. Le premier toucher réveille sans activer le bouton dessous. La veille laisse fonctionner les lectures, le Web et les automatismes.

Les tarifs proposent deux plages d'heures creuses ou le mode Tempo. Date, heure et fuseau doivent être corrects. Les restrictions de recharge dépendent du système choisi, comme indiqué en page suivante.

### Web, historique et sauvegarde

Ouvrez **l'IP de l'écran** dans un navigateur du réseau local. Vous y trouverez configuration, tableau de bord, historique jusqu'à 24 h, diagnostic, export/import JSON et mise à jour OTA. L'historique en mémoire vive est effacé au redémarrage.

L'export concerne le modèle actif et les paramètres communs inclus, avec le type de logger et le choix de borne. Il n'inclut ni mot de passe Wi-Fi ni identifiants Web. Faites une sauvegarde avant une mise à jour ou une modification importante.

<!-- pagebreak -->

## Recharge et passage au firmware unifié

### Choisir sa borne

Dans **VÉHICULE ÉLECTRIQUE > ACTIVATION / TARIFS**, choisissez puis enregistrez :

| Choix | Utilisation |
|---|---|
| Aucune | Mesures Deye et fonctions communes, sans client de recharge actif. |
| Deye LoRa | Borne compatible, uniquement sur **SUN-12K-SG02LP1-EU-AM2**. |
| Vetronic WB01 | Passerelle ESP32 compatible, disponible avec les 12 profils de l'écran. |

Une seule recharge est active. **Changer ou désactiver la borne dans l'écran n'arrête pas une charge en cours** : utilisez d'abord sa commande d'arrêt si nécessaire. Un redémarrage n'envoie aucune consigne.

### Deye LoRa

Modes **Arrêt / Solaire / Libre**, avec un plafond de 1 400 à 7 400 W. Le plafond n'est pas une consommation mesurée. Les écritures doivent être vérifiées puis déverrouillées explicitement sur le Web ; elles restent bloquées sans profil ou liaison adaptés.

Les restrictions tarifaires concernent uniquement une charge Libre lancée et confirmée depuis **l'écran tactile pendant le démarrage courant**. Aucune charge autonome ni ancienne pause n'est reprise après redémarrage. Une modification externe, une autre commande locale ou un résultat incertain annulent la reprise tarifaire.

### VE TRONIC WB01

Renseignez l'IP de la passerelle dans la page recharge. Commandes : **Arrêt**, **Charge immédiate**, **Solaire**, **Rendre la main à la borne**. Les boutons +/- préparent le courant ; Charge immédiate applique la demande dans le plafond de la passerelle et la limite locale de 32 A. `~` indique une puissance estimée par courant mesuré × 230 V.

La protection SOC solaire exige l'API compatible de la passerelle, avec au moins cinq points entre MIN et MAX. Les restrictions tarifaires concernent les charges manuelles lancées et confirmées depuis **l'écran ou son Web pendant le démarrage courant**. Arrêt, Solaire ou Rendre la main annulent la reprise.

La passerelle garde son propre firmware et ses propres paramètres Deye. Changer le modèle sur l'écran ne reconfigure pas sa régulation solaire.

### Migrer et vérifier

Exportez le JSON, mettez à jour avec une installation compatible sans effacement, puis vérifiez le modèle et le logger. Depuis V3/Vetronic V3, **choisissez explicitement la borne** : l'ancienne activation VE est ambiguë et ne réactive rien automatiquement. Un ancien JSON avec VE actif sans `ev_backend` est refusé ; sélectionnez la borne puis créez un nouvel export.

Comparez les mesures au LCD et contrôlez la liaison avant toute commande. La 4.6.3 a été compilée et testée par logiciel ; le retour Wi-Fi après redémarrage réel du routeur, le parcours complet de téléchargement depuis le menu et les scénarios de charge restent à vérifier sur matériel. DL1000B reste testé sur simulateur.

[Guide complet](GUIDE_UTILISATEUR_V3.md) · Installation Windows/OTA · Validation 4.6.3.
